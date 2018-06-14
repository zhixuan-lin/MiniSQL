#include "Interpreter.h"

#include <cassert>

using namespace MINI_TYPE;

void CreateTableTest();

void DropTableTest();

void CreateIndexTest();

void DropIndexTest();

void SelectTest();

void InsertTest();

void DeleteTest();

int main() {

    CreateTableTest();
    DropTableTest();
    CreateIndexTest();
    DropIndexTest();
    SelectTest();
    InsertTest();
    DeleteTest();

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

    auto command = Interpreter::Parse("    create table student (\n"
                                      "            sno char(8),\n"
                                      "            sname char(16) unique,\n"
                                      "            sage int,\n"
                                      "            sgender char (1),\n"
                                      "            primary key ( sno )\n"
                                      "    );");

    assert(command.commandType == CreateTableCmd);
    assert(command.tableInfo.name == "student");
    assert(command.tableInfo.primaryKey == "sno");

    assert(command.tableInfo.attributes[0].name == "sno");
    assert(command.tableInfo.attributes[0].type.type == MiniChar);
    assert(command.tableInfo.attributes[0].type.char_size == 8);
    assert(!command.tableInfo.attributes[0].unique);

    assert(command.tableInfo.attributes[1].name == "sname");
    assert(command.tableInfo.attributes[1].type.type == MiniChar);
    assert(command.tableInfo.attributes[1].type.char_size == 16);
    assert(command.tableInfo.attributes[1].unique);

    assert(command.tableInfo.attributes[2].name == "sage");
    assert(command.tableInfo.attributes[2].type.type == MiniInt);
    assert(!command.tableInfo.attributes[2].unique);

    assert(command.tableInfo.attributes[3].name == "sgender");
    assert(command.tableInfo.attributes[3].type.type == MiniChar);
    assert(command.tableInfo.attributes[3].type.char_size == 1);
    assert(!command.tableInfo.attributes[3].unique);
}

void DropTableTest() {
//    drop table student;

    auto command = Interpreter::Parse("drop table student;");

    assert(command.commandType == DropTableCmd);
    assert(command.tableName == "student");
}

void CreateIndexTest() {
//    create index stunameidx on student ( sname );

    auto command = Interpreter::Parse("create index stunameidx on student ( sname );");

    assert(command.commandType == CreateIndexCmd);
    assert(command.indexInfo.alias == "stunameidx");
    assert(command.indexInfo.table == "student");
    assert(command.indexInfo.attribute == "sname");
}

void DropIndexTest() {
//    drop index stunameidx;

    auto command = Interpreter::Parse("drop index stunameidx;");

    assert(command.commandType == DropIndexCmd);
    assert(command.indexName == "stunameidx");
}

void SelectTest() {
//    select * from student;
//    select * from student where sno = ‘88888888’;
//    select * from student where sage > 20 and sgender = ‘F’;

    auto command0 = Interpreter::Parse("select * from student;");

    assert(command0.commandType == SelectCmd);
    assert(command0.tableName == "student");
    assert(command0.condArray.empty());
    assert(command0.attrList.empty());

    auto command1 = Interpreter::Parse("select * from student where sno = ‘88888888’;");

    assert(command1.commandType == SelectCmd);
    assert(command1.tableName == "student");
    assert(command1.condArray[0].attributeName == "sno");
//    assert(command1.condArray[0].value.type == MiniChar);
    assert(command1.condArray[0].value.str == "88888888");
    assert(command1.condArray[0].op == Equal);
    assert(command1.attrList.empty());

    auto command2 = Interpreter::Parse("select * from student where sage > 20 and sgender = ‘F’;");

    assert(command2.commandType == SelectCmd);
    assert(command2.tableName == "student");
    assert(command2.condArray[0].attributeName == "sage");
//    assert(command2.condArray[0].value.type == MiniInt);
    assert(command2.condArray[0].value.i == 20);
    assert(command2.condArray[0].op == GreaterThan);
    assert(command2.condArray[1].attributeName == "sgender");
//    assert(command2.condArray[1].value.type == MiniChar);
    assert(command2.condArray[1].value.str == "F");
    assert(command2.condArray[1].op == Equal);
    assert(command2.attrList.empty());
}

void InsertTest() {
//    insert into student values (‘12345678’,’wy’,22,’M’);

    auto command = Interpreter::Parse("insert into student values (‘12345678’,’wy’,22,’M’);");

    assert(command.commandType == InsertCmd);
    assert(command.tableName == "student");
//    assert(command.valueArray[0].type.type == MiniChar);
    assert(command.valueArray[0].str == "12345678");
//    assert(command.valueArray[1].type.type == MiniChar);
    assert(command.valueArray[1].str == "wy");
//    assert(command.valueArray[2].type.type == MiniInt);
    assert(command.valueArray[2].i == 22);
//    assert(command.valueArray[3].type.type == MiniChar);
    assert(command.valueArray[3].str == "M");
    assert(command.valueArray.size() == 4);
}

void DeleteTest() {
//    delete from student;
//    delete from student where sno = ‘88888888’;

    auto command0 = Interpreter::Parse("delete from student;");

    assert(command0.commandType == DeleteCmd);
    assert(command0.tableName == "student");
    assert(command0.condArray.empty());

    auto command1 = Interpreter::Parse("delete from student where sno = ‘88888888’;");

    assert(command1.commandType == DeleteCmd);
    assert(command1.tableName == "student");
    assert(command1.condArray.size() == 1);
    assert(command1.condArray[0].op == Equal);
    assert(command1.condArray[0].value.str == "88888888");
    assert(command1.condArray[0].attributeName == "sno");
}
