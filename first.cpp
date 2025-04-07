#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <string>

using namespace std;

map<string, vector<vector<string>>> grammar;
map<string, set<string>> firstSet;
set<string> nonTerminals;
set<string> terminals;
const string EPSILON = "ε";

void parseGrammar(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string lhs, arrow, token;
        iss >> lhs >> arrow;

        if (arrow != "->") {
            cerr << "Error de sintaxis: se esperaba '->' en: " << line << endl;
            continue;
        }

        nonTerminals.insert(lhs);
        vector<string> production;
        vector<vector<string>> rules;

        while (iss >> token) {
            if (token == "|") {
                if (!production.empty()) rules.push_back(production);
                production.clear();
            } else {
                production.push_back(token);
            }
        }

        if (!production.empty()) rules.push_back(production);
        grammar[lhs].insert(grammar[lhs].end(), rules.begin(), rules.end());
    }

    // Detectar terminales automáticamente
    set<string> allSymbols;
    for (const auto& [lhs, rules] : grammar) {
        for (const auto& rule : rules) {
            for (const string& sym : rule) {
                allSymbols.insert(sym);
            }
        }
    }

    for (const string& sym : allSymbols) {
        if (!nonTerminals.count(sym) && sym != EPSILON)
            terminals.insert(sym);
    }
}

set<string> computeFirst(const string& symbol) {
    if (firstSet.count(symbol)) return firstSet[symbol];

    set<string> result;

    if (terminals.count(symbol) || symbol == EPSILON) {
        result.insert(symbol);
        return result;
    }

    if (!grammar.count(symbol)) {
        cerr << "Símbolo no definido en la gramática: " << symbol << endl;
        return result;
    }

    for (const auto& production : grammar[symbol]) {
        bool addEpsilon = true;

        for (const string& sym : production) {
            if (sym == symbol) break;  // prevenir recursión infinita directa
            set<string> temp = computeFirst(sym);

            for (const string& t : temp) {
                if (t != EPSILON) result.insert(t);
            }

            if (!temp.count(EPSILON)) {
                addEpsilon = false;
                break;
            }
        }

        if (addEpsilon) result.insert(EPSILON);
    }

    firstSet[symbol] = result;
    return result;
}

void printFirstSets() {
    cout << "\n== FIRST Sets ==\n";
    for (const string& nt : nonTerminals) {
        const auto& firsts = computeFirst(nt);
        cout << "FIRST(" << nt << ") = { ";
        for (const string& f : firsts) cout << f << " ";
        cout << "}\n";
    }
}

int main() {
    parseGrammar("gramatica.txt");

    for (const string& nt : nonTerminals) {
        computeFirst(nt);
    }

    printFirstSets();
    return 0;
}
