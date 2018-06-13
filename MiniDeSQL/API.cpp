#include "API.h"

API *API::api = new API();

API::API() {
    auto im = new IndexManager;
    auto bm = new BufferManager;
    rm = new RecordManager(bm, im);
    cm = new CatalogManager;

    // rebuild indices
    for (auto &index : cm->GetIndices())
        rm->BuildIndex(cm->GetTableByName(index.table), cm->GetAttrByName(index.table, index.attribute));
}

API::~API() {
    delete rm;
    delete cm;
}

bool API::Exit() {
    delete api;
}

bool API::Execute(MINI_TYPE::SqlCommand sqlCommand) {
    using namespace MINI_TYPE;
    switch (sqlCommand.commandType) {
        case CreateTableCmd:
            CreateTable(sqlCommand.tableInfo);
            break;
        case DropTableCmd:
            DropTable(sqlCommand.tableName);
            break;
        case CreateIndexCmd:
            CreateIndex(sqlCommand.indexInfo);
            break;
        case DropIndexCmd:
            DropIndex(sqlCommand.indexName);
            break;
        case SelectCmd:
            Select(sqlCommand.tableName,
                   sqlCommand.condArray,
                   sqlCommand.attrList);
            break;
        case InsertCmd:
            Insert(sqlCommand.tableName,
                   sqlCommand.valueArray);
            break;
        case DeleteCmd:
            Delete(sqlCommand.tableName,
                   sqlCommand.condArray);
            break;
        default:
            // DO NOTHING
            break;
    }
}

bool API::CreateTable(MINI_TYPE::TableInfo tableInfo) {

    // 1) check if the table exists

    if (api->cm->TableExists(tableInfo.name)) {
        std::cerr << "Table " << tableInfo.name << " exists." << std::endl;
        return false;
    }

    // 2) check if the parameters are valid

    if (tableInfo.primaryKey.empty()) {
        std::cerr << "No primary key." << std::endl;
        return false;
    }

    for (auto &attr : tableInfo.attributes)
        if (attr.type.type == MINI_TYPE::TypeId::MiniChar && !MINI_TYPE::IsValidString(attr.type.char_size)) {
            std::cerr << "String out of range." << std::endl;
            return false;
        }

    // 3) start creating table

    api->cm->MakeAttrUnique(tableInfo, tableInfo.primaryKey);
    api->cm->CreateTable(tableInfo);
    api->rm->CreateTableFile(tableInfo);
    api->cm->CreateIndex(tableInfo.name, tableInfo.primaryKey);
    api->rm->BuildIndex(tableInfo, api->cm->GetAttrByName(tableInfo, tableInfo.primaryKey));
    api->cm->SaveCatalogToFile();

    return true;
}

bool API::CreateIndex(const MINI_TYPE::IndexInfo indexInfo) {

    // 1) check if the index exists

    if (api->cm->IndexExists(indexInfo.name)) {
        std::cerr << "Index " << indexInfo.name << " exists." << std::endl;
        return false;
    }

    // 2) check if the table exists

    if (!api->cm->TableExists(indexInfo.table)) {
        std::cerr << "Table " << indexInfo.table << " does not exist." << std::endl;
        return false;
    }

    // 3) check if the attribute is unique
    auto &attr = api->cm->GetAttrByName(indexInfo.table, indexInfo.attribute);
    if (!attr.unique) {
        std::cerr << "Attribute " << attr.name << " is not unique." << std::endl;
        return false;
    }

    // 4) start creating index
    api->cm->CreateIndex(indexInfo);
    api->cm->AttachIndexToTable(indexInfo);
    // TODO: index file?

    auto &tableInfo = api->cm->GetTableByName(indexInfo.table);
    api->rm->BuildIndex(tableInfo, api->cm->GetAttrByName(tableInfo, indexInfo.attribute));
    api->cm->SaveCatalogToFile();

    return true;
}

bool API::DropTable(const std::string tableName) {

    // 1) check if the table exists

    if (!api->cm->TableExists(tableName)) {
        std::cerr << "Table " << tableName << " does not exist." << std::endl;
        return false;
    }

    // 2) start deleting
    api->rm->DropIndex(api->cm->GetIndexConcerned(tableName));
    api->cm->DeleteTable(tableName); // includes DeleteIndex
    api->cm->SaveCatalogToFile();

    return true;
}

bool API::DropIndex(std::string indexName) {

    // 1) check if the index exists

    if (!api->cm->IndexFindAndNormalizeAlias(indexName)) {
        std::cerr << "Index " << indexName << " does not exist." << std::endl;
        return false;
    }

    // 2) start deleting

    api->rm->DropIndex(indexName);
    api->cm->DeleteIndex(indexName);
    api->cm->SaveCatalogToFile();

    return true;
}

bool API::Delete(const std::string tableName, const std::vector<MINI_TYPE::Condition> condList) {

    // 1) check if the table exists

    if (!api->cm->TableExists(tableName)) {
        std::cerr << "Table " << tableName << " does not exist." << std::endl;
        return false;
    }

    // 2) start deleting
    api->rm->DeleteRecord(api->cm->GetTableByName(tableName), condList);

    return true;
}

bool API::Select(const std::string tableName, const std::vector<MINI_TYPE::Condition> condList,
                 std::vector<std::string> attrList) {

    if (attrList.empty())
        attrList = api->cm->GetAttrNames(tableName);

    // 1) check if the table exists

    if (!api->cm->TableExists(tableName)) {
        std::cerr << "Table " << tableName << " does not exist." << std::endl;
        return false;
    }

    // 2) start selecting
    MINI_TYPE::Table tableRes;

    bool existNeq = false;
    for (auto &cond : condList)
        if (cond.op == MINI_TYPE::Operator::NotEqual)
            existNeq = true;

    auto &tableInfo = api->cm->GetTableByName(tableName);
    if (existNeq)
        tableRes = api->rm->SelectRecord(tableInfo, condList);
    else
        tableRes = api->rm->SelectRecord(tableInfo, condList, api->cm->GetPrimaryIndex(tableInfo));

    tableRes.DisplayAttr(attrList);

    return true;
}

bool API::Insert(std::string tableName, std::vector<MINI_TYPE::SqlValue> valueList) {
    // 1) check if the table exists

    if (!api->cm->TableExists(tableName)) {
        std::cerr << "Table " << tableName << " does not exist." << std::endl;
        return false;
    }

    // 2) check if the value types are valid

    auto &tableInfo = api->cm->GetTableByName(tableName);
    if (tableInfo.attributes.size() != valueList.size()) {
        std::cerr << "Invalid size of value list." << std::endl;
        return false;
    }

    for (int i = 0; i < tableInfo.attributes.size(); i++) {

        if (tableInfo.attributes[i].type != valueList[i].type) {
            std::cerr << "Type mismatch." << std::endl;
            return false;
        }

        if (valueList[i].type.type == MINI_TYPE::TypeId::MiniChar &&
            !MINI_TYPE::IsValidString(valueList[i].type.char_size)) {
            std::cerr << "Invalid string." << std::endl;
            return false;
        }

    }

    // 3) start inserting

    MINI_TYPE::Record record(valueList);
    api->rm->InsertRecord(tableInfo, record);

    return true;
}