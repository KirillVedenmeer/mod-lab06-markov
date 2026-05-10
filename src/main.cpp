// Copyright 2024 Your Name
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "textgen.h"

int main(int argc, char* argv[]) {
  std::string input_file = "input/source.txt";
  std::string output_file = "result/gen.txt";
  int pref_size = 2;
  int max_words = 1000;
  unsigned seed = 42;

  if (argc > 1)
    input_file = argv[1];
  if (argc > 2)
    output_file = argv[2];
  if (argc > 3)
    pref_size = std::stoi(argv[3]);
  if (argc > 4)
    max_words = std::stoi(argv[4]);
  if (argc > 5)
    seed = std::stoi(argv[5]);

  MarkovGenerator gen(pref_size, max_words, seed);
  gen.ReadInputFile(input_file);
  std::string text = gen.GenerateText();

  std::ofstream out(output_file);
  if (out.is_open()) {
    out << text;
    out.close();
    std::cout << "Generated text saved to " << output_file << std::endl;
  } else {
    std::cerr << "Error: Cannot write to " << output_file << std::endl;
    return 1;
  }
  return 0;
}
