a.exe: main.o SourceReader.o StringParser.o Scanner.o
	g++ main.o SourceReader.o StringParser.o Scanner.o -o a.exe

main.o: main.cpp Scanner.o
	g++ -c main.cpp

SourceReader.o: SourceReader.cpp SourceReader.h StringParser.o
	g++ -c SourceReader.cpp -o SourceReader.o

StringParser.o: StringParser.cpp StringParser.h
	g++ -c StringParser.cpp -o StringParser.o

Scanner.o: Scanner\Scanner.hpp Scanner\Scanner.cpp SourceReader.o
	g++ -c Scanner\Scanner.cpp -o Scanner.o

clean:
	rm -rf *.o

assignment1:
	make -f deliverables\assignment1\Makefile
	
TestSourceReader:
	make -f tests\SourceReader\Makefile