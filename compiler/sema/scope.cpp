#include "scope.h"

#include "double_definition_error.h"
#include "module.h"
#include "stmt_nodes.h"

Scope::Scope(Kind kind, Scope* parent_scope)
    : kind(kind), parent_scope(parent_scope) {}

std::optional<Symbol> Scope::resolve(const std::string &name) const {
    auto tmp = this;
    while (tmp) {
        if (tmp->symbols.contains(name)) {
            return tmp->symbols.at(name);
        }
        tmp = tmp->parent_scope;
    }
    return {};
}

Symbol& Scope::get_symbol(const std::string &name) {
    auto tmp = this;
    while (tmp) {
        if (tmp->symbols.contains(name)) {
            return tmp->symbols.at(name);
        }
        tmp = tmp->parent_scope;
    }
    throw std::runtime_error("get_symbol should only be used after name resolution!");
}

void Scope::add_function(Location& loc, FunSignature &sig) {
    if (symbols.contains(sig.id)) {
        throw DoubleDefinitionError(sig.id, loc);
    }
    symbols.emplace(sig.id, FunSymbol(sig));
}

void Scope::add_module(Module &module) {
    symbols.emplace(module.name, ModuleSymbol(module));
}

void Scope::add_var(Location& loc, VarInit &var) {
    if (symbols.contains(var.id)) {
        throw DoubleDefinitionError(var.id, loc);
    }
    symbols.emplace(var.id, VarSymbol(var.is_const, var));
}

void Scope::add_param(DefArg &arg) {
    if (symbols.contains(arg.id)) {
        throw DoubleDefinitionError(arg.id, arg.loc);
    }
    symbols.emplace(arg.id, ParamSymbol(arg));
}

bool Scope::is_function(const std::string &name){
    return std::holds_alternative<FunSymbol>(get_symbol(name).kind);
}

bool Scope::can_be_reassigned(const std::string &id) {
    if (std::holds_alternative<VarSymbol>(get_symbol(id).kind)) {
        return !std::get<VarSymbol>(get_symbol(id).kind).is_const;
    }
    return std::holds_alternative<ParamSymbol>(get_symbol(id).kind);
}


