#pragma once
#include <string>

#include "location.h"
#include "source_file.h"

inline std::string token_message(const Location& loc, const std::string& message) {
    const std::string& filename = loc.src->filename;
    const std::string prefix = filename + ":" + std::to_string(loc.start.line) + ":" +
                               std::to_string(loc.start.column) + ": " + message + "\n";
    const std::string codeLine = " " + std::to_string(loc.start.line) + " | " + loc.src->get_line(loc.start.line);
    const std::string pointer  = std::string(std::to_string(loc.start.line).length(), ' ')
                                + "  | " + std::string(loc.start.column -1, ' ')
                                + std::string(loc.end.column - loc.start.column, '^')  + "\n";
    return prefix + codeLine + pointer;
}
