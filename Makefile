FLAGS=-Wall -g -Wc++11-extensions -std=c++11

build: build/etree.o build/main.o
	g++ $(FLAGS) -o build/etree build/main.o build/etree.o
run: build/etree
	build/etree
etree.o: src/etree.cpp
	g++ $(FLAGS) -c src/etree.cpp build/etree.o
main.o:	main.cpp
	g++ $(FLAGS) -c src/main.cpp build/main.o
clean:
	rm build/* 
