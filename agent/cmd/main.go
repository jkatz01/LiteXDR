package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
)

func main() {

	values := map[string]string{"key": "my hash"}
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
