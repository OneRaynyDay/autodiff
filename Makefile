FLAGS=-Wall -g -Wc++11-extensions -std=c++11

build: etree.o main.o
	g++ $(FLAGS) -o build/main build/main.o build/etree.o
test: etree.o tests.o
	g++ $(FLAGS) -o build/tests build/tests.o build/etree.o
run_main: build/main
	build/main
run_tests: build/tests
	build/tests
etree.o: src/etree.cpp
	g++ $(FLAGS) -c src/etree.cpp -o build/etree.o
tests.o: test/tests.cpp
	g++ $(FLAGS) -c test/tests.cpp -o build/tests.o
main.o: src/main.cpp
	g++ $(FLAGS) -c src/main.cpp -o build/main.o
clean:
	rm build/* 
