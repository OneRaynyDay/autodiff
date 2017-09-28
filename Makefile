CC=clang++
FLAGS=-Wall -g -Wc++11-extensions -std=c++11

# RUN
build: expression.o main.o var.o
	$(CC) $(FLAGS) -o build/main build/main.o build/var.o build/expression.o
	build/main
test: var-test expression-test # utils-test
	build/var-test
	build/expression-test
	# build/utils-test

# SRC BUILD
var.o: src/var.cpp
	$(CC) $(FLAGS) -c src/var.cpp -o build/var.o
expression.o: src/expression.cpp src/visitors.cpp
	$(CC) $(FLAGS) -c src/expression.cpp src/visitors.cpp -o build/expression.o

# TEST BUILD
main-test.o: test/main-test.cpp
	$(CC) $(FLAGS) -c test/main-test.cpp -o build/main-test.o
var-test: test/var-test.cpp main-test.o
	$(CC) $(FLAGS) build/main-test.o \
		test/var-test.cpp \
		src/var.cpp \
		-o build/var-test
expression-test: test/expression-test.cpp src/expression.cpp src/var.cpp main-test.o
	$(CC) $(FLAGS) build/main-test.o \
		test/expression-test.cpp \
		src/expression.cpp \
		src/var.cpp \
		-o build/expression-test
utils-test: test/utils-test.cpp test/expression-test.cpp src/expression.cpp src/var.cpp src/utils.cpp main-test.o
	$(CC) $(FLAGS) build/main-test.o \
		test/utils-test.cpp \
		src/utils.cpp \
		src/expression.cpp \
		src/var.cpp \
		-o build/utils-test

# MAIN BUILD
main.o: src/main.cpp
	$(CC) $(FLAGS) -c src/main.cpp -o build/main.o

# CLEAN/DIST
clean:
	rm -rf build/* 
