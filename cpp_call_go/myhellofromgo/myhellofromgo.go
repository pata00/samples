package main
import "C"
import "fmt"
//export my_hello_from_go
func my_hello_from_go() {
	fmt.Printf("my_hello_from_go!\n")
}

func main() {

}
