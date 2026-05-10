#include "textgen.h"
#include <iostream>

MarkovGenerator::MarkovGenerator(int prefSize, int maxGen, unsigned seed)
    : prefixSize(prefSize), maxGenerated(maxGen), rng(seed) {}

prefix MarkovGenerator::makePrefix(const std::vector<std::string> &words, int size)
{
    prefix p;
    for (size_t i = 0; i < std::min(words.size(), static_cast<size_t>(size)); ++i)
    {
        p.push_back(words[i]);
    }
    return p;
}

void MarkovGenerator::addSuffix(statetab &table, const prefix &pref, const std::string &suffix)
{
    table[pref].push_back(suffix);
}

std::string MarkovGenerator::randomChoice(const std::vector<std::string> &vec, std::mt19937 &rng)
{
    if (vec.empty())
        return "";
    std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
    return vec[dist(rng)];
}

void MarkovGenerator::readInputFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    std::string word;
    std::vector<std::string> allWords;
    while (file >> word)
    {
        if (!word.empty())
            allWords.push_back(word);
    }
    file.close();

    if (allWords.empty())
    {
        std::cerr << "Warning: No valid words found in " << filename << std::endl;
        return;
    }
    std::cout << "✅ Loaded " << allWords.size() << " words from input." << std::endl;

    prefix pref;
    for (const auto &w : allWords)
    {
        if (pref.size() == static_cast<size_t>(prefixSize))
        {
            states[pref].push_back(w);
            pref.pop_front();
        }
        pref.push_back(w);
    }

    for (int i = 0; i < prefixSize; ++i)
    {
        if (pref.size() == static_cast<size_t>(prefixSize))
        {
            states[pref].push_back(NOPREFIX);
            pref.pop_front();
        }
        pref.push_back(NOPREFIX);
    }
}

std::string MarkovGenerator::generateText()
{
    if (states.empty())
        return "Error: State table is empty.";

    prefix pref = states.begin()->first;
    std::ostringstream result;
    for (const auto &w : pref)
    {
        if (w != NOPREFIX)
            result << w << " ";
    }

    int count = static_cast<int>(pref.size());
    while (count < maxGenerated)
    {
        auto it = states.find(pref);

        // Если цепочка оборвалась или встретился маркер конца,
        // переходим на случайный префикс из таблицы
        bool restart = false;
        if (it == states.end())
        {
            restart = true;
        }
        else
        {
            std::string next = randomChoice(it->second, rng);
            if (next == NOPREFIX)
                restart = true;
            else
            {
                result << next << " ";
                count++;
                pref.pop_front();
                pref.push_back(next);
                continue;
            }
        }

        if (restart)
        {
            size_t idx = std::uniform_int_distribution<size_t>(0, states.size() - 1)(rng);
            auto rand_it = states.begin();
            std::advance(rand_it, idx);
            pref = rand_it->first;
        }
    }
    return result.str();
}