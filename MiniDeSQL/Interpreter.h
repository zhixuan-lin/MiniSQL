#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include "MiniType.h"

#include <cstring>

class Interpreter {
public:
    static void MainInteractive();

    static MINI_TYPE::SqlCommand ReadCommand();
    static MINI_TYPE::SqlCommand Parse(std::string input);

    static MINI_TYPE::SqlCommand ParseCreateTable(std::vector<std::string> tokens);

    static MINI_TYPE::SqlCommand ParseCreateIndex(std::vector<std::string> tokens);

    static MINI_TYPE::SqlCommand ParseDropTable(std::vector<std::string> tokens);

    static MINI_TYPE::SqlCommand ParseDropIndex(std::vector<std::string> tokens);

    static MINI_TYPE::SqlCommand ParseSelect(std::vector<std::string> tokens);

    static MINI_TYPE::SqlCommand ParseInsert(std::vector<std::string> tokens);

    static MINI_TYPE::SqlCommand ParseDelete(std::vector<std::string> tokens);
};


#endif //MINISQL_INTERPRETER_H
