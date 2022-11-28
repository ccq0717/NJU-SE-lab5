#ifndef NODE_H
#define NODE_H

class node {
private:
    //程序对的路径
    char* file1_path;
    char* file2_path;
    //等价判断工具的判断结果是否为等价
    bool isEqual;
    //用户判定结果
    int flag;//0，尚未确认；1，等价；2，不等价；3，存疑
    friend class ConfirmTool;
public:
    node(const char* file1_path, const char* file2_path, bool isEqual);
    ~node() { delete[] file1_path; delete[] file2_path; }
};

#endif // NODE_H
