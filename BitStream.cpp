#include "BitStream.h"

BitInputStream::BitInputStream(istream &in) : input(in),
                                              currentByte(0),
                                              numBitsRemaining(0) {}

void BitInputStream::seekg(int offset)
{
    input.seekg(offset, ios::beg);
}

int BitInputStream::read()
{
    if (currentByte == -1){
        return -1;
    }
        
    if (numBitsRemaining == 0)
    {
        currentByte = input.get();
        if (currentByte == EOF)
            return -1;
        numBitsRemaining = 8;
    }
    numBitsRemaining--;
    return (currentByte >> numBitsRemaining) & 1;
}

char BitInputStream::readChar()
{
    return input.get();
}

BitOutputStream::BitOutputStream(ostream &out) : output(out),
                                                 currentByte(0),
                                                 numBitsFilled(0) {}

void BitOutputStream::write(int b)
{
    if (b != 0 && b != 1)
        return;
    currentByte = (currentByte << 1) | b;
    numBitsFilled++;
    if (numBitsFilled == 8)
    {
        if (numeric_limits<char>::is_signed)
        {
            currentByte -= (currentByte >> 7) << 8;
            output.put(static_cast<char>(currentByte));
            currentByte = 0;
            numBitsFilled = 0;
        }
    }
}

void BitOutputStream::writeChar(char c)
{
    output.put(c);
}

void BitOutputStream::finish()
{
    while (numBitsFilled != 0)
    {
        write(0);
    }
}