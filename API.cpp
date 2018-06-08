#include "API.h"

void API::Execute(SqlCommand sqlCommand) {
    // TODO: add timing
    switch (sqlCommand.commandType) {
        case CreateTableCmd:
            CreateTable(sqlCommand.commandInfo.tableInfo);
            break;
        case DropTableCmd:
            DropTable(sqlCommand.commandInfo.tableName);
            break;
        case CreateIndexCmd:
            CreateIndex(sqlCommand.commandInfo.indexInfo);
            break;
        case DropIndexCmd:
            DropIndex(sqlCommand.commandInfo.indexName);
            break;
        case SelectCmd:
            Select(sqlCommand.commandInfo.tableName,
                   sqlCommand.commandInfo.condArray);
            break;
        case InsertCmd:
            Insert(sqlCommand.commandInfo.tableName,
                   sqlCommand.commandInfo.valueArray);
            break;
        case DeleteCmd:
            Delete(sqlCommand.commandInfo.tableName,
                   sqlCommand.commandInfo.condArray);
            break;
        default:
            // DO NOTHING
            break;
    }

}

bool API::CreateTable(TableInfo tableInfo) {
    using namespace std;
    // 1) CM: check if the table exists;
    // 2) CM: create the table in the catalog;
    // 3) RM: create a file for the table;
    // 4) IM: create an index on the primary key.

    if (catalogManager.TableExists(tableInfo.tableName)) {
        // TODO: use exception?
        cout << "Table " << tableInfo.tableName << " exists." << endl;
        return false;
    }

    catalogManager.CreateTable(tableInfo);

    recordManager.CreateTableFile(tableInfo.tableName);

    indexManager.CreateIndex(tableInfo);
}

bool API::DropTable(char *tableName) {
    using namespace std;
    // 1) CM: check if the table exists;
    // 2) IM: delete all indices on the table.
    // 3) CM: delete the table in the catalog;
    // 4) RM: delete the file for the table;

    if (!catalogManager.TableExists(tableName)) {
        cout << "Table " << tableName << " does not exist." << endl;
        return false;
    }

    auto indicesToDelete = catalogManager.GetIndicesOnTable(tableName);
    indexManager.DeleteIndices(indicesToDelete);

    catalogManager.DeleteTable(tableName);

    recordManager.DeleteTableFile(tableName);

}

bool API::CreateIndex(IndexInfo indexInfo) {
    using namespace std;
    // 1) CM: check if the index exists;
    // 2) IM: create the index;
    // 3) CM: attach the index to the table.

    if (catalogManager.IndexExists(indexInfo.indexName)) {
        cout << "Index " << indexInfo.indexName << " exists." << endl;
        return false;
    }

    // call bufferManager to open the file which stores data
    // find the corresponding column
    // TODO: To be discussed
    indexManager.CreateIndex(indexInfo);

    catalogManager.AddIndex(indexInfo);
}

bool API::DropIndex(char *indexName) {
    using namespace std;
    // 1) CM: check if the index exists;
    // 2) IM: delete the index;
    // 3) CM: detach the index from the table.

    if (!catalogManager.IndexExists(indexName)) {
        cout << "Index " << indexName << " does not exist." << endl;
        return false;
    }

    indexManager.DeleteIndex(indexName);

    catalogManager.DeleteIndex(indexName);
}

bool API::Select(char *tableName, CondArray condArray) {
    // 1) CM: check if indices concerned exist and return vector<IndexInfo>;
    // 2) IM: if there are available indices return vector<IndexNode>
    // TODO: intersection inside indexManager?
    // 3) RM: do the selection w/ or w/o indices returned by IM.
}

bool API::Insert(char *tableName, ValueArray valueArray) {
    using namespace std;
    // 1) CM: check if the values are valid & if table exists
    // 1) RM: insert the record;
    // 2) IM: update the index if necessary.

    if (!catalogManager.TableExists(tableName)) {
        cout << "Table " << tableName << " does not exist." << endl;
        return false;
    }

    if (!catalogManager.ValidValue(tableName, valueArray)) {
        cout << "Invalid values." << endl;
        return false;
    }

    auto indexNode = recordManager.InsertRecord(tableName, valueArray);
    auto indicesConcerned = catalogManager.GetIndicesOnTable(tableName);

    indexManager.AddIndexNode(indicesConcerned, indexNode, valueArray);
}

bool API::Delete(char *tableName, CondArray condArray) {
    // 1) CM: check if indices concerned exist and return vector<IndexInfo>;
    // 2) IM: if there are available indices return vector<IndexNode>
    // 3) RM: do the deletion w/ or w/o indices returned by IM.
}