#pragma once
#include <vector>
#include "location.h"
#include "expr.h"


struct TypeChecker;

enum class FormatKind {
    Integer,
    Float,
    String,
    Char,
};

// TODO add options
struct FormatSpecifier {
    FormatKind kind;
    Location loc;
};


std::vector<FormatSpecifier> get_format_specifiers(const StringConst& string);

// Uses the type* field
bool expr_matches_format(const TypeChecker& ty, const Expr& expr, const FormatSpecifier &format);