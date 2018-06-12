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
    block = bm->GetBlock(MINI_TYPE::TableFileName(table.name), block_id);
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

bool RecordManager::RecordIterator::Next(bool expand)
{
    record_index++;
    in_block_record_index++;
    if (in_block_record_index >= records_per_block)
    {
        block_id++;
        in_block_record_index = 0;
        block = bm->GetBlock(table.name, block_id);
    }
    if (block_id >= past_the_end_block_id and not expand)
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
    std::string index_name = MINI_TYPE::IndexName(table.name, attribute.name);
    im->CreateIndex(index_name, attribute.type);
    if(table.record_count > 0) {
        RecordIterator iter(table, 0, bm);
        while (true) {
            MINI_TYPE::Record record;
            iter.Read(record);
            MINI_TYPE::SqlValue key;
            for (int i = 0; i < table.attributes.size(); i++) {
                if (table.attributes[i].name == attribute.name) {
                    key = record.values[i];
                    break;
                }
            }
            im->InsertKey(index_name, key, iter.CurrentIndex());
        }
    }
}
bool RecordManager::DropIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute)
{
    std::string index_name = MINI_TYPE::IndexName(table.name, attribute.name);
    im->DropIndex(index_name);
    return true;
}

bool RecordManager::DropIndex(const std::string indexName) {
    im->DropIndex(indexName);
    return true;
}

bool RecordManager::DropIndex(const std::vector<std::string> indexNames) {
    for (auto &indexName : indexNames)
        im->DropIndex(indexName);
    return true;
}
bool RecordManager::InsertRecord(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Record & record)
{
    int past_the_end_block_id = bm->PastTheEndBlockID(MINI_TYPE::TableFileName(table.name));
    int start_record_index;
    int record_length = table.record_length + 1;
    int records_per_block = MINI_TYPE::BlockSize / record_length;
    if (past_the_end_block_id == 0)
        start_record_index = 0;
    else
        start_record_index = records_per_block * (past_the_end_block_id - 1);
    RecordIterator iter(table, start_record_index, bm);
    while (true)
    {
        MINI_TYPE::Record temp;
        if (not iter.Read(temp))
        {
            iter.Write(record);
            break;
        }
        iter.Next(true);
    }
    return true;
}

MINI_TYPE::Table RecordManager::SelectRecord(const MINI_TYPE::TableInfo & table, \
       const std::vector<MINI_TYPE::Condition> & conditions)
{
    MINI_TYPE::Table result(table);
    RecordIterator iter(table, 0, bm);
    while (true)
    {
        MINI_TYPE::Record temp;
        if (iter.Read(temp))
        {
            if (MINI_TYPE::Test(conditions, table, temp))
                result.records.push_back(temp);   
        }
        if (not iter.Next())
                break;
    }
    return result;
}
MINI_TYPE::Table RecordManager::SelectRecord(const MINI_TYPE::TableInfo & table, \
       const std::vector<MINI_TYPE::Condition> & conditions, const MINI_TYPE::IndexInfo & index)
{
    MINI_TYPE::Table result(table);
    MINI_TYPE::Condition cond_using_index;
    using MINI_TYPE::Operator;
    for (auto & cond : conditions)
    {
        if (cond.attributeName == index.name)
        {
            cond_using_index = cond;
        }
    }
    IndexManager::iterator start;
    IndexManager::iterator finish;
    auto op = cond_using_index.op;
    if (op == Operator::LessThan or op == Operator::LessEqual)
        start = im->Begin(index.name);
    else
        start = im->Find(index.name, cond_using_index.value);
    if (op == Operator::GreaterThan or op == Operator::GreaterEqual)
        finish = im->End(index.name);
    else
        start = im->Find(index.name, cond_using_index.value);
    IndexManager::iterator iter;
    for (auto iter = start; iter != finish; iter++)
    {
        auto record_fetcher = RecordIterator(table, (*iter).second, bm);
        MINI_TYPE::Record temp;
        if (record_fetcher.Read(temp) and MINI_TYPE::Test(cond_using_index, table, temp))
            result.records.push_back(temp);
    }
    // fetch last and test
    auto record_fetcher = RecordIterator(table, (*iter).second, bm);
    MINI_TYPE::Record temp;
    if (record_fetcher.Read(temp) and MINI_TYPE::Test(cond_using_index, table, temp))
        result.records.push_back(temp);
    
}

bool RecordManager::DeleteRecord(const MINI_TYPE::TableInfo & table, const vector<MINI_TYPE::Condition> & conditions){}
