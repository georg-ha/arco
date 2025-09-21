#include "location.h"
#include <string>

#include "source_file.h"

std::string Location::to_string() const {
    return src->filename + ":: " + std::to_string(start.line) + ":" + std::to_string(start.column)
    + " - " + std::to_string(end.line) + ":" +  std::to_string(end.column);
}