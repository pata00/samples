.PHONEY:all lib

all:lib main_with_static_lib main_with_shared_lib

lib:
	make -C ./myhellofromgo

main_with_static_lib:main.cpp
	g++ $< -o $@ -I ./myhellofromgo -L ./myhellofromgo -l:libmyhellofromgo.a -pthread

main_with_shared_lib:main.cpp
	g++ $< -o $@ -I ./myhellofromgo -L ./myhellofromgo -l:libmyhellofromgo.so -Wl,--rpath=./myhellofromgo

clean:
	rm -fr main_with*
	make -C ./myhellofromgo clean
	
