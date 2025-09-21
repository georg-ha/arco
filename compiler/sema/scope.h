#pragma once
#include <string>
#include <unordered_map>

#include "symbol.h"


struct Location;
struct DefArg;

struct Scope {
    enum class Kind {Block, Function, Module} kind;
    Scope* parent_scope;
    std::unordered_map<std::string, Symbol> symbols;

    explicit Scope(Kind kind, Scope* parent_scope = nullptr);

    std::optional<Symbol> resolve(const std::string& name) const;

    // Should only be used after name resolution
    Symbol& get_symbol(const std::string& name);

    void add_function(Location& loc,  FunSignature& sig);

    void add_module(Module& module);

    void add_var(Location& loc, VarInit& var);

    void add_param(DefArg& arg);

    bool is_function(const std::string& name);

    bool can_be_reassigned(const std::string& id);
};
