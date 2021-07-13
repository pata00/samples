package main
import "C"
import "fmt"
//export SayHello
func SayHello() {
	fmt.Printf("Hello from go!\n")
}
func main() {

}
