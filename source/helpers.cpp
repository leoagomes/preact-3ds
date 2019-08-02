#include "helpers.hpp"

#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <streambuf>

long file_length(FILE* file);

std::string helpers::read_file(const char* filename) {
    std::ifstream file(filename, std::ios::in);
    std::stringstream string_stream;
    string_stream << file.rdbuf();
    return string_stream.str();
}
