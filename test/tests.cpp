// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>
#include "textgen.h"
#include <set>
#include <sstream>

// 1. формирование префикса из заданного числа слов
TEST(MarkovChain, CreatePrefixFromWords)
{
    std::vector<std::string> words = {"one", "two", "three", "four"};
    prefix p = MarkovGenerator::makePrefix(words, 2);
    ASSERT_EQ(p.size(), 2);
    EXPECT_EQ(p[0], "one");
    EXPECT_EQ(p[1], "two");
}

// 2. формирование записи "префикс-суффикс"
TEST(MarkovChain, PrefixSuffixRecord)
{
    statetab table;
    prefix pref = {"hello", "world"};
    MarkovGenerator::addSuffix(table, pref, "foo");
    MarkovGenerator::addSuffix(table, pref, "bar");
    ASSERT_EQ(table[pref].size(), 2);
    EXPECT_EQ(table[pref][0], "foo");
    EXPECT_EQ(table[pref][1], "bar");
}

// 3. выбор единственного суффикса из вектора (использование ПСЧ)
TEST(MarkovChain, ChooseSingleSuffix)
{
    std::vector<std::string> suffixes = {"only_choice"};
    std::mt19937 rng(42);
    EXPECT_EQ(MarkovGenerator::randomChoice(suffixes, rng), "only_choice");
}

// 4. выбор суффикса из вектора, содержащего несколько вариантов (ПСЧ)
TEST(MarkovChain, ChooseMultipleSuffixes)
{
    std::vector<std::string> suffixes = {"alpha", "beta", "gamma"};
    std::mt19937 rng(42);
    std::set<std::string> valid(suffixes.begin(), suffixes.end());
    for (int i = 0; i < 20; ++i)
    {
        std::string choice = MarkovGenerator::randomChoice(suffixes, rng);
        EXPECT_TRUE(valid.count(choice) > 0) << "Invalid choice: " << choice;
    }
}

// 5. формирование текста заданной длины (на основе таблицы, заполненной вручную)
// ИСПРАВЛЕНО: используем префиксы, которые будут первыми в лексикографическом порядке
TEST(MarkovChain, GenerateTextFixedLength)
{
    MarkovGenerator gen(2, 10, 42);

    // Используем префиксы, где первый элемент "aa..." будет первым в map
    gen.addTestState({"aa", "bb"}, "cc"); // начальный префикс (лексикографически первый)
    gen.addTestState({"bb", "cc"}, "dd");
    gen.addTestState({"cc", "dd"}, "ee");
    gen.addTestState({"dd", "ee"}, "\n"); // маркер конца

    std::string result = gen.generateText();

    // Подсчёт слов (исключаем маркер конца)
    std::istringstream iss(result);
    std::string word;
    int count = 0;
    while (iss >> word)
    {
        if (word != "\n")
            count++;
    }

    // Ожидаем: начальный префикс (2 слова) + минимум 1 суффикс = 3 слова
    EXPECT_GE(count, 3);  // минимум: префикс + 1 суффикс
    EXPECT_LE(count, 10); // не больше лимита
    EXPECT_FALSE(result.empty());
}

// Дополнительные тесты
TEST(MarkovChain, EmptyTableHandling)
{
    MarkovGenerator gen(2, 50, 42);
    EXPECT_TRUE(gen.generateText().find("Error") != std::string::npos);
}

TEST(MarkovChain, PrefixSizeValidation)
{
    EXPECT_EQ(MarkovGenerator(1).getPrefixSize(), 1);
    EXPECT_EQ(MarkovGenerator(3).getPrefixSize(), 3);
}

TEST(MarkovChain, ReproducibilityWithSameSeed)
{
    MarkovGenerator g1(2, 20, 12345), g2(2, 20, 12345);
    g1.addTestState({"a", "b"}, "c");
    g1.addTestState({"b", "c"}, "\n");
    g2.addTestState({"a", "b"}, "c");
    g2.addTestState({"b", "c"}, "\n");
    EXPECT_EQ(g1.generateText(), g2.generateText());
}

TEST(MarkovChain, DequeEqualityAsMapKey)
{
    prefix p1 = {"x", "y"}, p2 = {"x", "y"};
    EXPECT_EQ(p1, p2);
}

TEST(MarkovChain, UnigramChainModel)
{
    MarkovGenerator gen(1, 15, 42);
    gen.addTestState({"A"}, "B");
    gen.addTestState({"B"}, "C");
    gen.addTestState({"C"}, "\n");
    std::string res = gen.generateText();
    EXPECT_FALSE(res.empty());
    EXPECT_GT(res.length(), 5);
}

TEST(MarkovChain, StrictWordCountLimit)
{
    MarkovGenerator gen(2, 5, 42);
    gen.addTestState({"w1", "w2"}, "w3");
    gen.addTestState({"w2", "w3"}, "w4");
    gen.addTestState({"w3", "w4"}, "w5");
    gen.addTestState({"w4", "w5"}, "w6");
    gen.addTestState({"w5", "w6"}, "\n");
    std::string res = gen.generateText();
    std::istringstream iss(res);
    int cnt = 0;
    std::string w;
    while (iss >> w)
        if (w != "\n")
            cnt++;
    EXPECT_EQ(cnt, 5);
}