#pragma once
#include <string>


struct SourceFile;

struct Position {
    int line;
    int column;

    bool operator==(const Position& other) const {
        return line == other.line && column == other.column;
    }
};

struct Location {
    SourceFile* src;
    Position start;
    Position end;

    std::string to_string() const;

    Location(SourceFile& src, int line, int start_col, int end_col)
        : src(&src), start({line, start_col}), end({line, end_col}) {}
};