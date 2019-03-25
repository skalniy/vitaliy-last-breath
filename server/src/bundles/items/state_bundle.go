package items

import (
	"net/http"

	"../../core"
)

// ItemsBundle handle state of service
type ItemsBundle struct {
	routes []core.Route
}

// NewItemsBundle instance
func NewItemsBundle(itemsMap map[uint64]ItemInfo, shipMap map[uint64]ItemInfo) core.Bundle {

	routes := []core.Route{
		core.Route{
			Method:  http.MethodPost,
			Path:    "/update",
			Handler: update(itemsMap, shipMap),
		},
		core.Route{
			Method:  http.MethodGet,
			Path:    "",
			Handler: get_data(itemsMap, shipMap),
		},
	}

	return &ItemsBundle{
		routes: routes,
	}
}

// GetRoutes implement interface core.Bundle
func (b *ItemsBundle) GetRoutes() []core.Route {
	return b.routes
}
