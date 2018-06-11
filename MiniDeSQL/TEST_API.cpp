#include "API.h"

#include <cassert>

using namespace MINI_TYPE;

void CreateTableTest();

int main() {

    CreateTableTest();

    return 0;
}

void CreateTableTest() {
//    create table student (
//            sno char(8),
//            sname char(16) unique,
//            sage int,
//            sgender char (1),
//            primary key ( sno )
//    );

    MINI_TYPE::TableInfo tableInfo;
    tableInfo.name = "student";
    SqlValueType t0(MiniChar, 8);
    SqlValueType t1(MiniChar, 16);
    SqlValueType t2(MiniInt);
    SqlValueType t3(MiniChar, 1);
    Attribute a0("sno", t0);
    Attribute a1("sname", t1, false, true);
    Attribute a2("sage", t2);
    Attribute a3("sgender", t3);
    tableInfo.attributes.push_back(a0);
    tableInfo.attributes.push_back(a1);
    tableInfo.attributes.push_back(a2);
    tableInfo.attributes.push_back(a3);
    tableInfo.primaryKey = "sno";

    API::CreateTable(tableInfo);
}