#ifndef MINISQL_MINITYPE_H
#define MINISQL_MINITYPE_H

/*
 * FORMAT SPECIFICATION:
 *
 * .log  :  TableInfo OR IndexInfo
 *
 * .index:  Serialization of BPlusTree (Key: MiniValue, Value: IndexNode)
 *
 * .table:  Serialization of Records
 *
 */

#include <bits/exception.h>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

#define CATALOG_TABLE_PATH  "table.log"
#define CATALOG_INDEX_PATH  "index.log"

#define TABLE_NAME_LEN      32
#define INDEX_NAME_LEN      32
#define ATTRIBUTE_NAME_LEN  32
#define FILE_NAME_LEN       32

#define STRING_MAX_LEN      255
// TODO: char(x) support

#define BLOCKNUM            100
#define BLOCKSIZE           1024

typedef char TableName[TABLE_NAME_LEN];
typedef char IndexName[INDEX_NAME_LEN];
typedef char AttributeName[ATTRIBUTE_NAME_LEN];
typedef char FileName[FILE_NAME_LEN];

enum SqlCommandType {
    CreateTableCmd,    // arg: TableInfo
    DropTableCmd,      // arg: TableName
    CreateIndexCmd,    // arg: IndexInfo
    DropIndexCmd,      // arg: IndexName
    SelectCmd,         // arg: TableName, CondArray
    InsertCmd,         // arg: TableName, ValueArray
    DeleteCmd,         // arg: TableName, CondArray
    QuitCmd,           // arg:
    ExecfileCmd        // arg: FileName
};

enum OpType {
    Equal,
    NotEqual,
    GreaterThan,
    LessThan,
    GreaterEqual,
    LessEqual
};

enum ValueType {
    MiniInt,
    MiniFloat,
    MiniChar
};

union ValueStorage {
    int intValue;
    float floatValue;
    char charValue[STRING_MAX_LEN];
};

typedef std::vector<ValueStorage> ValueArray;

struct MiniValue {
    ValueType valueType;
    ValueStorage valueStorage;
};

typedef std::vector<MiniValue> Record;

struct Attribute {
    ValueType valueType;
    AttributeName attributeName;

    friend std::ostream &operator<<(std::ostream &out, const Attribute attribute) {
        out << attribute.valueType << ' ' << attribute.attributeName;
    }

    friend std::istream &operator>>(std::istream &in, Attribute &attribute) {
        int typeNum = 0;
        in >> typeNum;

        switch (typeNum) {
            case MiniInt  :
                attribute.valueType = MiniInt;
                break;
            case MiniFloat:
                attribute.valueType = MiniFloat;
                break;
            case MiniChar :
                attribute.valueType = MiniChar;
                break;
            default:
                break;
        }

        in >> attribute.attributeName;
    }
};

typedef std::vector<Attribute> AttrArray;

struct IndexInfo {
    IndexName indexName;
    TableName tableName;
    AttributeName attributeName;
    int attributeIdx;   // TODO: should we keep this?
};

struct IndexNode {
    int  blockIdx;
    int  offset;
    bool isLeaf;
};

struct TableInfo {
    // TODO: ctor
    TableName tableName;
    AttrArray attrArray;
    int primaryIdx;

    friend std::ostream &operator<<(std::ostream &out, const TableInfo tableInfo) {
        out << tableInfo.tableName << ' ' << tableInfo.primaryIdx << ' ' << tableInfo.attrArray.size() << ' ';
        for (auto &attribute : tableInfo.attrArray)
            out << attribute << ' ';
        out << std::endl;
    }

    friend std::istream &operator>>(std::istream &in, TableInfo &tableInfo) {
        in >> tableInfo.tableName >> tableInfo.primaryIdx;

        int attrSize = 0;
        in >> attrSize;

        tableInfo.attrArray.resize(static_cast<unsigned long>(attrSize));

        for (int i = 0; attrSize > i; i++)
            in >> tableInfo.attrArray[i];
    }
};

struct Condition {
    AttributeName attributeName;
    OpType opType;
    MiniValue value;
};

typedef std::vector<Condition> CondArray;

// do NOT use union here.
struct SqlCommandInfo {
    // TODO: ctor
    TableInfo tableInfo;
    IndexInfo indexInfo;
    TableName tableName;
    IndexName indexName;
    CondArray condArray;
    ValueArray valueArray;
    FileName fileName;    // TODO: may be redundant
};

struct SqlCommand {
    explicit SqlCommand(SqlCommandType sqlCommandType) : commandType(sqlCommandType) {};

    SqlCommandType commandType;
    SqlCommandInfo commandInfo;
};

class SyntaxError : public std::exception {
public:
    explicit SyntaxError(std::string errMessage) : errMessage(std::move(errMessage)) {};

    const char *what() const noexcept override {
        return errMessage.c_str();
    }

private:
    std::string errMessage;
};

#endif //MINISQL_MINITYPE_H
