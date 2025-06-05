package main

import (
	"bytes"
	"encoding/binary"
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"strconv"
	//"net/http/httputil"

	"database/sql"
	"os"
	_ "github.com/mattn/go-sqlite3"
)

var procDatabase *sql.DB

type ProcessData struct {
	ImageSize uint64
	ProcName  string //Maybe use an array so its not malloced somewhere else?
	ProcCount uint64
}

type ProcessDetailsRequest struct {
	ID       string `json:"id"`
	ImgSize  string `json:"img_size"`
	ProcName string `json:"proc_name"`
}

var process_map map[uint64]ProcessData

func PrintMap() {
	fmt.Println("--------------------------- TABLE -------------------------")
	for k, v := range process_map {
		fmt.Printf("|%20d|%10d|%-100s|%10d| \n", k, v.ImageSize, v.ProcName, v.ProcCount)
	}
}

func createProcessTable(db *sql.DB) {
	// Maybe add a separate table like HOST | TIME | HASH SENT 	
	// And maybe only log to this table when suspicious processes are sent

	createProcTableSQL := `
		CREATE TABLE IF NOT EXISTS processes (
			"hash" TEXT NOT NULL PRIMARY KEY,
			"img_size" INTEGER,
			"proc_name" TEXT,
			"proc_count" INTEGER DEFAULT 1
		);`
	// TODO:
	// make the process count autoincrement
	// Add last sent by host|time to table or add a separate host table 
	statement, err := db.Prepare(createProcTableSQL)
	if err != nil {
		log.Fatal(err.Error())
	}
	statement.Exec()
	log.Println("Process table created")
}

func insertProcess(db *sql.DB, procHash uint64, procData ProcessData) {
	insertProcSql := `
		REPLACE INTO processes(hash, img_size, proc_name)
		VALUES (?, ?, ?)`
	statement, err := db.Prepare(insertProcSql)
	if err != nil {
		log.Fatal(err.Error())
	}
	// NOTE: Mabe format has to base 16, but then its not clear its a uint64 hash
	_, err = statement.Exec(strconv.FormatUint(procHash, 10), procData.ImageSize, procData.ProcName)
	if err != nil {
		log.Fatal(err.Error())
	}
}

func updateProcessCount(db *sql.DB, procHash uint64) {
	updateProcSql := `
		UPDATE processes SET proc_count = proc_count + 1 WHERE hash = ?`
	statement, err := db.Prepare(updateProcSql)
	if err != nil {
		log.Fatal(err.Error())
	}
	_, err = statement.Exec(strconv.FormatUint(procHash, 10))
	if err != nil {
		log.Fatal(err.Error())
	}
}

func HashesRequestProcessDetails(w http.ResponseWriter, req *http.Request) {
	// get all details of processes
	str, err := io.ReadAll(req.Body)
	if err != nil {
		log.Fatal("request", err)
	}

	var values []ProcessDetailsRequest
	err = json.Unmarshal([]byte(str), &values)
	if err != nil {
		log.Fatal("json unmarshal", err)
	}

	for _, val := range values {
		uid, _ := strconv.ParseUint(val.ID, 10, 64)
		// TODO: add check if hash is actually unknown
		curproc, ok := process_map[uid]
		if ok {
			imgs, _ := strconv.Atoi(val.ImgSize)
			thisProcData := ProcessData{uint64(imgs), val.ProcName, curproc.ProcCount}
			process_map[uid] = thisProcData
			// TODO: insert(replace) process details to database
			insertProcess(procDatabase, uid, thisProcData)
		} else {
			log.Printf("Process details sent for nonexistent hash: %d", uid)
		}
	}
}

func HashesRequestRead(w http.ResponseWriter, req *http.Request) {
	// res, err := httputil.DumpRequest(req, true)
	// if err != nil {
	// 	log.Fatal(err)
	// }
	// fmt.Println(string(res))

	// Read hashes from request
	fmt.Println(req.Header.Get("PostFieldSize"))
	buffer, err := io.ReadAll(req.Body)
	if err != nil {
		log.Fatal("request", err)
	}
	fmt.Println(buffer)
	hash_buf := bytes.NewReader(buffer)
	var num_hashes = len(buffer) / 8
	var hashes = make([]uint64, num_hashes)
	var unknown_hashes = make([]uint64, 0)
	binary.Read(hash_buf, binary.LittleEndian, &hashes)
	fmt.Println(hashes)

	// Add hashes to map
	for _, h := range hashes {
		curproc, ok := process_map[h]
		if ok {
			process_map[h] = ProcessData{curproc.ImageSize, curproc.ProcName, curproc.ProcCount + 1}
			// TODO: increase count of process in database
			updateProcessCount(procDatabase, h)
		} else {
			unknown_hashes = append(unknown_hashes, h)
			process_map[h] = ProcessData{0, "no_name", 1}
			// TODO: add unknown process to database
			insertProcess(procDatabase, h, ProcessData{0, "unknown_process", 1})
		}
		// if some data is unknown, http response to ask for details
	}

	//Write response for unknown hashes
	w_buf := new(bytes.Buffer)
	binary.Write(w_buf, binary.LittleEndian, &unknown_hashes)
	w.Header().Set("Content-Type", "application/octet-stream")
	w.Header().Set("Content-Length", strconv.Itoa(w_buf.Len()))
	w.Write(w_buf.Bytes())

	PrintMap()
}

func main() {
	process_map = make(map[uint64]ProcessData)
	process_map[100] = ProcessData{999, "first_process", 1}


	_, err_file := os.Stat("process-database.db")
	if (os.IsNotExist(err_file)) {
		log.Println("Creating process-database.db")
		file, err := os.Create("process-database.db")
		if err != nil {
			log.Fatal(err.Error())
		}
		file.Close()
		log.Println("process-database.db created")
	}

	procDatabase, _ = sql.Open("sqlite3", "./process-database.db")
	defer procDatabase.Close()
	createProcessTable(procDatabase)

	http.HandleFunc("/send_hash", HashesRequestRead)
	http.HandleFunc("/send_proc_data", HashesRequestProcessDetails)

	log.Println("Server listening on port 1337")
	log.Fatal(http.ListenAndServe(":1337", nil))
}
