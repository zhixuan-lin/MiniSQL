#include "BufferManager.h"

BufferBlock::BufferBlock() {
    // default empty block as placeholder
    // pass
}

BufferBlock::BufferBlock(string filename, int blockIdx) {
    this->dataField = new char[BLOCKSIZE];

    fstream infile;
    infile.open(filename, ios::in | ios::out | ios::binary);
    infile.seekg(ios::beg + blockIdx * BLOCKSIZE);
    infile.read(this->dataField, BLOCKSIZE);
    infile.close();

    readBlock(this->bufferInfo, sizeof(this->bufferInfo));

    this->bufferInfo.timestamp = 0;

    if(!this->bufferInfo.hasInfo){
        this->bufferInfo.hasInfo = true;
        this->bufferInfo.offset = sizeof(bufferInfo);
        this->bufferInfo.blockIdx = blockIdx;
        this->bufferInfo.filename = filename;
        writeBlock(this->bufferInfo, sizeof(this->bufferInfo));
    }
}

void BufferBlock::writeBlock(string data, int offset) {
    this->bufferInfo.modified = true;
    memcpy(this->dataField + offset, (char*)data.c_str(), data.size());
    memset(this->dataField + offset + data.size(), 0, BLOCKSIZE - (offset + data.size()));
    this->bufferInfo.offset = data.size();
}

void BufferBlock::readBlock(string &dest, size_t size) {
    auto temp = new char(size);
    memcpy(temp, this->dataField, size);
    dest.assign(temp, size);
}

void BufferBlock::saveBlock() {
    if(this && this->bufferInfo.modified){
        std::fstream outfile;
        outfile.open(this->bufferInfo.filename, ios::out | ios::app | ios::binary);
        outfile.seekp(ios::beg + this->bufferInfo.blockIdx * BLOCKSIZE);
        outfile.write(dataField, BLOCKSIZE);
        outfile.close();
    }
}

size_t BufferBlock::getOffset() {
    return this->bufferInfo.offset;
}

BufferBlock& BufferManager::getBlock(string filename, int blockIdx) {
    int minTimestamp = buffer[0]->bufferInfo.timestamp;
    int minIndex = 0;

    // FIXME: insufficient implementation O(N)
    for (int i = 0; i < BLOCKNUM; i++) {
        if (buffer[i]->bufferInfo.blockIdx == blockIdx && buffer[i]->bufferInfo.filename == filename) {
            buffer[i]->bufferInfo.timestamp++;
            return *buffer[i];
        }
        if (buffer[i]->bufferInfo.timestamp < minTimestamp) {
            minTimestamp = buffer[i]->bufferInfo.timestamp;
            minIndex = i;
        }
    }

    buffer[minIndex]->saveBlock();
    buffer[minIndex] = new BufferBlock(filename, blockIdx);
    buffer[minIndex]->bufferInfo.timestamp ++;
    return *buffer[minIndex];
}

void BufferManager::saveAllBlocks() {
    for (auto &i : buffer)
        i->saveBlock();
}

BufferManager::BufferManager() {
    for(auto &i : buffer)
        i = new BufferBlock();
}

BufferManager::~BufferManager() {
    for(auto &i : buffer){
        i->saveBlock();
        delete i;
    }
}

bool BufferManager::fileExists(string filename) {
    ifstream f(filename.c_str());
    return f.good();
}