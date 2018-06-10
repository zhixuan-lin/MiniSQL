#include "RecordManager.hpp"
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

private:
    BufferManager *bm;
    IndexManager *im;
};
