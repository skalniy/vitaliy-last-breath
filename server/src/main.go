package main

import (
	"log"
	"net/http"

	"github.com/gorilla/mux"

	items "./bundles/items"

	"./core"
)

func initBundles(itemsMap map[uint64]items.ItemInfo, shipMap map[uint64]items.ItemInfo) []core.Bundle {
	return []core.Bundle{items.NewItemsBundle(itemsMap, shipMap)}
}

func main() {

	router := mux.NewRouter()
	apiV1 := router.PathPrefix("/items").Subrouter()

	itemMap := make(map[uint64]items.ItemInfo)
	shipMap := make(map[uint64]items.ItemInfo)

	// now := time.Now()
	// itemMap[24] = items.ItemInfo{
	// 	Name:     "",
	// 	ID:       24,
	// 	Shipment: now,
	// 	Arrival:  now,
	// 	Location: "fu3hfupf3",
	// }
	// itemMap[12] = items.ItemInfo{
	// 	Name:     "Obj 12",
	// 	ID:       12,
	// 	Shipment: now,
	// 	Arrival:  now,
	// 	Location: "cdfnvjk3qn4uit",
	// }
	// later := time.Now()
	// itemMap[6] = items.ItemInfo{
	// 	Name:     "Obj 6",
	// 	ID:       6,
	// 	Shipment: now,
	// 	Arrival:  later,
	// 	Location: "f34nufini23qwdn",
	// }
	// itemMap[3] = items.ItemInfo{
	// 	Name:     "Obj 3",
	// 	ID:       3,
	// 	Shipment: now,
	// 	Arrival:  later,
	// 	Location: "fnr3igfn39gnup952",
	// }
	// itemMap[0] = items.ItemInfo{
	// 	Name:     " ",
	// 	ID:       0,
	// 	Shipment: now,
	// 	Arrival:  later,
	// 	Location: "9ru84rh87thfpw3hfu",
	// }

	bundles := initBundles(itemMap, shipMap)

	for _, bundle := range bundles {
		for _, route := range bundle.GetRoutes() {
			apiV1.HandleFunc(route.Path, route.Handler).Methods(route.Method)
		}
	}

	log.Printf("%v\n", itemMap)
	log.Fatal(http.ListenAndServe(":5000", router))
}
