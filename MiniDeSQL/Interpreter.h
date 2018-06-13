#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include "MiniType.h"

class Interpreter {
public:
    static MINI_TYPE::SqlCommand Parse(std::string input);
};


#endif //MINISQL_INTERPRETER_H
