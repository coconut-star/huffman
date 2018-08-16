#include "CodeTree.h"
#include <fstream>
#include <iostream>
#include <stack>
#include <iterator>
#include <bitset>
#include <deque>
#include <memory.h>

const int INF = 100000000;

CodeTree::CodeTree(string inpath, string outpath)
{
    this->inpath = inpath;
    this->outpath = outpath;
}

CodeTree::~CodeTree()
{
    for (unordered_map<char, int>::iterator iter = freqs.begin(); iter != freqs.end(); iter++)
    {
        char symbol = iter->first;
        vector<int> *bitarray = codes[symbol];
        delete bitarray;
    }
}

void CodeTree::condense()
{
    const int max_size = 256;
    num = 0;
    count = vector<int>(256, 0);

    read_file(inpath);
    calWeight();
    for (int i = 0; i < 2 * num - 1; i++)
    {
        huffTree[i].parent = -1;
        huffTree[i].leftChild = -1;
        huffTree[i].rightChild = -1;
    }

    //find two smallest weight nodes, combine them under one parent node
    for (int i = 0; i < num - 1; i++)
    {
        int m1 = INF;
        int m2 = INF;
        int x1 = 0;
        int x2 = 0;
        for (int j = 0; j < num + i; j++)
        {
            if (huffTree[j].weight < m1 && huffTree[j].parent == -1)
            {
                m2 = m1;
                x2 = x1;
                m1 = huffTree[j].weight;
                x1 = j;
            }
            else if (huffTree[j].weight < m2 && huffTree[j].parent == -1)
            {
                m2 = huffTree[j].weight;
                x2 = j;
            }
        }
        huffTree[x1].parent = num + i;
        huffTree[x2].parent = num + i;
        huffTree[num + i].weight = huffTree[x1].weight + huffTree[x2].weight;
        huffTree[num + i].leftChild = x1;
        huffTree[num + i].rightChild = x2;
    }


    for (int i = 0; i < num; i++)
    {
        char symbol = huffTree[i].symbol;
        int parent = huffTree[i].parent;
        int child = i;
        stack<int> s;

        while (parent != -1)
        {
            if (huffTree[parent].leftChild == child)
            {
                s.push(0);
            }
            else
            {
                s.push(1);
            }
            child = parent;
            parent = huffTree[child].parent;
        }
        vector<int> *bitarray = new vector<int>();
        while (!s.empty())
        {
            int bit_symbol = s.top();
            bitarray->push_back(bit_symbol);
            s.pop();
        }
        codes[symbol] = bitarray;
    }
    outtofile();
}

void CodeTree::intToByte(int i, byte *bytes, int size )

{
    //byte[] bytes = new byte[4];
    memset(bytes, 0, sizeof(byte) * size);
    bytes[0] = (byte)(0xff & i);
    bytes[1] = (byte)((0xff00 & i) >> 8);
    bytes[2] = (byte)((0xff0000 & i) >> 16);
    bytes[3] = (byte)((0xff000000 & i) >> 24);
    return;
}

