huffman: huffman.cpp BitStream.o CodeTree.o
	g++ -std=gnu++11 -o huffman huffman.cpp BitStream.o CodeTree.o
BitStream.o : BitStream.cpp BitStream.h
	g++ -std=gnu++11 -c -o BitStream.o BitStream.cpp
CodeTree.o: BitStream.o CodeTree.h CodeTree.cpp
	g++ -std=gnu++11 -c -o CodeTree.o CodeTree.cpp