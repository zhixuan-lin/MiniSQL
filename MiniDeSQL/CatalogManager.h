#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H

#include "MiniType.h"

#include <fstream>
#include <sstream>

class CatalogManager {
public:
    CatalogManager();

    ~CatalogManager();

    void ReadCatalogFromFile();

    void SaveCatalogToFile();

    bool TableExists(std::string tableName) const;

    bool CreateTable(MINI_TYPE::TableInfo& tableInfo);

    bool DeleteTable(std::string tableName);

    bool IndexExists(std::string indexName) const;

    bool IndexFindAndNormalizeAlias(std::string& indexAlias);

    bool CreateIndex(MINI_TYPE::IndexInfo indexInfo);

    bool CreateIndex(std::string tableName, std::string attrName);

    bool DeleteIndex(std::string indexName);

    std::vector<std::string> GetIndexConcerned(std::string tableName);

    void AttachIndexToTable(MINI_TYPE::IndexInfo indexInfo);

    void AttachIndexToTable(MINI_TYPE::TableInfo &tableInfo, std::string attrName);

    void MakeAttrUnique(MINI_TYPE::TableInfo tableInfo, std::string attrName);

    MINI_TYPE::Attribute &GetAttrByName(MINI_TYPE::TableInfo tableInfo, std::string attrName);

    MINI_TYPE::Attribute &GetAttrByName(std::string tableName, std::string attrName);

    MINI_TYPE::TableInfo &GetTableByName(std::string tableName);

    MINI_TYPE::IndexInfo &GetIndexByName(std::string indexName);

    MINI_TYPE::IndexInfo &GetPrimaryIndex(MINI_TYPE::TableInfo tableInfo);

private:
    std::vector<MINI_TYPE::TableInfo> tableInfos;
    std::vector<MINI_TYPE::IndexInfo> indexInfos;

    static constexpr auto tableLogFile = "metaTable.log";
    static constexpr auto indexLogFile = "metaIndex.log";
};


#endif //MINISQL_CATALOGMANAGER_H
