#include <fstream>
#include "source_file.h"

SourceFile::SourceFile(const std::string &filename)
    : filename(filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Couldn't open file: " + filename);
    }
    std::string line;
    while (std::getline(in, line)) {
        lines.push_back(line + "\n");
    }
    if (lines.empty()) {
        lines.emplace_back("\n");
    }
}

SourceFile::SourceFile(const std::vector<std::string> &src)
    : filename("testing") {
    for (const auto& line: src) {
        lines.push_back(line + "\n");
    }
}

std::string& SourceFile::get_line(const int n) {
    return lines.at(n-1);
}

int SourceFile::length() const {
    return static_cast<int>(lines.size());
}