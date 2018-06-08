#ifndef MINISQL_INDEXMANAGER_H
#define MINISQL_INDEXMANAGER_H

#include "MiniType.h"

class IndexManager {
public:
    bool CreateIndex(IndexInfo indexInfo);

    bool CreateIndex(TableInfo tableInfo);

    bool DeleteIndex(IndexName indexInfo);

    bool DeleteIndices(std::vector<IndexInfo> indexInfos);

    bool AddIndexNode(std::vector<IndexInfo> indexInfos, IndexNode indexNode, ValueArray valueArray);
};

#endif //MINISQL_INDEXMANAGER_H
