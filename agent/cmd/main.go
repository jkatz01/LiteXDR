package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

type HashRequest struct {
	Key   string `json:"key"`
	Value string `json:"value"`
}

type HashResponse struct {
	RetMsg bool `json:"ret_msg"`
}

func main() {

	println("Agent started.")

	// Create a JSON payload
	requestBody := HashRequest{
		Key:   "Key1",
		Value: "Val1 (Blablong1)",
	}

	// Convert the payload to JSON
	jsonData, err := json.Marshal(requestBody)
	if err != nil {
		fmt.Println("Failed to marshal JSON:", err)
		return
	}

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
	var response HashResponse
	err = json.Unmarshal(body, &response)
	if err != nil {
		fmt.Println("Failed to unmarshal JSON response:", err)
		return
	}

	// Print the response
	fmt.Println("ret_msg:", response.RetMsg)
}
