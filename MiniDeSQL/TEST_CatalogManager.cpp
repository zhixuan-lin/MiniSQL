#include "CatalogManager.h"
#include <cassert>

int main(){
    using namespace MINI_TYPE;

    auto cm = new CatalogManager();

    TableInfo tableInfo;
    tableInfo.name = "testTable";
    tableInfo.record_length = 128;
    tableInfo.record_count = 2333;
    Attribute test1("testAttr", MiniInt);
    Attribute test2("testAttr2", MiniFloat);
    tableInfo.attributes.push_back(test1);
    tableInfo.attributes.push_back(test2);
    tableInfo.indices["123"] = "321";
    tableInfo.indices["233"] = "332";

    assert(cm->CreateTable(tableInfo));
    assert(cm->TableExists("testTable"));
    delete cm;

    auto cmNew = new CatalogManager();
    assert(cmNew->TableExists("testTable"));
    assert(cmNew->DeleteTable("testTable"));
    delete cmNew;

    auto cmNewNew = new CatalogManager();
    assert(!cmNewNew->TableExists("testTable"));

    return 0;
}