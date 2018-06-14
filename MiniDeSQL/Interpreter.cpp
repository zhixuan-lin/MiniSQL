#include "Interpreter.h"

void Interpreter::MainInteractive() {
    while (true) {
        try {
            ReadCommand();
        }
        catch (MINI_TYPE::SyntaxError &err) {
            std::cerr << err.what() << std::endl;
            break;
        };
    }
}

MINI_TYPE::SqlCommand Interpreter::ReadCommand() {
    using namespace std;

    cout << "MiniSql> ";

    string lines;

    while (true) {
        string line;
        getline(cin, line);

        if (line[line.size() - 1] == ';') {
            lines += line.substr(0, line.size() - 1);
            break;
        } else {
            cout << "       > ";
            lines += line;
        }

    }

    return Parse(lines);
}

MINI_TYPE::SqlCommand Interpreter::Parse(std::string input) {
    using namespace std;

    auto token = strtok(const_cast<char *>(input.c_str()), ",;()\n\' ‘’");

    vector<string> tokens;

    while (token) {
        if (strcmp(token, "") != 0) {
            tokens.emplace_back(token);
            token = strtok(nullptr, ",;()\n\' ‘’");
        }
    }

    if (tokens[0] == "create") {
        if (tokens[1] == "table")
            return ParseCreateTable(tokens);
        if (tokens[1] == "index")
            return ParseCreateIndex(tokens);
        throw MINI_TYPE::SyntaxError("Invalid create command.");
    }

    if (tokens[0] == "drop") {
        if (tokens[1] == "table")
            return ParseDropTable(tokens);
        if (tokens[1] == "index")
            return ParseDropIndex(tokens);
        throw MINI_TYPE::SyntaxError("Invalid drop command.");
    }

    if (tokens[0] == "select")
        return ParseSelect(tokens);

    if (tokens[0] == "insert")
        return ParseInsert(tokens);

    if (tokens[0] == "delete")
        return ParseDelete(tokens);
}

MINI_TYPE::SqlCommand Interpreter::ParseCreateTable(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() <= 5)
        throw SyntaxError("Too few arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = CreateTableCmd;
    sqlCommand.tableInfo.name = tokens[2];

    for (int i = 3; tokens.size() > i;) {
        if (tokens[i] == "primary" && tokens[i + 1] == "key") {
            sqlCommand.tableInfo.primaryKey = tokens[i + 2];
            i += 3;
            continue;
        }

        Attribute attribute;
        attribute.name = tokens[i];

        // should use map
        if (tokens[i + 1] == "int") {
            attribute.type.type = MiniInt;
            i = i + 2;
        } else if (tokens[i + 1] == "float") {
            attribute.type.type = MiniFloat;
            i = i + 2;
        } else if (tokens[i + 1] == "char") {
            attribute.type.type = MiniChar;

            try {
                attribute.type.char_size = static_cast<size_t>(std::stoi(tokens[i + 2]));
            } catch (...) {
                throw SyntaxError("Invalid char size.");
            }

            i = i + 3;
        }

        if (tokens[i] == "unique") {
            attribute.unique = true;
            i++;
        }

        sqlCommand.tableInfo.attributes.push_back(attribute);
    }

    return sqlCommand;
}

MINI_TYPE::SqlCommand Interpreter::ParseCreateIndex(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() <= 5)
        throw SyntaxError("Too few arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = CreateIndexCmd;
    sqlCommand.indexInfo.alias = tokens[2];
    sqlCommand.indexInfo.table = tokens[4];
    sqlCommand.indexInfo.attribute = tokens[5];

    return sqlCommand;
}

MINI_TYPE::SqlCommand Interpreter::ParseDropTable(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() != 3)
        throw SyntaxError("Invalid number of arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = DropTableCmd;
    sqlCommand.tableName = tokens[2];

    return sqlCommand;
}

MINI_TYPE::SqlCommand Interpreter::ParseDropIndex(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() != 3)
        throw SyntaxError("Invalid number of arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = DropIndexCmd;
    sqlCommand.indexName = tokens[2];

    return sqlCommand;
}

MINI_TYPE::SqlCommand Interpreter::ParseSelect(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() <= 3)
        throw SyntaxError("Too few arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = SelectCmd;

    int pointer = 1;
    if (tokens[1] != "*") {
        for (; tokens.size() > pointer; pointer++) {
            sqlCommand.attrList.push_back(tokens[pointer]);
            if (tokens[pointer + 1] == "from")
                break;
        }
    }

    pointer += 2; // points to tablename

    sqlCommand.tableName = tokens[pointer];

    if (pointer + 1 < tokens.size() && tokens[pointer + 1] == "where") {
        pointer += 2;   // points to the 1st attr
        while (pointer < tokens.size()) {
            Condition cond;
            cond.attributeName = tokens[pointer];
            cond.op = GetOpFromString(tokens[pointer + 1]);
            SqlValue sqlValue;

            try {
                sqlValue.i = std::stoi(tokens[pointer + 2]);
            } catch (...) {}

            try {
                sqlValue.f = std::stof(tokens[pointer + 2]);
            } catch (...) {}

            sqlValue.str = tokens[pointer + 2];

            cond.value = sqlValue;
            sqlCommand.condArray.push_back(cond);
            pointer += 4;
        }
    }

    return sqlCommand;
}

MINI_TYPE::SqlCommand Interpreter::ParseInsert(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() <= 4)
        throw SyntaxError("Invalid number of arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = InsertCmd;
    sqlCommand.tableName = tokens[2];

    for (int pointer = 4; tokens.size() > pointer; pointer++) {
        SqlValue sqlValue;

        try {
            sqlValue.i = std::stoi(tokens[pointer]);
        } catch (...) {}

        try {
            sqlValue.f = std::stof(tokens[pointer]);
        } catch (...) {}

        sqlValue.str = tokens[pointer];

        sqlCommand.valueArray.push_back(sqlValue);
    }

    return sqlCommand;
}

MINI_TYPE::SqlCommand Interpreter::ParseDelete(std::vector<std::string> tokens) {
    using namespace MINI_TYPE;

    if (tokens.size() < 3)
        throw SyntaxError("Too few arguments.");

    SqlCommand sqlCommand;
    sqlCommand.commandType = DeleteCmd;
    sqlCommand.tableName = tokens[2];

    auto pointer = 2;

    if (pointer + 1 < tokens.size() && tokens[pointer + 1] == "where") {
        pointer += 2;
        while (pointer < tokens.size()) {
            Condition cond;
            cond.attributeName = tokens[pointer];
            cond.op = GetOpFromString(tokens[pointer + 1]);
            SqlValue sqlValue;

            try {
                sqlValue.i = std::stoi(tokens[pointer + 2]);
            } catch (...) {}

            try {
                sqlValue.f = std::stof(tokens[pointer + 2]);
            } catch (...) {}

            sqlValue.str = tokens[pointer + 2];

            cond.value = sqlValue;
            sqlCommand.condArray.push_back(cond);
            pointer += 4;
        }
    }

    return sqlCommand;
}