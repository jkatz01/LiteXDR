package main

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"io"
	"log"
	"net/http"
	"net/http/httputil"
)

func HashesRequestRead(w http.ResponseWriter, req *http.Request) {
	res, err := httputil.DumpRequest(req, true)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(string(res))
	// Some kind of problem when reading 0 in a byte
	fmt.Println(req.Header.Get("PostFieldSize"))
	buffer, err := io.ReadAll(req.Body)
	if err != nil {
		log.Fatal("request", err)
	}
	fmt.Println(buffer)
	hash_buf := bytes.NewReader(buffer)
	var num_hashes = len(buffer) / 8
	var hashes = make([]uint64, num_hashes)
	binary.Read(hash_buf, binary.LittleEndian, &hashes)
	fmt.Println(hashes)
}

func main() {
	http.HandleFunc("/check_hash", HashesRequestRead)
	log.Println("Server listening on port 1337")
	log.Fatal(http.ListenAndServe(":1337", nil))
}
