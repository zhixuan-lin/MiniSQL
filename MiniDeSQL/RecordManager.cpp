#include "RecordManager.hpp"
#include "MiniType.h"
#include "BufferManager.h"
#include "IndexManager.hpp"

RecordManager::RecordIterator::RecordIterator(const MINI_TYPE::TableInfo & table, int record_index, BufferManager * bm)
{
    this->table = table;
    this->bm = bm;
    this->record_index = record_index;
    record_length = table.record_length + 1;
    block_id = record_index * record_length / MINI_TYPE::BlockSize;
    in_block_record_index = record_index * record_length - MINI_TYPE::BlockSize * block_id;
    past_the_end_block_id = bm->PastTheEndBlockID(table.name);
    records_per_block = MINI_TYPE::BlockSize / record_length;
    block = bm->GetBlock(table.name, block_id);
}

RecordManager::RecordIterator::~RecordIterator()
{
    bm->FreeBlock(table.name, block_id);
}

bool RecordManager::RecordIterator::Read(MINI_TYPE::Record & record)
{
    if ((*block)[in_block_record_index * record_length] == MINI_TYPE::Empty)
        return false;
    else
    {
        int byte_offset = 1;
        for (auto & value : record.values)
        {
            value.ReadFromMemory(block->head_pointer(), in_block_record_index * record_length + byte_offset);
            byte_offset += value.type.TypeSize();
        }
        return true;
    }
}
void RecordManager::RecordIterator::Write(const MINI_TYPE::Record & record) const
{
    int byte_offset = 1;
    for (auto & value : record.values)
    {
        value.WriteToMemory(block->head_pointer(), in_block_record_index * record_length + byte_offset);
        (*block)[in_block_record_index * record_length] = MINI_TYPE::NonEmpty;
        byte_offset += value.type.TypeSize();
    }
}
void RecordManager::RecordIterator::Delete()
{
    (*block)[in_block_record_index * record_length] = MINI_TYPE::Empty;
}

bool RecordManager::RecordIterator::Next()
{
    record_index++;
    in_block_record_index++;
    if (in_block_record_index >= records_per_block)
    {
        block_id++;
        in_block_record_index = 0;
        block = bm->GetBlock(table.name, block_id);
    }
    if (block_id >= past_the_end_block_id)
        return false;
    else
        return true;
}

// class RecordManager
// {
// public:
//     RecordManager(BufferManager *bm, IndexManager *im) : bm(bm), im(im) {}
//     bool CreateTableFile(const MINI_TYPE::TableInfo & table);
//     bool DeleteTableFile(const MINI_TYPE::TableInfo & table);
//     bool BuildIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);
//     bool DropIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);
//     bool InsertRecord(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Record & record);
//     MINI_TYPE::Table SelectRecord(const MINI_TYPE::TableInfo & table, \
//             const std::vector<MINI_TYPE::Condition> & conditions);
//     MINI_TYPE::Table SelectRecord(const MINI_TYPE::TableInfo & table, \
//             const std::vector<MINI_TYPE::Condition> & conditions, const IndexInfo & index);
//     bool DeleteRecord(const MINI_TYPE::TableInfo & table, const vector<Condition> & conditions);
// private:
//     BufferManager * bm;
//     IndexManager * im;
// };
bool RecordManager::CreateTableFile(const MINI_TYPE::TableInfo & table)
{
   bm->CreateFile(MINI_TYPE::TableFileName(table.name));
   return true;
}

bool RecordManager::DeleteTableFile(const MINI_TYPE::TableInfo & table)
{
   bm->RemoveFile(table.name);
   return true;
}

bool RecordManager::BuildIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute)
{
    
}
bool RecordManager::DropIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);
bool RecordManager::InsertRecord(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Record & record);
MINI_TYPE::Table RecordManager::SelectRecord(const MINI_TYPE::TableInfo & table, \
       const std::vector<MINI_TYPE::Condition> & conditions);
MINI_TYPE::Table RecordManager::SelectRecord(const MINI_TYPE::TableInfo & table, \
       const std::vector<MINI_TYPE::Condition> & conditions, const IndexInfo & index);
bool RecordManager::DeleteRecord(const MINI_TYPE::TableInfo & table, const vector<Condition> & conditions);
