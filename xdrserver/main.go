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
)

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
		fmt.Printf("|%10d|%6d|%32s|%6d| \n" ,k, v.ImageSize, v.ProcName, v.ProcCount)
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
		id, _ := strconv.Atoi(val.ID)
		uid := uint64(id)
		// TODO: add check if hash is actually unknown
		curproc, ok := process_map[uid]
		if ok {
			imgs, _ := strconv.Atoi(val.ImgSize)
			process_map[uid] = ProcessData{uint64(imgs), val.ProcName, curproc.ProcCount }
		} else {
			log.Println("Process details sent for nonexistent hash")
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
		} else {
			unknown_hashes = append(unknown_hashes, h)
			process_map[h] = ProcessData{0, "no_name", 1}
		}
		// if some data is unknown, http response to ask for details
	}

	//Write response for unknown hashes 
	w_buf := new(bytes.Buffer)
	binary.Write(w_buf, binary.LittleEndian, &unknown_hashes)
	w.Header().Set("Content-Type", "application/octet-stream")
	w.Header().Set("Content-Length", strconv.Itoa( w_buf.Len() ))
	w.Write(w_buf.Bytes())

	PrintMap()
}

func main() {
	process_map = make(map[uint64]ProcessData)
	process_map[100] = ProcessData{999, "first_process", 1}

	http.HandleFunc("/send_hash", HashesRequestRead)
	http.HandleFunc("/send_proc_data", HashesRequestProcessDetails)

	log.Println("Server listening on port 1337")
	log.Fatal(http.ListenAndServe(":1337", nil))
}
