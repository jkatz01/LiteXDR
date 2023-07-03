package main

import (
	"bytes"
	"crypto/sha1"
	"fmt"
	"io/ioutil"
	"net/http"
)

type ProcessInfo struct {
	name string
	path string
}
type HashRequest struct {
	Key string `json:"key"`
}

type HashResponse struct {
	RetMsg bool `json:"ret_msg"`
}

func HashProcessInfo(sourceProcess ProcessInfo) string {
	//Function to convert a ProcessInfo struct into a single hash and returns it as a string
	processString := fmt.Sprintf("%s%s", sourceProcess.name, sourceProcess.path) //string concatenation
	processHash := sha1.New()
	processHash.Write([]byte(processString))
	hashedString := string(processHash.Sum(nil)) //gives the hash in a (byte slice converted to) string format
	return hashedString
}

func main() {

	println("Agent started.")

	/*	testProcess := ProcessInfo{
			name: "Cool Process",
			path: "D/whatever/somefolder",
		}
	*/
	// Create a JSON payload
	/*	requestBody := HashRequest{
			Key: HashProcessInfo(testProcess),
		}
	*/
	jsonData := []byte("just a strinbg")
	// Convert the payload to JSON
	/*	jsonData, err := json.Marshal(requestBody)
		if err != nil {
			fmt.Println("Failed to marshal JSON:", err)
			return
		}
	*/

	// Send the POST request
	url := "http://localhost:1037/check_hash" // Replace with the actual URL
	resp, err := http.Post(url, "application/json", bytes.NewBuffer(jsonData))
	if err != nil {
		fmt.Println("Failed to send POST request:", err)
		return
	}
	defer resp.Body.Close()

	// Read the response body
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println("Failed to read response body:", err)
		return
	}

	// Parse the JSON response
	/*	var response HashResponse
		err = json.Unmarshal(body, &response)
		if err != nil {
			fmt.Println("Failed to unmarshal JSON response:", err)
			return
		}
	*/
	// Print the response
	fmt.Println("ret_msg:" /*response.RetMsg*/, body)
}
