#include "Test.h"

void Test::InterpreterTest() {
    Interpreter::ReadCommand();
}

void Test::CatalogTest() {
    auto cm = new CatalogManager();

    TableInfo tableInfo;
    strcpy(tableInfo.tableName, "testTable");

    Attribute attribute;
    attribute.valueType = MiniInt;
    strcpy(attribute.attributeName, "testInt");

    tableInfo.attrArray.push_back(attribute);
    tableInfo.primaryIdx = 0;

    assert(cm->CreateTable(tableInfo));
    assert(cm->TableExists(const_cast<char *>("testTable")));

    delete cm;

    auto cmNew = new CatalogManager();
    assert(cmNew->TableExists(const_cast<char *>("testTable")));

    assert(cmNew->DeleteTable(const_cast<char *>("testTable")));
    assert(!cmNew->TableExists(const_cast<char *>("testTable")));

    delete cmNew;

    auto cmNewNew = new CatalogManager();
    assert(!cmNewNew->TableExists(const_cast<char *>("testTable")));
}