.PHONEY:all

all: main_static main_shared

INCDIR:= -I ./hellofromgo

main_static:main.cpp ./hellofromgo/libhellofromgo.so
	g++ -o $@ $< $(INCDIR) -L ./hellofromgo -lhellofromgo -Wl,--rpath=./hellofromgo
	
main_shared:main.cpp ./hellofromgo/libhellofromgo.a
	g++ -o $@ $< $(INCDIR) ./hellofromgo/libhellofromgo.a -pthread

./hellofromgo/libhellofromgo.so ./hellofromgo/libhellofromgo.a:
	make -C hellofromgo

clean:
	rm -fr main_static main_shared
	make -C hellofromgo clean
