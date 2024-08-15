// package main

// /*
// static void invoke(void (*f)(void*),void* user_data) {
//     f(user_data);
// }

// void go_print_hello(void* user_data);  // https://golang.org/issue/19837

// typedef void (*closure)();  // https://golang.org/issue/19835
// */
// import "C"

// import (
// 	"fmt"
// 	"unsafe"
// )

// // var a = &A{"dsadadsad"}

// //export go_print_hello
// func go_print_hello(user_data unsafe.Pointer) {
// 	a := (*A)(user_data)
// 	fmt.Println("Hello", a.body)
// }

// type A struct {
// 	body string
// }

//	func main() {
//		a := &A{"dsadadsad"}
//		fmt.Println(a.body)
//		C.invoke(C.closure(C.go_print_hello), unsafe.Pointer(a))
//	}
package main

import (
	"fmt"
	"io"
	"strconv"
)

func test() (err error) {
	a := "sdasdad"

	r, err := strconv.ParseUint(a, 10, 64)
	if err != nil {
		return
	}
	_ = r
	return io.EOF
}

func main() {
	fmt.Println(test())
}
