#include "CatalogManager.h"
#include <utility>

CatalogManager::CatalogManager() {
    ReadCatalogFromFile();
}

CatalogManager::~CatalogManager() {
    SaveCatalogToFile();
}

void CatalogManager::ReadCatalogFromFile() {
    using namespace std;
    fstream tableFile(tableLogFile, ios_base::in);
    fstream indexFile(indexLogFile, ios_base::in);

    string data;

    if (tableFile.good())
        while (getline(tableFile, data)) {
            if (data.empty() || data == "\n")
                continue;
            stringstream ss(data);
            MINI_TYPE::TableInfo temp;
            ss >> temp;
            tableInfos.push_back(temp);
        }

    if (indexFile.good())
        while (getline(indexFile, data)) {
            if (data.empty() || data == "\n")
                continue;
            stringstream ss(data);
            MINI_TYPE::IndexInfo temp;
            ss >> temp;
            indexInfos.push_back(temp);
        }
}

void CatalogManager::SaveCatalogToFile() {
    using namespace std;
    fstream tableFile(tableLogFile, ios_base::out);
    fstream indexFile(indexLogFile, ios_base::out);

    for (auto &tableInfo : tableInfos)
        tableFile << tableInfo;

    for (auto &indexInfo : indexInfos)
        indexFile << indexInfo;
}

bool CatalogManager::TableExists(std::string tableName) const{
    for (auto &table : tableInfos)
        if(tableName == table.name)
            return true;
    return false;
}

bool CatalogManager::CreateTable(MINI_TYPE::TableInfo& tableInfo){
    tableInfo.record_count = 0;
    tableInfo.record_length = 0;
    for (auto &attr : tableInfo.attributes)
        tableInfo.record_length += attr.type.TypeSize();
    AttachIndexToTable(tableInfo, tableInfo.primaryKey);
    tableInfos.push_back(tableInfo);
    return true;
}

bool CatalogManager::DeleteTable(std::string tableName) {
    for(auto it = tableInfos.begin(); it != tableInfos.end(); it++){
        if(it->name == tableName){
            for (auto &pair : it->indices)
                DeleteIndex(pair.second);
            tableInfos.erase(it);
            return true;
        }
    }
    return false;
}


bool CatalogManager::IndexExists(std::string indexName) const {
    for (auto &index : indexInfos)
        if (indexName == index.name || indexName == index.alias)
            return true;
    return false;
}

bool CatalogManager::CreateIndex(MINI_TYPE::IndexInfo indexInfo) {
    indexInfos.push_back(indexInfo);
    return true;
}

bool CatalogManager::DeleteIndex(std::string indexName) {
    for (auto it = indexInfos.begin(); it != indexInfos.end(); it++) {
        if (it->name == indexName) {
            auto &tableInfo = GetTableByName(it->table);
            tableInfo.indices.erase(it->attribute);
            indexInfos.erase(it);
            return true;
        }
    }
    return false;
}

bool CatalogManager::CreateIndex(std::string tableName, std::string attrName) {
    MINI_TYPE::IndexInfo indexInfo(std::move(tableName), std::move(attrName));
    indexInfos.push_back(indexInfo);
}

void CatalogManager::MakeAttrUnique(MINI_TYPE::TableInfo tableInfo, std::string attrName) {
    auto &attr = GetAttrByName(std::move(tableInfo), std::move(attrName));
    attr.unique = true;
}

MINI_TYPE::Attribute &CatalogManager::GetAttrByName(MINI_TYPE::TableInfo tableInfo, std::string attrName) {
    for (auto &attr : tableInfo.attributes)
        if (attr.name == attrName)
            return attr;
}

MINI_TYPE::Attribute &CatalogManager::GetAttrByName(std::string tableName, std::string attrName) {
    auto &tableInfo = GetTableByName(std::move(tableName));
    for (auto &attr : tableInfo.attributes)
        if (attr.name == attrName)
            return attr;
}

MINI_TYPE::TableInfo &CatalogManager::GetTableByName(std::string tableName) {
    for (auto &table : tableInfos)
        if (table.name == tableName)
            return table;
}

void CatalogManager::AttachIndexToTable(MINI_TYPE::IndexInfo indexInfo) {
    auto &tableInfo = GetTableByName(indexInfo.table);
    tableInfo.indices[indexInfo.attribute] = indexInfo.name;
}

void CatalogManager::AttachIndexToTable(MINI_TYPE::TableInfo &tableInfo, std::string attrName) {
    tableInfo.indices[attrName] = MINI_TYPE::IndexName(tableInfo.name, attrName);
}

std::vector<std::string> CatalogManager::GetIndexConcerned(std::string tableName) {
    std::vector<std::string> indexNames;
    auto &tableInfo = GetTableByName(std::move(tableName));
    for (auto &pair : tableInfo.indices)
        indexNames.push_back(pair.second);
    return indexNames;
}

MINI_TYPE::IndexInfo &CatalogManager::GetIndexByName(std::string indexName) {
    for (auto &index : indexInfos)
        if (index.name == indexName)
            return index;
}

MINI_TYPE::IndexInfo &CatalogManager::GetPrimaryIndex(MINI_TYPE::TableInfo tableInfo) {
    return GetIndexByName(MINI_TYPE::IndexName(tableInfo.name, tableInfo.primaryKey));
}

bool CatalogManager::IndexFindAndNormalizeAlias(std::string& indexAlias) {
    for (auto &index : indexInfos)
        if (indexAlias == index.name || indexAlias == index.alias) {
            indexAlias = index.name;
            return true;
        }
    return false;
}

std::vector<std::string> &CatalogManager::GetAttrNames(std::string tableName) {
    std::vector<std::string> res;
    auto &tableInfo = GetTableByName(std::move(tableName));
    for (auto &attr : tableInfo.attributes)
        res.push_back(attr.name);
    return res;
}