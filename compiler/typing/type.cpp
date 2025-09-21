#include "type.h"



std::string vec_to_string(const std::vector<Type*>& vector, const std::string& separator = ",") {
    if (vector.empty()) {
        return "";
    }

    std::string result = (*vector.begin())->to_string();

    for (auto it = ++vector.begin(); it != vector.end(); ++it) {

        result +=  separator + (*it)->to_string();
    }
    return result;
}

BasicTy::BasicTy(Kind k)
    : kind(k) {}


std::string BasicTy::to_string() const {
    using enum Kind;
    switch (kind) {
        case Int: return "int";
        case Float: return "float";
        case Char: return "char";
        case String: return "string";
        case Bool: return "bool";
        case Unit: return "unit";
        default: return "";
    }
}

std::unique_ptr<Type> BasicTy::clone() const {
    return std::make_unique<BasicTy>(kind);
}

FunctionTy::FunctionTy(std::vector<Type *> v, Type * t)
    : arg_types(std::move(v)), return_type(t){}

std::string FunctionTy::to_string() const {
    return "(" + vec_to_string(arg_types, ", ") + ") -> " + return_type->to_string();
}
std::unique_ptr<Type> FunctionTy::clone() const {
    return std::make_unique<FunctionTy>(arg_types, return_type);
}