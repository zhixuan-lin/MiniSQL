#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include "MiniType.h"
#include "BufferManager.h"
#include "IndexManager.hpp"
#include "RecordManager.hpp"
using namespace MINI_TYPE;

int main()
{

    using namespace MINI_TYPE;
    const int Max = 100;
    BufferManager * bm = new BufferManager;
    IndexManager * im = new IndexManager;
    RecordManager * rm = new RecordManager(bm, im);

    Attribute sname("sname", SqlValueType(TypeId::MiniChar, 5));
    Attribute sno("sno", SqlValueType(TypeId::MiniInt));
    Attribute grade("sgrade", SqlValueType(TypeId::MiniFloat));

    std::vector<Attribute> vec {sname, sno, grade};

    TableInfo table("student", vec);
    rm->DeleteTableFile(table);
    rm->CreateTableFile(table);


    for (int i = 0; i < Max; i++)
    {
        Record record;
        record.values.push_back(SqlValue(sname.type, "s" + to_string(i)));
        record.values.push_back(SqlValue(sno.type, i));
        record.values.push_back(SqlValue(grade.type, float(std::rand() / Max)));
        rm->InsertRecord(table, record);
    }
    rm->BuildIndex(table, table.attributes[1]);

    Condition delete_cond(sno, Operator::Equal, SqlValue(sno.type, 55));
    rm->DeleteRecord(table, std::vector<Condition>(1, delete_cond));

    Condition cond(sno, Operator::GreaterThan, SqlValue(sno.type, 50));
    Condition cond2(sno, Operator::LessEqual, SqlValue(sno.type, 70));
    Condition cond3(sname, Operator::NotEqual, SqlValue(sname.type, "s60"));
    std::vector<Condition> conds {cond, cond2, cond3};

    auto t = rm->SelectRecord(table, conds, table.attributes[1].name);
    std::cout << t;

    delete rm;
    delete im;
    delete bm;
}

