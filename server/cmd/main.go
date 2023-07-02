package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

var processHashMap map[string]int

func checkHashHandler(w http.ResponseWriter, r *http.Request) {
	var processHash string
	if r.Method == "POST" {
		// Parse JSON request body
		err := json.NewDecoder(r.Body).Decode(&processHash)
		if err != nil {
			http.Error(w, "Invalid request body", http.StatusBadRequest)
			return
		}

		// Store data in the map
		processHashMap[processHash] += 1 //add hash to the map and increase the count if it already exists

		// Send a success response
		w.WriteHeader(http.StatusOK)
		fmt.Fprint(w, "Your hash is super cool")
	}
}

func main() {
	// Create hash map
	processHashMap = make(map[string]int)

	// Define the server routes
	http.HandleFunc("/check_hash", checkHashHandler)

	// Start the server
	log.Println("Server listening on port 1037")
	log.Fatal(http.ListenAndServe(":1037", nil))
}
