Huffman

A C++ program based on huffman algorithm. It can compress file by encoding file with shorter code.

Options:
    -e 
    huffman -e test1 test.enc
    -d
    huffman -d test.enc test.dec
    -s
    huffman -s "a" test.enc

Codefiles:
    BitStream.cpp BitStream.h:
    BitStream is used to input and output bit directly into file. Since we can only store byte into file, it is necessary to set extra 0 to confirm amount of bits mod 8 equals 0.

    CodeTree.cpp CodeTree.h
    Data struture for huffman tree. To avoid high memeory cost, I used an array of node to construct the data struture. A node contains index of parent, leftChild and rightChild. When a node is needed to be append as child, just set its index to its parent's child.

    huffman.cpp
    The entry of the program, to handle the 3 kinds of arguments.


Header design:
    1. Source file size: I used 4 bytes(one int) to store the size of the source file. Since the maximum size would be 100MB, one int value would be big enough to handle it.
    2. The encoding pairs: This part contains encoding pairs for all char occured in the source file. It contains 3 parts: 
        2.1 One byte to store the encoded code length
        2.2 One byte to store the original char
        2.3 Encoded code
    For example, if 'a' would be encoded into '1100', the 2.1 would be '00000100' which represents 4, 2.2 would be binary code for char 'a', 2.3 would be '11000000'(the extra 0000 is for 8 bits need).

How search works:
    To make the search fast, I construct a deque whose length equals the length of search keyword. Then try to read the file into this deque, when not matching, it will popfront and pushback one more char. Every time the deque "moves", do comparation between string in deque and keyword.


