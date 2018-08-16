#include <iostream>
#include <fstream>
#include <limits>

using namespace std;


class BitInputStream{
  private:
    istream &input;
    int currentByte;
    int numBitsRemaining;

  public:
    void seekg(int);
    BitInputStream(istream &in);
    int read();
    char readChar();
    int readNoEof();
};

class BitOutputStream
{
  private:
    ostream &output;
    int currentByte;
    int numBitsFilled;

  public:
    BitOutputStream(ostream &out);
    void write(int b);
    void writeChar(char c);
    void finish();
};