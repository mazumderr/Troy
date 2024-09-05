a.exe: main.o
	g++ -std=c++20 main.o SourceReader.o StringParser.o -o a.exe

main.o: tests\SourceReader\main.cpp SourceReader.o
	g++ -c -std=c++20 tests\SourceReader\main.cpp

SourceReader.o: SourceReader.cpp SourceReader.h StringParser.o
	g++ -c -std=c++20 SourceReader.cpp -o SourceReader.o

StringParser.o: StringParser.cpp StringParser.h
	g++ -c -std=c++20 StringParser.cpp -o StringParser.o

assignment1:
	make -f deliverables\assignment1\Makefile

TestSourceReader:
	make -f tests\SourceReader\Makefile