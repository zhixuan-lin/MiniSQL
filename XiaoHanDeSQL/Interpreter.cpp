#include "Interpreter.h"

void Interpreter::MainInteractive() {
    while (true) {
        try {
            ReadCommand();
        }
        catch (SyntaxError &err) {
            std::cerr << err.what() << std::endl;
            break;
        };
    }
}

SqlCommand Interpreter::ReadCommand() {
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

    lines.erase(remove_if(lines.begin(), lines.end(), ::isspace), lines.end());

    return ParseCommand(lines);
}

// the input should be a complete string without ';'
SqlCommand Interpreter::ParseCommand(std::string input) {
    using namespace std;
}

SqlCommand Interpreter::ParseCreateTableCommand(std::string input) {

}