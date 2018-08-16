#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <map>
typedef unsigned char byte;

#include "BitStream.h"
using namespace std;

const int max_size = 256;
class Node
{
public:
  char symbol;
  int weight;
  int leftChild, rightChild, parent;
};

class CodeTree
{
private:
  int num;
  vector<int> count;
  Node huffTree[2 * max_size - 1];
  int indexOf(char sym);
  void findMin(int &i1, int &i2);
  void read_file(string inpath);
  void save_code(int code_cache[], int code_cache_size);
  void calWeight();

  BitOutputStream *ptr_bout;
  BitInputStream *ptr_bin;
  string inpath;
  string outpath;
  unordered_map<char, int> freqs;
  unordered_map<char, vector<int> *> codes;
  map<string, char> decodes;
  void intToByte(int i, byte *bytes, int size = 4);

  int bytesToInt(byte *bytes, int size = 4);
public:
  CodeTree(string inpath, string outpath);
  virtual ~CodeTree();
  void print();
  void condense();
  void outtofile();
  void decoding();
  int search(string keyword);
};