// Copyright 2021 GHA Test Team
#include <gtest/gtest.h>

#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "textgen.h"

TEST(MarkovChain, CreatePrefixFromWords) {
  std::vector<std::string> words = {"one", "two", "three", "four"};
  prefix p = MarkovGenerator::MakePrefix(words, 2);
  ASSERT_EQ(p.size(), 2);
  EXPECT_EQ(p[0], "one");
  EXPECT_EQ(p[1], "two");
}

TEST(MarkovChain, PrefixSuffixRecord) {
  statetab table;
  prefix pref = {"hello", "world"};
  MarkovGenerator::AddSuffix(table, pref, "foo");
  MarkovGenerator::AddSuffix(table, pref, "bar");
  ASSERT_EQ(table[pref].size(), 2);
  EXPECT_EQ(table[pref][0], "foo");
  EXPECT_EQ(table[pref][1], "bar");
}

TEST(MarkovChain, ChooseSingleSuffix) {
  std::vector<std::string> suffixes = {"only_choice"};
  std::mt19937 rng(42);
  EXPECT_EQ(MarkovGenerator::RandomChoice(suffixes, rng), "only_choice");
}

TEST(MarkovChain, ChooseMultipleSuffixes) {
  std::vector<std::string> suffixes = {"alpha", "beta", "gamma"};
  std::mt19937 rng(42);
  std::set<std::string> valid(suffixes.begin(), suffixes.end());
  for (int i = 0; i < 20; ++i) {
    std::string choice = MarkovGenerator::RandomChoice(suffixes, rng);
    EXPECT_TRUE(valid.count(choice) > 0) << "Invalid choice: " << choice;
  }
}

TEST(MarkovChain, GenerateTextFixedLength) {
  MarkovGenerator gen(2, 10, 42);
  gen.AddTestState({"aa", "bb"}, "cc");
  gen.AddTestState({"bb", "cc"}, "dd");
  gen.AddTestState({"cc", "dd"}, "ee");
  gen.AddTestState({"dd", "ee"}, NOPREFIX);

  std::string result = gen.GenerateText();
  std::istringstream iss(result);
  std::string word;
  int count = 0;
  while (iss >> word) {
    if (word != NOPREFIX)
      ++count;
  }
  EXPECT_GE(count, 3);
  EXPECT_LE(count, 10);
  EXPECT_FALSE(result.empty());
}

TEST(MarkovChain, EmptyTableHandling) {
  MarkovGenerator gen(2, 50, 42);
  EXPECT_TRUE(gen.GenerateText().find("Error") != std::string::npos);
}

TEST(MarkovChain, PrefixSizeValidation) {
  EXPECT_EQ(MarkovGenerator(1).GetPrefixSize(), 1);
  EXPECT_EQ(MarkovGenerator(3).GetPrefixSize(), 3);
}

TEST(MarkovChain, ReproducibilityWithSameSeed) {
  MarkovGenerator g1(2, 20, 12345), g2(2, 20, 12345);
  g1.AddTestState({"a", "b"}, "c");
  g1.AddTestState({"b", "c"}, NOPREFIX);
  g2.AddTestState({"a", "b"}, "c");
  g2.AddTestState({"b", "c"}, NOPREFIX);
  EXPECT_EQ(g1.GenerateText(), g2.GenerateText());
}

TEST(MarkovChain, DequeEqualityAsMapKey) {
  prefix p1 = {"x", "y"}, p2 = {"x", "y"};
  EXPECT_EQ(p1, p2);
}

TEST(MarkovChain, UnigramChainModel) {
  MarkovGenerator gen(1, 15, 42);
  gen.AddTestState({"A"}, "B");
  gen.AddTestState({"B"}, "C");
  gen.AddTestState({"C"}, NOPREFIX);
  std::string res = gen.GenerateText();
  EXPECT_FALSE(res.empty());
  EXPECT_GT(res.length(), 5);
}

TEST(MarkovChain, StrictWordCountLimit) {
  MarkovGenerator gen(2, 5, 42);
  gen.AddTestState({"w1", "w2"}, "w3");
  gen.AddTestState({"w2", "w3"}, "w4");
  gen.AddTestState({"w3", "w4"}, "w5");
  gen.AddTestState({"w4", "w5"}, "w6");
  gen.AddTestState({"w5", "w6"}, NOPREFIX);
  std::string res = gen.GenerateText();
  std::istringstream iss(res);
  int cnt = 0;
  std::string w;
  while (iss >> w) {
    if (w != NOPREFIX)
      ++cnt;
  }
  EXPECT_EQ(cnt, 5);
}
