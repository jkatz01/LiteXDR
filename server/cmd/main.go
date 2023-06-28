package main

import (
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

func checkHashHandler(w http.ResponseWriter, r *http.Request) {
	if r.Method == "POST" {
		// Parse JSON request body
		var hashData map[string]string
		err := json.NewDecoder(r.Body).Decode(&hashData)
		if err != nil {
			http.Error(w, "Invalid request body", http.StatusBadRequest)
			return
		}

		// Store data in the map
		for key, value := range hashData {
			log.Println(key, value)
		}

		// Send a success response
		w.WriteHeader(http.StatusOK)
		fmt.Fprint(w, "Your hash is super cool")
	}
}

func main() {

	// Define the server routes
	http.HandleFunc("/check_hash", checkHashHandler)

	// Start the server
	log.Println("Server listening on port 1037")
	log.Fatal(http.ListenAndServe(":1037", nil))
}
