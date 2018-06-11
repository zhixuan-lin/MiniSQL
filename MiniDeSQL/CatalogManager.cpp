#include "CatalogManager.h"

CatalogManager::CatalogManager() {
    ReadCatalogFromFile();
}

CatalogManager::~CatalogManager() {
    SaveCatalogToFile();
}

void CatalogManager::ReadCatalogFromFile() {
    using namespace std;
    fstream infile(logFileName, ios_base::in);

    string data;
    if (infile.good())
        while (getline(infile, data)) {
            if (data.empty() || data == "\n")
                continue;
            stringstream ss(data);
            MINI_TYPE::TableInfo temp;
            ss >> temp;
            tableInfos.push_back(temp);
        }
}

void CatalogManager::SaveCatalogToFile() {
    using namespace std;
    fstream outfile(logFileName, ios_base::out);

    for (auto &tableInfo : tableInfos)
        outfile << tableInfo;
}

bool CatalogManager::TableExists(std::string tableName) const{
    for (auto &table : tableInfos)
        if(tableName == table.name)
            return true;
    return false;
}

bool CatalogManager::CreateTable(MINI_TYPE::TableInfo tableInfo){
    tableInfos.push_back(tableInfo);
    return true;
}

bool CatalogManager::DeleteTable(std::string tableName) {
    for(auto it = tableInfos.begin(); it != tableInfos.end(); it++){
        if(it->name == tableName){
            tableInfos.erase(it);
            return true;
        }
    }
    return false;
}