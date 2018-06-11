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

    bool CreateTable(MINI_TYPE::TableInfo tableInfo);

    bool DeleteTable(std::string tableName);

private:
    std::vector<MINI_TYPE::TableInfo> tableInfos;

    static constexpr auto logFileName = "meta.log";
};


#endif //MINISQL_CATALOGMANAGER_H
