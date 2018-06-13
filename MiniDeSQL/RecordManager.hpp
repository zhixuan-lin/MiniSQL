#ifndef RecordManager_hpp
#define RecordManager_hpp

#include <iostream>

#include "MiniType.h"
#include "BufferManager.h"
#include "IndexManager.hpp"
class RecordManager
{
public:
    RecordManager(BufferManager *bm, IndexManager *im) : bm(bm), im(im) {}
    bool CreateTableFile(const MINI_TYPE::TableInfo & table);
    bool DeleteTableFile(const MINI_TYPE::TableInfo & table);
    bool BuildIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);
    bool DropIndex(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);

    bool DropIndex(std::string indexName);

    bool DropIndex(std::vector<std::string> indexNames);
    bool InsertRecord(const MINI_TYPE::TableInfo & table, const MINI_TYPE::Record & record);
    MINI_TYPE::Table SelectRecord(const MINI_TYPE::TableInfo & table, \
            const std::vector<MINI_TYPE::Condition> & conditions);
    MINI_TYPE::Table SelectRecord(const MINI_TYPE::TableInfo & table, \
                                  const std::vector<MINI_TYPE::Condition> & conditions, const MINI_TYPE::IndexInfo & index);
    bool DeleteRecord(const MINI_TYPE::TableInfo & table, const vector<MINI_TYPE::Condition> & conditions);
private:
    BufferManager * bm;
    IndexManager * im;
    class RecordIterator
    {
    public:
        RecordIterator(const MINI_TYPE::TableInfo & table, int record_index, BufferManager * bm);
        ~RecordIterator();
        bool Read(MINI_TYPE::Record & record);
        void Write(const MINI_TYPE::Record & r) const;
        void Delete();
        bool Next(bool expand = false);
        int CurrentIndex() {return record_index;}
    private:
        MINI_TYPE::TableInfo table;
        BufferManager * bm;
        Block * block;
        int record_length;
        int record_index;
        int records_per_block;
        int block_id;
        int in_block_record_index;
        int past_the_end_block_id;
    };
};


#endif /* RecordManager_hpp */
