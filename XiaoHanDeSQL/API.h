#ifndef MINISQL_API_H
#define MINISQL_API_H

#include "MiniType.h"
#include "CatalogManager.h"
#include "RecordManager.h"
#include "IndexManager.h"

#include <iostream>

class API {
public:
    void Execute(SqlCommand sqlCommand);

private:
    bool CreateTable(TableInfo tableInfo);

    bool DropTable(TableName tableName);

    bool CreateIndex(IndexInfo indexInfo);

    bool DropIndex(IndexName indexName);

    bool Select(TableName tableName, CondArray condArray);

    bool Insert(TableName tableName, ValueArray valueArray);

    bool Delete(TableName tableName, CondArray condArray);
    // TODO: Quit and Execfile should be processed in Interpreter

    CatalogManager catalogManager;
    RecordManager recordManager;
    IndexManager indexManager;
};


#endif //MINISQL_API_H
