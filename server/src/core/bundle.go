package core

// Bundle describe a HTTP routes
type Bundle interface {
	GetRoutes() []Route
}
