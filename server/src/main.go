package main

import (
	"log"
	"net/http"

	"github.com/gorilla/mux"

	items "./bundles/items"
	"./core"
)

func initBundles() []core.Bundle {
	return []core.Bundle{items.NewItemsBundle()}
}

func main() {

	router := mux.NewRouter()
	apiV1 := router.PathPrefix("/items").Subrouter()

	bundles := initBundles()

	for _, bundle := range bundles {
		for _, route := range bundle.GetRoutes() {
			apiV1.HandleFunc(route.Path, route.Handler).Methods(route.Method)
		}
	}

	log.Fatal(http.ListenAndServe(":5000", router))
}
