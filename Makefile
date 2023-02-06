poke327: interface.o map.o character.o world.o util.o heap.o minheap.o poke327.o
	g++ interface.o map.o character.o world.o util.o heap.o minheap.o poke327.o -o poke327 -lncurses

character.o: character.cpp character.hpp
	g++ -Wall -Werror -g character.cpp -c

util.o: util.cpp util.hpp
	g++ -Wall -Werror -g util.cpp -c

poke327.o: poke327.cpp
	g++ -Wall -Werror -g poke327.cpp -c

map.o: map.cpp map.hpp
	g++ -Wall -Werror -g map.cpp -c

world.o: world.cpp world.hpp
	g++ -Wall -Werror -g world.cpp -c

interface.o: interface.cpp interface.hpp
	g++ -Wall -Werror -g interface.cpp -c

heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -c

minheap.o: minheap.cpp minheap.hpp
	g++ -Wall -Werror -g minheap.cpp -c

clean:
	rm -f *.o poke327