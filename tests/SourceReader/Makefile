a.exe: main.o
	g++ main.o SourceReader.o StringParser.o -o a.exe

main.o: tests\SourceReader\main.cpp SourceReader.o
	g++ -c tests\SourceReader\main.cpp

SourceReader.o: SourceReader.cpp SourceReader.h StringParser.o
	g++ -c SourceReader.cpp -o SourceReader.o

StringParser.o: StringParser.cpp StringParser.h
	g++ -c StringParser.cpp -o StringParser.o

assignment1:
	make -f deliverables\assignment1\Makefile

TestSourceReader:
	make -f tests\SourceReader\Makefile