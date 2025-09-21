#pragma once
#include <vector>
#include <string>

struct SourceFile {
    std::string filename;

    explicit SourceFile(const std::string& filename);

    // For testing only
    explicit SourceFile(const std::vector<std::string> &src);

    // 1-based indexing
    std::string& get_line(int);

    int length() const;

private:
    std::vector<std::string> lines;
};
