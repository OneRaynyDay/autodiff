FLAGS=-Wall -g

build: etree.o main.o
	g++ $(FLAGS) -o etree main.o proto.o
run: etree
	./etree
etree.o: etree.cpp
	g++ -c etree.cpp 
main.o:	main.cpp
	g++ -c main.cpp
clean:
	rm etree.o main.o etree
