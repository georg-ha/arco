#include "format_specifier.h"

#include "expr_nodes.h"
#include "token_error.h"
#include "type_checker.h"
#include "type_error.h"

Location shift_col(const Location& loc,size_t i) {
    return {*loc.src, loc.start.line, loc.start.column + static_cast<int>(i), loc.end.column};
}

bool is_format_char(char c) {
    switch (c) {
        case 'd':
        case 'i':
        case 'f':
        case 's':
        case 'c':
            return true;
        default: return false;
    }
}

FormatKind specifier_of_char(char c) {
    using enum FormatKind;
    FormatKind kind = Integer;
    switch (c) {
        case 'f': kind = Float; break;
        case 's': kind = String; break;
        case 'c': kind = Char; break;
        case 'd':
        case 'i': kind = Integer; break;
        default: break;
    }
    return {kind};
}


std::vector<FormatSpecifier> get_format_specifiers(const StringConst& string) {
    std::vector<FormatSpecifier> specifiers;
    const auto str_size = string.val.size();
    for (size_t i = 0; i < str_size; i++) {
        if (string.val[i] == '%') {
            if (i == str_size - 1) {
                throw TokenError("'%' expects a format specifier",
                    shift_col(string.loc, static_cast<int>(i)));
            }
            const char c = string.val[i+1];
            if (is_format_char(c)) {
                specifiers.push_back({specifier_of_char(c), shift_col(string.loc, i)});
            } else {
                if (c != '%') {
                    auto loc = shift_col(string.loc, i);
                    throw TypeError(loc, "Unknown format specifier");
                }
            }
        }

    }
    return specifiers;
}

bool expr_matches_format(const TypeChecker& ty, const Expr &expr, const FormatSpecifier &format) {
    using enum FormatKind;
    switch (format.kind) {
        case Integer: return expr.type == ty.int_ty();
        case Float: return expr.type == ty.float_ty();
        case Char: return expr.type == ty.char_ty();
        case String: return expr.type == ty.string_ty();
    }
    return false;
}
