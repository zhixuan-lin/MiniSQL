#ifndef MINISQL_BUFFERMANAGER_H
#define MINISQL_BUFFERMANAGER_H

#include "MiniType.h"

#include <memory>
#include <cstring>

enum BlockType {
    IndexBlock,
    RecordBlock
};

class BufferBlock {
public:
    friend class BufferManager;

    BlockType blockType;

    BufferBlock();

    BufferBlock(std::string filename, int blockIdx);

    // Note: fixed length

    void WriteBlock(IndexNode data, int offset);

    void WriteBlock(Record data, int offset);

    void ReadBlock(IndexNode &dest, int offset);

    void ReadBlock(Record &dest, int offset);

    void SaveBlock();

protected:
    char *dataField;
    int timestamp;
};

class BufferManager {
public:
    BufferBlock &GetBlock(std::string filename, int blockIdx);

    void SaveAllBlocks();

    bool FileExists(std::string filename);

    void NewFile(std::string filename);

private:
    BufferBlock buffer[BLOCKNUM];
};


#endif //MINISQL_BUFFERMANAGER_H