int CodeTree::bytesToInt(byte *bytes, int size )
{
    int addr = bytes[0] & 0xFF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    addr |= ((bytes[2] << 16) & 0xFF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

void CodeTree::outtofile()
{
    const size_t WR_SIZE = 1024 * 1024 * 10;
    const int header_size = 1024;
    int header_index = 0;
    char header[header_size];
    char *buf = new char[WR_SIZE];
    ifstream in(inpath, ios::binary);
    ofstream out(outpath, ios::binary);
    BitOutputStream bout(out);
    ptr_bout = &bout;
    int weight_sum = 0;
    for(int i = 0; i < num ; i++){
        weight_sum += huffTree[i].weight;
    }
    byte sumbyte[4];
    this->intToByte(weight_sum, sumbyte);
    ptr_bout->writeChar((char)sumbyte[0]);
    ptr_bout->writeChar((char)sumbyte[1]);
    ptr_bout->writeChar((char)sumbyte[2]);
    ptr_bout->writeChar((char)sumbyte[3]);
    header_index += 4;
    for (unordered_map<char, int>::iterator iter = freqs.begin(); iter != freqs.end(); iter++)
    {
        char sym = iter->first;
        int index = indexOf(sym);
        vector<int> *bitarray = codes[sym];
        ptr_bout->writeChar(bitarray->size());
        ptr_bout->writeChar(sym);

        for (int j = 0; j < bitarray->size(); j++)
        {
            ptr_bout->write(bitarray->at(j));
        }
        ptr_bout->finish();
        header_index += 2;

        if (bitarray->size() % 8 == 0)
        {
            header_index += (bitarray->size() / 8);
        }
        else
        {
            header_index += (bitarray->size() / 8);
            header_index += 1;
        }
    }
    while (header_index < 1024)
    {
        ptr_bout->writeChar(0);
        header_index++;
    }
    while (!in.eof())
    {
        streamsize num_read = in.read(buf, WR_SIZE).gcount();
        if (0 == num_read)
            break;
        for (int i = 0; i < num_read; i++)
        {
            char symbol = buf[i];
            vector<int> *bitarray = this->codes[symbol];

            for (int j = 0; j < bitarray->size(); j++)
            {
                ptr_bout->write(bitarray->at(j));
            }
        }
        in.seekg(num_read, ios::cur);
    }

    in.close();

    ptr_bout->finish();
}

void CodeTree::decoding()
{
    const size_t WR_SIZE = 1024;
    size_t header_size = 1024;
    char *buf = new char[header_size];
    ifstream in(inpath, ios::binary);
    BitInputStream bin(in);
    ptr_bin = &bin;
    ofstream out(outpath, ios::binary);
    BitOutputStream bout(out);
    ptr_bout = &bout;
    num = 0;
    int header_index = 0;
    byte sumbyte[4];
    sumbyte[0] = ptr_bin->readChar();
    sumbyte[1] = ptr_bin->readChar();
    sumbyte[2] = ptr_bin->readChar();
    sumbyte[3] = ptr_bin->readChar();
    header_size -= 4;
    int sum = this->bytesToInt(sumbyte);
    int count = 0;
    for (int i = 0; i < header_size; i++)
    {
        int bitarraysize = ptr_bin->readChar();
        if (bitarraysize == 0)
            break;
        char sym = ptr_bin->readChar();

        vector<int> *bitarray = new vector<int>();
        string bitstr = "";
        int charsize = 0;

        if (bitarraysize % 8 == 0)
        {
            charsize = bitarraysize / 8;
        }
        else
        {
            charsize = bitarraysize / 8 + 1;
        }

        for (int j = 0; j < charsize; j++)
        {
            char b = ptr_bin->readChar();
            int bit = 0;
            bitset<8> bits(b);
            for (int i = 7; i >= 0; i--)
            {

                bitstr += (bits[i] + '0');
            }
        }
        bitstr = bitstr.substr(0, bitarraysize);

        huffTree[num].symbol = sym;
        decodes[bitstr] = sym;
        codes[sym] = bitarray;
        num++;
    }

    int root = 2 * num - 2;
    ptr_bin->seekg(1024);
    int bit = ptr_bin->read();

    string bitstr = "";
    char symbol;
    while (bit != -1)
    {
        bitstr += (char)(bit + '0');
        map<string, char>::iterator it = decodes.find(bitstr);
        if (it != decodes.end())
        {
            symbol = it->second;
            ptr_bout->writeChar(symbol);
            count++;
            if(count == sum){
                in.close();
                ptr_bout->write(EOF);
                ptr_bout->finish();
                return;
            }
            bitstr = "";
        }

        bit = ptr_bin->read();
    }

    in.close();
    ptr_bout->write(EOF);
    ptr_bout->finish();
}


void CodeTree::calWeight()
{
    num = 0;
    for (unordered_map<char, int>::iterator iter = freqs.begin(); iter != freqs.end(); iter++)
    {
        char sym = iter->first;
        huffTree[num].symbol = sym;
        huffTree[num].weight = iter->second;
        num++;
    }
}

void CodeTree::read_file(string inpath)
{
    const size_t WR_SIZE = 1024 * 1024 * 10;
    char *buf = new char[WR_SIZE];
    ifstream in(inpath, ios::binary);
    while (!in.eof())
    {
        streamsize num_read = in.read(buf, WR_SIZE).gcount();
        if (0 == num_read)
            break;
        for (int i = 0; i < num_read; i++)
        {
            char symbol = buf[i];
            if (freqs.find(symbol) == freqs.end())
            {
                freqs[symbol] = 1;
            }
            else
            {
                freqs[symbol]++;
            }
        }
        //cout<<freqs['d']<<"|";
        in.seekg(num_read, ios::cur);
    }

    in.close();
}

int CodeTree::indexOf(char sym)
{
    for (int i = 0; i < num; i++)
    {
        if (huffTree[i].symbol == sym)
        {
            return i;
        }
    }
    return -1;
}

int CodeTree::search(string keyword)
{
    const size_t WR_SIZE = 1024;
    size_t header_size = 1024;
    char *buf = new char[header_size];
    ifstream in(inpath, ios::binary);
    BitInputStream bin(in);
    ptr_bin = &bin;

    num = 0;
    int header_index = 0;
    byte sumbyte[4];
    sumbyte[0] = ptr_bin->readChar();
    sumbyte[1] = ptr_bin->readChar();
    sumbyte[2] = ptr_bin->readChar();
    sumbyte[3] = ptr_bin->readChar();
    header_size -= 4;
    int sum = this->bytesToInt(sumbyte);
    for (int i = 0; i < header_size; i++)
    {
        int bitarraysize = ptr_bin->readChar();
        if (bitarraysize == 0)
            break;
        char sym = ptr_bin->readChar();

        vector<int> *bitarray = new vector<int>();
        string bitstr = "";
        int charsize = 0;

        if (bitarraysize % 8 == 0)
        {
            charsize = bitarraysize / 8;
        }
        else
        {
            charsize = bitarraysize / 8 + 1;
        }

        for (int j = 0; j < charsize; j++)
        {
            char b = ptr_bin->readChar();
            int bit = 0;
            bitset<8> bits(b);
            for (int i = 7; i >= 0; i--)
            {
                bitstr += (bits[i] + '0');
            }
            for (int i = 7; i > (7 - bitarraysize); i--)
            {
                bitarray->push_back(bits[i]);
            }
        }
        bitstr = bitstr.substr(0, bitarraysize);
        huffTree[num].symbol = sym;
        decodes[bitstr] = sym;
        codes[sym] = bitarray;
        num++;
    }


    ptr_bin->seekg(1024);
    int bit = ptr_bin->read();

    string bitstr = "";
    char symbol;
    int times = 0;
    int wr_index = 0;
    int match_char_num = 0;
    deque<char> dq;
    int keyword_size = keyword.length();
    int count = 0;
    while (bit != -1)
    {
        bitstr += (char)(bit + '0');
        map<string, char>::iterator it = decodes.find(bitstr);
        if (it != decodes.end())
        {
            bitstr = "";
            symbol = it->second;
            count ++;
            dq.push_back(symbol);
            if (dq.size() == keyword_size)
            {
                bool isSame = true;
                for (int j = 0; j < keyword_size; j++)
                {
                    if (dq.at(j) != keyword.at(j)){
                        isSame = false;
                        break;
                    }
                }
                if(isSame) {
                    //cout<<keyword;
                    times++;
                    dq.pop_front();
                }else{
                    dq.pop_front();
                }
            }
            if(count == sum) {
                in.close();
                return times;
            }
        }
        bit = ptr_bin->read();
    }

    return times;
}