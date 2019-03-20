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
func NewItemsBundle() core.Bundle {

	routes := []core.Route{
		core.Route{
			Method:  http.MethodGet,
			Path:    "/update",
			Handler: update,
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
