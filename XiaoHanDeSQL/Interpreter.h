

#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include "MiniType.h"

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <exception>
#include <utility>
#include <cstring>
#include <regex>

class Interpreter {
public:
    static void MainInteractive();

    static SqlCommand ReadCommand();

    static SqlCommand ParseCommand(std::string command);

    static SqlCommand ParseCreateTableCommand(std::string input);

    static SqlCommand ParseCreateIndexCommand(std::string input);
};


#endif //MINISQL_INTERPRETER_H
