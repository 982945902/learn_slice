package main

import (
	"context"

	"github.com/cloudwego/hertz/pkg/app"
	"github.com/cloudwego/hertz/pkg/app/server"
	"github.com/cloudwego/hertz/pkg/protocol/consts"
)

func main() {
	h := server.Default()

	h.GET("/test", func(c context.Context, ctx *app.RequestContext) {
		// time.Sleep(time.Millisecond * 10)

		ctx.String(consts.StatusOK, "test")
	})

	h.Spin()
}
