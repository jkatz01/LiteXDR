package main

import (
	"bytes"
	"crypto/sha1"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

type ProcessInfo struct {
	name string
	path string
}

func HashProcessInfo(sourceProcess ProcessInfo) string {
	//Function to convert a ProcessInfo struct into a single hash and returns it as a string
	processString := fmt.Sprintf("%s%s", sourceProcess.name, sourceProcess.path) //string concatenation
	processHash := sha1.New()
	processHash.Write([]byte(processString))
	hashedString := string(processHash.Sum(nil)) //gives the hash in a (byte slice converted to) string format
	return hashedString
}

// Add local cache???

func main() {

	testProcess := ProcessInfo{
		name: "Cool Process",
		path: "D/whatever/folder",
	}

	var combinedHash string
	combinedHash = HashProcessInfo(testProcess)

	values := map[string]string{"key": combinedHash}
	json_data, err := json.Marshal(values)

	if err != nil {
		log.Fatal(err)
	}

	resp, err := http.Post("http://localhost:1037/check_hash", "application/json",
		bytes.NewBuffer(json_data))

	if err != nil {
		log.Fatal(err)
	}

	defer resp.Body.Close()

	var res map[string]interface{}

	json.NewDecoder(resp.Body).Decode(&res)

	fmt.Println(res["json"])
}
