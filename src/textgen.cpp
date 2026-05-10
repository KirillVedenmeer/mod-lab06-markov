// Copyright 2024 Your Name
#include "textgen.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

MarkovGenerator::MarkovGenerator(int pref_size, int max_gen, unsigned seed)
    : prefix_size_(pref_size), max_generated_(max_gen), rng_(seed) {}

prefix MarkovGenerator::MakePrefix(const std::vector<std::string>& words,
                                   int size) {
  prefix p;
  for (size_t i = 0; i < std::min(words.size(), static_cast<size_t>(size));
       ++i) {
    p.push_back(words[i]);
  }
  return p;
}

// NOLINTNEXTLINE(runtime/references)
void MarkovGenerator::AddSuffix(statetab& table, const prefix& pref,
                                const std::string& suffix) {
  table[pref].push_back(suffix);
}

// NOLINTNEXTLINE(runtime/references)
std::string MarkovGenerator::RandomChoice(const std::vector<std::string>& vec,
                                          std::mt19937& rng) {
  if (vec.empty())
    return "";
  std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
  return vec[dist(rng)];
}

void MarkovGenerator::ReadInputFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error: Cannot open file " << filename << std::endl;
    return;
  }

  std::string word;
  std::vector<std::string> all_words;
  while (file >> word) {
    if (!word.empty()) {
      all_words.push_back(word);
    }
  }
  file.close();

  if (all_words.empty()) {
    std::cerr << "Warning: No valid words found in " << filename << std::endl;
    return;
  }
  std::cout << "Loaded " << all_words.size() << " words from input."
            << std::endl;

  prefix pref;
  for (const auto& w : all_words) {
    if (pref.size() == static_cast<size_t>(prefix_size_)) {
      states_[pref].push_back(w);
      pref.pop_front();
    }
    pref.push_back(w);
  }

  for (int i = 0; i < prefix_size_; ++i) {
    if (pref.size() == static_cast<size_t>(prefix_size_)) {
      states_[pref].push_back(NOPREFIX);
      pref.pop_front();
    }
    pref.push_back(NOPREFIX);
  }
}

std::string MarkovGenerator::GenerateText() {
  if (states_.empty())
    return "Error: State table is empty.";

  prefix pref = states_.begin()->first;
  std::ostringstream result;
  for (const auto& w : pref) {
    if (w != NOPREFIX)
      result << w << " ";
  }

  int count = static_cast<int>(pref.size());
  while (count < max_generated_) {
    auto it = states_.find(pref);

    bool restart = false;
    if (it == states_.end()) {
      restart = true;
    } else {
      // ✅ Теперь передаём rng_ по ссылке (как и объявлено)
      std::string next = RandomChoice(it->second, rng_);
      if (next == NOPREFIX) {
        restart = true;
      } else {
        result << next << " ";
        ++count;
        pref.pop_front();
        pref.push_back(next);
        continue;
      }
    }

    if (restart) {
      size_t idx =
        std::uniform_int_distribution<size_t>(0, states_.size() - 1)(rng_);
      auto rand_it = states_.begin();
      // ✅ Исправлено: std::ptrdiff_t вместо long
      std::advance(rand_it, static_cast<std::ptrdiff_t>(idx));
      pref = rand_it->first;
    }
  }
  return result.str();
}