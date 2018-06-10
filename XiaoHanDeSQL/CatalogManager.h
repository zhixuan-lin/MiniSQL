#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H

#include "MiniType.h"

#include <fstream>
#include <cstring>
#include <sstream>

class CatalogManager {
public:
    CatalogManager();

    ~CatalogManager();

    void ReadCatalogFromFile();

    void SaveCatalogToFile();

    bool TableExists(TableName tableName);

    bool CreateTable(TableInfo tableInfo);

    bool DeleteTable(TableName tableName);

    bool IndexExists(IndexName indexName);

    bool AddIndex(IndexInfo indexInfo);

    bool DeleteIndex(IndexName indexName);

    std::vector<IndexInfo> GetIndicesOnTable(TableName tableName);

    bool ValidValue(TableName tableName, ValueArray valueArray);

private:
    std::vector<TableInfo> tableInfos;
    std::vector<IndexInfo> indexInfos;
};


#endif //MINISQL_CATALOGMANAGER_H
