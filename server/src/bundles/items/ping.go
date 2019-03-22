package items

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"../../core/common"
)

type Message struct {
	Msg string `json:"message"`
}

func update(itemsMap map[uint64]ItemInfo) func(response http.ResponseWriter, request *http.Request) {
	return func(response http.ResponseWriter, request *http.Request) {
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

		id := updateInfo.ID
		if item, ok := itemsMap[id]; ok {
			defer delete(itemsMap, id)
			item.Shipment = updateInfo.Time
			log.Printf("Shipment: %v", item)
			common.WriteJSONBody(&response, http.StatusOK, item)
			return
		} else {
			var item ItemInfo
			item.ID = id
			item.Arrival = updateInfo.Time
			item.Location = updateInfo.Location
			itemsMap[id] = item
			log.Printf("Arrival: %v", itemsMap[id])
			common.WriteJSONBody(&response, http.StatusOK, Message{fmt.Sprintf("Registered: %d", id)})
			return
		}

		common.WriteJSONBody(&response, http.StatusOK, itemsMap)
		return
	}
}
