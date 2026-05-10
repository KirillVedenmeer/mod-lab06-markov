#include "textgen.h"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    std::string inputFile = "input/source.txt";
    std::string outputFile = "result/gen.txt";
    int prefSize = 2;
    int maxWords = 1000;
    unsigned seed = 42;

    if (argc > 1)
        inputFile = argv[1];
    if (argc > 2)
        outputFile = argv[2];
    if (argc > 3)
        prefSize = std::stoi(argv[3]);
    if (argc > 4)
        maxWords = std::stoi(argv[4]);
    if (argc > 5)
        seed = std::stoi(argv[5]);

    MarkovGenerator gen(prefSize, maxWords, seed);
    gen.readInputFile(inputFile);
    std::string text = gen.generateText();

    std::ofstream out(outputFile);
    if (out.is_open())
    {
        out << text;
        out.close();
        std::cout << "Generated text saved to " << outputFile << std::endl;
    }
    else
    {
        std::cerr << "Error: Cannot write to " << outputFile << std::endl;
        return 1;
    }
    return 0;
}