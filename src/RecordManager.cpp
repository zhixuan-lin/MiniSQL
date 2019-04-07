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
    records_per_block = MINI_TYPE::BlockSize / record_length;
    block_id = record_index / records_per_block;
    in_block_record_index = record_index - block_id * records_per_block;
    past_the_end_block_id = bm->PastTheEndBlockID(table.name);
    
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
        record.Conform(table);
        int byte_offset = 1;
        for (auto & value : record.values)
        {
            value.ReadFromMemory(block->head_pointer(false), in_block_record_index * record_length + byte_offset);
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
        value.WriteToMemory(block->head_pointer(true), in_block_record_index * record_length + byte_offset);
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
    past_the_end_block_id = bm->PastTheEndBlockID(table.name);
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

bool RecordManager::BuildIndex(MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute)
{
    std::string index_name = MINI_TYPE::IndexName(table.name, attribute.name);
    table.indices[attribute.name] = index_name;
    im->CreateIndex(index_name, attribute.type);
    RecordIterator iter(table, 0, bm);
    while (true)
    {
        MINI_TYPE::Record record;
        if (iter.Read(record))
        {
            MINI_TYPE::SqlValue key;
            for (int i = 0; i < table.attributes.size(); i++)
            {
                if (table.attributes[i].name == attribute.name)
                {
                    key = record.values[i];
                    break;
                }
            }
            im->InsertKey(index_name, key, iter.CurrentIndex());
            
        }
        if (not iter.Next())
            break;
    }
    return true;
}
bool RecordManager::DropIndex(MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute)
{
    std::string index_name = MINI_TYPE::IndexName(table.name, attribute.name);
//    table.indices.erase(attribute.name);
    im->DropIndex(index_name);
    return true;
}
bool RecordManager::InsertRecord(MINI_TYPE::TableInfo & table, const MINI_TYPE::Record & record)
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
            for (int i = 0; i < record.values.size(); i++)
            {
                if (table.indices.find(table.attributes[i].name) != table.indices.end())
                    im->InsertKey(table.indices.at(table.attributes[i].name), record.values[i], iter.CurrentIndex());
            }
            break;
        }
        iter.Next(true);
    }
    
    table.record_count ++;
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
       const std::vector<MINI_TYPE::Condition> & conditions, const std::string & attr_using_index)
{
    MINI_TYPE::Table result(table);
    MINI_TYPE::Condition cond_using_index;
    using MINI_TYPE::Operator;
    if (table.indices.find(attr_using_index) == table.indices.end())
    {
        std::cerr << "Index on " + attr_using_index + " not found! Using select without index...\n";
        return SelectRecord(table, conditions);
        
    }
    std::string index = table.indices.at(attr_using_index);
    
    for (auto & cond : conditions)
    {
        if (cond.attributeName == attr_using_index)
        {
            cond_using_index = cond;
        }
    }
    IndexManager::iterator start;
    IndexManager::iterator finish;
    auto op = cond_using_index.op;
    if (op == Operator::LessThan or op == Operator::LessEqual)
        start = im->Begin(index);
    else
        start = im->Find(index, cond_using_index.value);
    if (op == Operator::GreaterThan or op == Operator::GreaterEqual)
        finish = im->End(index);
    else
        finish = im->Find(index, cond_using_index.value);
    IndexManager::iterator iter;
    for (iter = start; iter != finish; iter++)
    {
        auto record_fetcher = RecordIterator(table, (*iter).second, bm);
        MINI_TYPE::Record temp;
        if (record_fetcher.Read(temp) and MINI_TYPE::Test(cond_using_index, table, temp))
            result.records.push_back(temp);
    }
    // fetch last and test

    if (iter != im->End(index))
    {
        auto record_fetcher = RecordIterator(table, (*iter).second, bm);
        MINI_TYPE::Record temp;
        if (record_fetcher.Read(temp) and MINI_TYPE::Test(cond_using_index, table, temp))
            result.records.push_back(temp);
    }
    

    return result.Select(conditions);
    
}

bool RecordManager::DeleteRecord(MINI_TYPE::TableInfo & table, const vector<MINI_TYPE::Condition> & conditions)
{
    auto iter = RecordIterator(table, 0, bm);
    while (true)
    {
        MINI_TYPE::Record temp;
        if (iter.Read(temp) and MINI_TYPE::Test(conditions, table, temp))
        {
            iter.Delete();
            for (int i = 0; i < temp.values.size(); i++)
            {
                if (table.indices.find(table.attributes[i].name) != table.indices.end())
                    im->RemoveKey(table.indices.at(table.attributes[i].name), temp.values[i]);
            }
        }
        
        if (not iter.Next())
            break;
    }
    table.record_count--;
    return true;
}
