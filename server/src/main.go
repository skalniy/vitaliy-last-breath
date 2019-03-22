package main

import (
	"log"
	"net/http"

	"github.com/gorilla/mux"

	items "./bundles/items"

	"./core"
)

func initBundles(itemsMap map[uint64]items.ItemInfo) []core.Bundle {
	return []core.Bundle{items.NewItemsBundle(itemsMap)}
}

func main() {

	router := mux.NewRouter()
	apiV1 := router.PathPrefix("/items").Subrouter()

	itemMap := make(map[uint64]items.ItemInfo)
	bundles := initBundles(itemMap)

	for _, bundle := range bundles {
		for _, route := range bundle.GetRoutes() {
			apiV1.HandleFunc(route.Path, route.Handler).Methods(route.Method)
		}
	}

	log.Fatal(http.ListenAndServe(":5000", router))
}
