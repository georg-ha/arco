#pragma once

#include "location.h"

struct Visitor;
struct Scope;
struct Type;

struct TypeAnno {
    Location loc;
    Scope* scope = nullptr;
    Type* type = nullptr;

    enum class Kind {Int, Float, Char, String, Bool, Unit} kind;

    TypeAnno(const Location& loc, Kind kind)
        : loc(loc), kind(kind) {}

    void accept(Visitor& visitor);
};
