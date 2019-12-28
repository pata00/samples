a.out:main.o neg_use_asm.o
	g++ $^ 

neg_use_asm.o:neg_use_asm.s
	as $< -o $@

main.o:main.cpp
	g++ $< -c -o $@

clean:
	rm -fr *.o a.out
