#include <iostream>
#include <unordered_map>
#include <sstream>
#include <cctype>
using namespace std;

unordered_map<string, int> variables;

string preprocess(const string &line) {
    string result;
    for (char ch : line) {
        if (ch == '=' || ch == '+' || ch == '-') {
            result += ' ';
            result += ch;
            result += ' ';
        } else {
            result += ch;
        }
    }
    return result;
}

void processStatement(string statement) {
    stringstream ss(statement);
    string var, op;
    ss >> var >> op;

    if (op != "=") {
        cout << "Error: Invalid syntax in statement: " << statement << "\n";
        return;
    }

    int result = 0;
    char operation = '+';
    string token;

    while (ss >> token) {
        int num = 0;

        if (isdigit(token[0]) || (token[0] == '-' && token.size() > 1)) {
            num = stoi(token);
        } else if (variables.find(token) != variables.end()) {
            num = variables[token];
        } else {
            cout << "Error: Undefined variable '" << token << "'\n";
            return;
        }

        if (operation == '+') result += num;
        else if (operation == '-') result -= num;

        if (!(ss >> operation)) break;
    }

    variables[var] = result;
}

void processLine(const string &line) {
    string preprocessed = preprocess(line);
    stringstream ss(preprocessed);
    string statement;
    vector<string> statements;

    while (getline(ss, statement, ';')) {
        if (!statement.empty()) statements.push_back(statement);
    }

    for (const string &stmt : statements) {
        processStatement(stmt);
    }
}

int main() {
    string line;
    cout << "Enter SimpleLang code (empty line to stop):\n";
    while (getline(cin, line) && !line.empty()) {
        processLine(line);
    }

    cout << "\nStored Variables:\n";
    for (const auto &pair : variables) {
        cout << pair.first << " = " << pair.second << endl;
    }

    return 0;
}
