#ifndef MINISQL_TEST_H
#define MINISQL_TEST_H

#include "Interpreter.h"
#include "CatalogManager.h"

#include <cassert>

class Test {
public:
    static void InterpreterTest();

    static void CatalogTest();
};


#endif //MINISQL_TEST_H
