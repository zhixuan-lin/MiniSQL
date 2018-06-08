#include "CatalogManager.h"

CatalogManager::CatalogManager() {
    ReadCatalogFromFile();
}

CatalogManager::~CatalogManager() {
    SaveCatalogToFile();
}

void CatalogManager::ReadCatalogFromFile() {
    // TODO: index file
    using namespace std;
    fstream infile(CATALOG_TABLE_PATH, ios_base::in);

    string data;
    if (infile.good())
        while (getline(infile, data)) {
            if (data.empty())
                continue;
            stringstream ss(data);
            TableInfo temp;
            ss >> temp;
            tableInfos.push_back(temp);
        }
}

void CatalogManager::SaveCatalogToFile() {
    // TODO: index file
    using namespace std;
    fstream outfile(CATALOG_TABLE_PATH, ios_base::out);

    for (auto &tableInfo : tableInfos)
        outfile << tableInfo;
}

bool CatalogManager::TableExists(char *tableName) {
    // TODO: better with std::find_if
    for (auto &tableInfo : tableInfos)
        if (strcmp(tableInfo.tableName, tableName) == 0)
            return true;
    return false;
}

bool CatalogManager::CreateTable(TableInfo tableInfo) {
    tableInfos.push_back(tableInfo);
    return true;    // TODO: is return value necessary?
}

bool CatalogManager::DeleteTable(char *tableName) {
    // TODO: remove_if?
    for (auto it = tableInfos.begin(); it != tableInfos.end(); it++)
        if (strcmp(it->tableName, tableName) == 0) {
            tableInfos.erase(it);
            return true;
        }
    return false;
}