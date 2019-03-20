package items

import (
	"encoding/json"
	"io/ioutil"
	"log"
	"net/http"
	"time"

	"../../core/common"
)

func update(response http.ResponseWriter, request *http.Request) {
	response.Header().Set("Content-Type", "application/json; charset=utf-8")

	body, err := ioutil.ReadAll(request.Body)
	if err != nil {
		panic(err)
	}
	defer request.Body.Close()

	var updateInfo UpdateInfo
	err = json.Unmarshal(body, &updateInfo)
	if err != nil {
		panic(err)
	}

	log.Printf("LOG: %v", updateInfo)

	common.WriteJSONBody(&response, http.StatusOK, ItemInfo{
		ID:       0x12f03422,
		Arrival:  time.Now().Truncate(time.Millisecond).UTC(),
		Shipment: time.Now().Truncate(time.Millisecond).UTC(),
		Location: "783479483254025.34235475812428",
	})
}
