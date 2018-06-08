#ifndef MINISQL_RECORDMANAGER_H
#define MINISQL_RECORDMANAGER_H

#include "MiniType.h"

class RecordManager {
public:
    bool CreateTableFile(TableName tableName);

    bool DeleteTableFile(TableName tableName);

    IndexNode InsertRecord(TableName tableName, ValueArray valueArray);

};


#endif //MINISQL_RECORDMANAGER_H
