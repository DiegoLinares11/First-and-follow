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
map<string, set<string>> followSet;
set<string> nonTerminals;
set<string> terminals;
const string EPSILON = "ε";
const string END_MARKER = "$";

string startSymbol;

void parseGrammar(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        exit(1);
    }
    
    //bool isFirstLine = true;   // Esto que esta comenado aca es para poder leer desde el archivo el simbolo inicial. 
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string lhs, arrow, token;
        iss >> lhs >> arrow;

        /*
        if (isFirstLine) {
            startSymbol = lhs; 
            isFirstLine = false;
        }*/
        
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
            if (sym == symbol) break;
            set<string> temp = computeFirst(sym);

            for (const string& t : temp)
                if (t != EPSILON)
                    result.insert(t);

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

set<string> firstOfSequence(const vector<string>& sequence, size_t start = 0) {
    set<string> result;
    bool addEpsilon = true;

    for (size_t i = start; i < sequence.size(); ++i) {
        set<string> temp = computeFirst(sequence[i]);
        for (const string& t : temp)
            if (t != EPSILON)
                result.insert(t);

        if (!temp.count(EPSILON)) {
            addEpsilon = false;
            break;
        }
    }

    if (addEpsilon) result.insert(EPSILON);
    return result;
}

void computeFollow(const string& startSymbol) {
    followSet[startSymbol].insert(END_MARKER); // Regla 1

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& [lhs, productions] : grammar) {
            for (const auto& prod : productions) {
                for (size_t i = 0; i < prod.size(); ++i) {
                    const string& B = prod[i];
                    if (!nonTerminals.count(B)) continue;

                    set<string> trailer;

                    if (i + 1 < prod.size()) {
                        auto firstBeta = firstOfSequence(prod, i + 1);
                        size_t oldSize = followSet[B].size();
                        for (const string& f : firstBeta)
                            if (f != EPSILON)
                                followSet[B].insert(f);

                        if (firstBeta.count(EPSILON)) {
                            for (const string& f : followSet[lhs])
                                followSet[B].insert(f);
                        }

                        if (followSet[B].size() > oldSize)
                            changed = true;
                    } else {
                        size_t oldSize = followSet[B].size();
                        for (const string& f : followSet[lhs])
                            followSet[B].insert(f);
                        if (followSet[B].size() > oldSize)
                            changed = true;
                    }
                }
            }
        }
    }
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

void printFollowSets() {
    cout << "\n== FOLLOW Sets ==\n";
    for (const string& nt : nonTerminals) {
        cout << "FOLLOW(" << nt << ") = { ";
        for (const string& f : followSet[nt]) cout << f << " ";
        cout << "}\n";
    }
}

void exportFirstFollowSets(const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "No se pudo abrir el archivo de salida: " << filename << endl;
        return;
    }

    out << "== FIRST Sets ==\n";
    for (const string& nt : nonTerminals) {
        const auto& firsts = firstSet[nt];
        out << "FIRST(" << nt << ") = { ";
        for (const string& f : firsts)
            out << f << " ";
        out << "}\n";
    }

    out << "\n== FOLLOW Sets ==\n";
    for (const string& nt : nonTerminals) {
        const auto& follows = followSet[nt];
        out << "FOLLOW(" << nt << ") = { ";
        for (const string& f : follows)
            out << f << " ";
        out << "}\n";
    }

    out.close();
    cout << "\nConjuntos FIRST y FOLLOW exportados a '" << filename << "'\n";
}



int main() {
    parseGrammar("gramatica.txt");

    for (const string& nt : nonTerminals)
        computeFirst(nt);

    //aca no se si dejarlo asi o deberia leerlo del archivo, porque el simbolo inicial siempre es el primero va???
    // O puede ir cambiando? si siempre es el primero pues si se puede leer del archivo
    string startSymbol = "S"; 
    computeFollow(startSymbol);

    printFirstSets();
    printFollowSets();
    exportFirstFollowSets("first_follow_output.txt");
    
    return 0;
}
