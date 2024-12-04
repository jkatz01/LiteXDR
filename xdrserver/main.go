package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"net/http"
	"strconv"
	//"net/http/httputil"
)

type ProcessData struct {
	image_size uint64 
	proc_name  string //Maybe use an array so its not malloced somewhere else?
	
	proc_count uint64
}

var process_map map[uint64]ProcessData

func PrintMap() {
	fmt.Println("--------------------------- TABLE -------------------------")
	for k, v := range process_map {
		fmt.Printf("|%10d|%6d|%32s|%6d| \n" ,k, v.image_size, v.proc_name, v.proc_count)
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
			process_map[h] = ProcessData{0, "no_name", curproc.proc_count + 1}
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

	http.HandleFunc("/check_hash", HashesRequestRead)
	log.Println("Server listening on port 1337")
	log.Fatal(http.ListenAndServe(":1337", nil))
}
