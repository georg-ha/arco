#include "symbol.h"


std::string string_of_symbol_type(const SymbolKind& sym) {
    if (std::holds_alternative<VarSymbol>(sym)) {
        return "variable";
    }
    if (std::holds_alternative<FunSymbol>(sym)) {
        return "function";
    }
    if (std::holds_alternative<ModuleSymbol>(sym)) {
        return "module";
    }
    if (std::holds_alternative<ParamSymbol>(sym)) {
        return "function parameter";
    }
    return "";
}
