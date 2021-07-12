.PHONEY: all
all: libmyhellofromgo.so libmyhellofromgo.a
libmyhellofromgo.so:myhellofromgo.go
	go build -o myhellofromgo.so -buildmode=c-shared .
	mv myhellofromgo.so libmyhellofromgo.so
libmyhellofromgo.a:myhellofromgo.go
	go build -o myhellofromgo.a -buildmode=c-archive .
	mv myhellofromgo.a libmyhellofromgo.a
clean:
	rm -fr *.a *.so *.h
