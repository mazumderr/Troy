a.exe: main.o
	g++ main.o -o a.exe

main.o: main.cpp
	g++ -c main.cpp

#would be cool if this was magically `rm` on linux/mac
clean:
	del main.o