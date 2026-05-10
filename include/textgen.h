// Copyright 2024 Your Name
#ifndef INCLUDE_TEXTGEN_H_
#define INCLUDE_TEXTGEN_H_

#include <deque>
#include <map>
#include <random>
#include <string>
#include <vector>

const int NPREF = 2;
const int MAXGEN = 1000;
const char NOPREFIX[] = "\n";

typedef std::deque<std::string> prefix;
typedef std::map<prefix, std::vector<std::string>> statetab;

class MarkovGenerator
{
private:
    statetab states_;
    int prefix_size_;
    int max_generated_;
    std::mt19937 rng_;

public:
    explicit MarkovGenerator(int pref_size = NPREF,
                             int max_gen = MAXGEN,
                             unsigned seed = 42);

    void ReadInputFile(const std::string &filename);
    std::string GenerateText();

    const statetab &GetStates() const { return states_; }
    int GetPrefixSize() const { return prefix_size_; }

    void AddTestState(const prefix &p, const std::string &suffix)
    {
        states_[p].push_back(suffix);
    }

    static prefix MakePrefix(const std::vector<std::string> &words, int size);
    static void AddSuffix(statetab &table,
                          const prefix &pref,
                          const std::string &suffix);
    static std::string RandomChoice(const std::vector<std::string> &vec,
                                    std::mt19937 &rng);
};

#endif // INCLUDE_TEXTGEN_H_