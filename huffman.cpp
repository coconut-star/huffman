#include <iostream>
#include <string>
#include "CodeTree.h"
using namespace std;

int encode(string inpath, string outpath)
{
    CodeTree *tree = new CodeTree(inpath, outpath);
    tree->condense();
    return 0;
}

int decode(string inpath, string outpath)
{
    CodeTree *tree = new CodeTree(inpath, outpath);
    tree->decoding();
    return 0;
}

int search(string inpath, string keyword)
{
    CodeTree *tree = new CodeTree(inpath, "NULL");
    return tree->search(keyword);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "invalid options";
        return -1;
    }
    string inpath;
    string outpath;
    string keyword;
    switch (argv[1][1])
    {
    case 'e':
        inpath = string(argv[2]);
        outpath = string(argv[3]);
        encode(inpath, outpath);
        break;
    case 'd':
        inpath = string(argv[2]);
        outpath = string(argv[3]);
        decode(inpath, outpath);
        break;
    case 's':
        inpath = string(argv[3]);
        keyword = string(argv[2]);
        cout<<search(inpath, keyword)<<endl;
        break;
    default:
        break;
    }
    return 0;
}