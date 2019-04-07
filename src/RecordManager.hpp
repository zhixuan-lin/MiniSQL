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
    bool BuildIndex(MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);
    bool DropIndex(MINI_TYPE::TableInfo & table, const MINI_TYPE::Attribute & attribute);
    bool InsertRecord(MINI_TYPE::TableInfo & table, const MINI_TYPE::Record & record);
    MINI_TYPE::Table SelectRecord(const MINI_TYPE::TableInfo & table, \
            const std::vector<MINI_TYPE::Condition> & conditions = vector<MINI_TYPE::Condition>());
    MINI_TYPE::Table SelectRecord(const MINI_TYPE::TableInfo & table, \
                                  const std::vector<MINI_TYPE::Condition> & conditions, const std::string & attr_using_index);
    bool DeleteRecord(MINI_TYPE::TableInfo & table, const vector<MINI_TYPE::Condition> & conditions = vector<MINI_TYPE::Condition>());
// private:
    BufferManager * bm;
    IndexManager * im;
    class RecordIterator
    {
    public:
        RecordIterator() {}
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
