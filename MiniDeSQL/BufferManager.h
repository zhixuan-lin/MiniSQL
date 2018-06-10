#include <iostream>
#include <map>
#include <utility>
#include "MiniType.h"
#include <array>

class BufferManager;

class Block
{
public:
	Block();
    
	Block & Write(void * source, int offset, std::size_t size);
	Block & Read(void * dest, int offset, std::size_t size);
	Block & Flush();
    char & operator[](int i) { return content[i]; }
    
private:
    Block & SetMRUtime(int t);
    Block & Reset();
    Block & Connect(const std::string & filename, int block_id, bool get_content=false);
    Block & SetPinned(bool pinned);
	bool dirty;
    char content[MINI_TYPE::BlockSize];
    std::string filename;
    int block_id;
    int MRUtime;
    bool pinned;
	
    friend BufferManager;
};

class BufferManager
{
public:
    BufferManager() {};
    ~BufferManager() {FlushAllBlocks();}
	Block & GetBlock(const std::string & filename, int block_id);
	Block & FreeBlock(const std::string & filename, int block_id);
	void CreateFile(const std::string & filename);
	void RemoveFile(const std::string & filename);
    int PastTheEndBlockID(const std::string & filename);
    void FlushAllBlocks();
private:
    
    using MapType = std::map<std::pair<std::string, int>, Block &>;
    MapType block_map;
	std::array<Block, MINI_TYPE::MaxBlocks> blocks;
	Block & GetLRU();
	int access_counter = 1;
};
