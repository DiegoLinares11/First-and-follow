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
}

set<string> computeFirst(const string& symbol) {

}

void printFirstSets() {

}

int main() {
    parseGrammar("gramatica.txt");

    for (const string& nt : nonTerminals) {
        computeFirst(nt);
    }

    printFirstSets();
    return 0;
}
