.PHONEY: all
all: libhellofromgo.so libhellofromgo.a
libhellofromgo.so:hellofromgo.go
	go build -o hellofromgo.so -buildmode=c-shared .
	mv hellofromgo.so libhellofromgo.so
libhellofromgo.a:hellofromgo.go
	go build -o hellofromgo.a -buildmode=c-archive .
	mv hellofromgo.a libhellofromgo.a

clean:
	rm -fr *.a *.h *.so
