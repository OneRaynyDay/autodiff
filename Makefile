FLAGS=-Wall -g -Wc++11-extensions -std=c++11

# RUN
build: etree.o main.o var.o
	g++ $(FLAGS) -o build/main build/main.o build/var.o build/etree.o
	build/main
test: var-test etree-test
	build/var-test
	build/etree-test

# SRC BUILD
var.o: src/var.cpp
	g++ $(FLAGS) -c src/var.cpp -o build/var.o
etree.o: src/etree.cpp
	g++ $(FLAGS) -c src/etree.cpp -o build/etree.o

# TEST BUILD
main-test.o: test/main-test.cpp
	g++ $(FLAGS) -c test/main-test.cpp -o build/main-test.o
var-test: test/var-test.cpp main-test.o
	g++ $(FLAGS) build/main-test.o \
		test/var-test.cpp \
		src/var.cpp \
		-o build/var-test
etree-test: test/etree-test.cpp main-test.o
	g++ $(FLAGS) build/main-test.o \
		test/etree-test.cpp \
		src/etree.cpp \
		src/var.cpp \
		-o build/etree-test

# MAIN BUILD
main.o: src/main.cpp
	g++ $(FLAGS) -c src/main.cpp -o build/main.o

# CLEAN/DIST
clean:
	rm build/* 
