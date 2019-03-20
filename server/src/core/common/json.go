package common

import (
	"encoding/json"
	"net/http"
)

func WriteJSONBody(respWriter *http.ResponseWriter, status int, v interface{}) {
	(*respWriter).WriteHeader(status)
	if err := json.NewEncoder(*respWriter).Encode(v); err != nil {
		(*respWriter).WriteHeader(500)
	}
}
