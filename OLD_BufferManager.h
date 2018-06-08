#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

/*
    我现在想了想，好像不能用等长 sizeof 什么的方式来读块。因为 record 是不等长的。

    因此，ReadBlock 和 WriteBlock 也许要定义三个参数？我想，对存索引的块而言都是一样的长度（sizeof(IndexInfo)，参考新的 BufferManager.h）。但是对存 record 的块而言应该要人为设置一个长度去读，可以预先存在表信息里（或者干脆都用 0 对齐强行等长）。对不起，这里我没有想得很清楚…

    然后这个代码当时为了能尽快跑起来所以很多地方效率很低。比如一开始把整个 buffer 区都构造了= =然后 LRU 用的是 O（N） 算法，用链表的话会快一些。然后标注某个块在被使用的标记也没有。

    当时写的时候脑子并不清楚（对这个没有什么概念，纯属因为看网上代码看不懂打算自己开始瞎写，然后其实不能用还写得很难看= =）建议你还是参考网上代码。
      
*/

#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>
#include <utility>
#include <iostream>

#include "./config.h"

using namespace std;

class BufferBlock {

public:
    friend class BufferManager;

    BufferBlock();
    BufferBlock(string filename, int blockIdx);

    template<typename T>
    void writeBlock(T data, int offset){
        this->bufferInfo.modified = true;
        memcpy(this->dataField + offset, (char*)&data, sizeof(data));
        this->bufferInfo.offset = sizeof(data);
    }

    template<typename T>
    void readBlock(T& dest, size_t size){
        memcpy((char *)&dest, this->dataField, size);
    }

    void writeBlock(string data, int offset);
    void readBlock(string& dest, size_t size);
    void saveBlock();
    size_t getOffset();

protected:
    char* dataField;

    class BufferInfo {
    public:

        BufferInfo():
                hasInfo(true),
                offset(0),
                modified(false),
                blockIdx(0),
                filename(""),
                timestamp(0)
        {};

        bool hasInfo;
        size_t offset;
        bool modified;
        int blockIdx;
        string filename;
        int timestamp;

    } bufferInfo;
};

class BufferManager {

public:
    BufferManager();
    ~BufferManager();
    BufferBlock& getBlock(string filename, int blockIdx);
    void saveAllBlocks();

    bool fileExists(string filename);
    void createFile(string filename);

private:
    BufferBlock* buffer[BLOCKNUM];
};


#endif //MINISQL_BUFFERMANAGER_H
