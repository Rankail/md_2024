#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    std::cout << "argc: " << argc << std::endl;
    std::vector<std::string> args{};
    for (int i = 0; i < argc; i++) {
        std::cout << argv[i] << std::endl;
        args.emplace_back(argv[i]);
    }

    if (argc != 3) {
        std::cerr << "Usage: pointCalc.exe [inputFile] [outputFile]" << std::endl;
        return -1;
    }





    return 0;
}