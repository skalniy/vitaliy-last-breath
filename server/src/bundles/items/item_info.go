package items

import "time"

type ItemInfo struct {
	ID       uint64    `json:"id"`
	Shipment time.Time `json:"shipment"`
	Arrival  time.Time `json:"arrival"`
	Location string    `json:"location"`
}

type UpdateInfo struct {
	ID       uint64    `json:"id"`
	Time     time.Time `json:"time"`
	Location string    `json:"location"`
}
