#pragma once
#include <variant>
#include <string>


struct DefArg;
struct VarInit;
struct FunSignature;
struct Module;
struct Type;

namespace llvm {
class AllocaInst;
}

struct VarSymbol {
    bool is_const;
    VarInit& def;
};

struct FunSymbol {
    FunSignature& signature;
};

struct ModuleSymbol {
    Module& module;
};

struct ParamSymbol {
    DefArg& def_arg;
};

using SymbolKind = std::variant<VarSymbol, FunSymbol, ModuleSymbol, ParamSymbol>;

struct Symbol {
    SymbolKind kind;
    Type* type = nullptr;
    llvm::AllocaInst* val = nullptr;
};

std::string string_of_symbol_type(const SymbolKind&);