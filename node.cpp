#include "node.h"
#include <cstring>
#include <string>
using namespace std;

node::node(const char* file1_path, const char* file2_path, bool isEqual)
{
    this->file1_path = new char[strlen(file1_path) + 1];
    strcpy(this->file1_path, file1_path);
    this->file2_path = new char[strlen(file2_path) + 1];
    strcpy(this->file2_path, file2_path);
    this->isEqual = isEqual;
    flag = 0;//尚未查看
}


