a.exe: main.o
	g++ main.o SourceReader.o StringParser.o -o a.exe

main.o: deliverables\assignment1\main.cpp SourceReader.o
	g++ -c deliverables\assignment1\main.cpp

SourceReader.o: SourceReader.cpp SourceReader.h StringParser.o
	g++ -c SourceReader.cpp -o SourceReader.o

StringParser.o: StringParser.cpp StringParser.h
	g++ -c StringParser.cpp -o StringParser.o