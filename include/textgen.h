#ifndef TEXTGEN_H
#define TEXTGEN_H

#include <string>
#include <deque>
#include <map>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

const int NPREF = 2;
const int MAXGEN = 1000;
const std::string NOPREFIX = "\n";

typedef std::deque<std::string> prefix;
typedef std::map<prefix, std::vector<std::string>> statetab;

class MarkovGenerator
{
private:
    statetab states;
    int prefixSize;
    int maxGenerated;
    std::mt19937 rng;

public:
    MarkovGenerator(int prefSize = NPREF, int maxGen = MAXGEN, unsigned seed = 42);

    void readInputFile(const std::string &filename);
    std::string generateText();

    // Геттеры
    const statetab &getStates() const { return states; }
    int getPrefixSize() const { return prefixSize; }

    // Методы для тестирования (позволяют заполнять таблицу вручную без const_cast)
    void addTestState(const prefix &p, const std::string &suffix) { states[p].push_back(suffix); }

    static prefix makePrefix(const std::vector<std::string> &words, int size);
    static void addSuffix(statetab &table, const prefix &pref, const std::string &suffix);
    static std::string randomChoice(const std::vector<std::string> &vec, std::mt19937 &rng);
};

#endif // TEXTGEN_H