
boost_lib = E:/libs/boost/include/boost-1_70
cc = g++
jtt808.o : jtt808.c jtt808.h
	$(cc) -c jtt808.c
test.exe : jtt808.o jtt808.h ./tests/test_jtt808.cpp
	$(cc) ./tests/test_jtt808.cpp jtt808.o -o test -I $(boost_lib) -I .

test : test.exe
	test.exe -i -p
	
clean : 
	-del *.o *.exe