#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include "DataStructure.h"
#include "BufferManager.h"


Block::Block() { Reset(); }



Block & Block::Connect(const std::string & filename, int block_id, bool get_content)
{
    Reset();
    dirty = true;
	this->filename = filename;
	this->block_id = block_id;
    if (get_content)
    {
        std::ifstream fin(filename.c_str(), std::ios_base::binary);
        if (not fin.is_open())
        {
            std::cerr << "Cannot open file " + filename + ".\n";
            std::exit(0);
        }
        fin.seekg(block_id * MINISQL_BASE::BlockSize);
        char temp[MINISQL_BASE::BlockSize];
        fin.read(temp, MINISQL_BASE::BlockSize);
        fin.close();
        Write(temp, 0, MINISQL_BASE::BlockSize);
    }
	return *this;
}

Block & Block::Reset()
{
	dirty = pinned = false;
	memset(content, 0, MINISQL_BASE::BlockSize);
	MRUtime = 0;
    return *this;
}
Block & Block::SetPinned(bool pinned)  { this->pinned =  pinned; return *this;}

Block & Block::Write(void * source, int offset, std::size_t size)
{
	dirty = true;
	std::memcpy(content + offset, source, size);
	return *this;
}

Block & Block::Read(void * dest, int offset, std::size_t size)
{
	std::memcpy(dest, content + offset, size);
	return *this;
}

Block & Block::Flush()
{
	if (dirty)
	{
		dirty = false;
        std::ofstream fout(filename, std::ios_base::binary | std::ios_base::app);
		fout.seekp(block_id * MINISQL_BASE::BlockSize);
		fout.write(content, MINISQL_BASE::BlockSize);
		fout.close();
	}
	
	return *this;
}

Block & Block::SetMRUtime(int t) 
{
	MRUtime = t;
    return *this;
}
int BufferManager::PastTheEndBlockID(const std::string & filename)
{
    struct stat st;
    if (stat(filename.c_str(), &st) == 0) {
        return int(st.st_size / MINISQL_BASE::BlockSize);
    }
    else
    {
        std::cerr << "Failed to get file past-the-end.\n";
        exit(0);
    }
}

Block & BufferManager::GetBlock(const std::string & filename, int block_id)
{
    if (block_id > PastTheEndBlockID(filename))
    {
        std::cerr << "Block id out of bound.\n";
        exit(0);
    }
    auto block_iter = block_map.find(std::make_pair(filename, block_id));
	if (block_iter != block_map.end())
	{
		block_iter->second.SetPinned(true);
		block_iter->second.SetMRUtime(access_counter++);
		return block_iter->second;
	}
	else
	{
		Block & block = GetLRU();
        if (PastTheEndBlockID(filename) == block_id)
            block.Reset().Flush().SetPinned(true).SetMRUtime(access_counter++).Connect(filename, block_id, false);
        else
            block.Reset().Flush().SetPinned(true).SetMRUtime(access_counter++).Connect(filename, block_id, true);
        block_map.erase(std::make_pair(block.filename, block.block_id));
        block_map.insert(MapType::value_type(std::make_pair(filename, block_id), block));
		return block;
	}
}
Block & BufferManager::FreeBlock(const std::string & filename, int block_id)
{
	auto block_iter = block_map.find(std::make_pair(filename, block_id));
	if (block_iter == block_map.end())
	{
		std::cerr << "Block not allocated!\n";
		std::exit(0);
	}
	else
		block_iter->second.SetPinned(false);
	return block_iter->second;
}
void BufferManager::CreateFile(const std::string & filename)
{
	std::ofstream(filename.c_str());
}

void BufferManager::RemoveFile(const std::string & filename)
{
	for (auto iter = block_map.begin(); iter != block_map.end(); iter++)
	{
        if (iter->first.first == filename)
        	block_map.erase(iter);
    }
	remove(filename.c_str());
}

void BufferManager::FlushAllBlocks()
{
    for (auto & block : block_map)
        block.second.Flush();
}
Block & BufferManager::GetLRU()
{
	int min = access_counter;
	Block * pb = nullptr;
	for (auto & block : blocks)
	{
		if (not block.pinned and block.MRUtime < min)
		{
			min = block.MRUtime;
			pb = &block;
		}
	}
	if (not pb)
	{
		std::cerr << "No LRU block found!\n";
		exit(0);
	}
	else
		return *pb;
}
