#pragma once
#include <vector>
#include <string>


struct Type {
    virtual ~Type() = default;

    virtual std::string to_string() const = 0;

    virtual std::unique_ptr<Type> clone() const = 0;
};


struct BasicTy final : Type {
    enum class Kind {Int, Float, Bool, Char, String, Unit} kind;

    explicit BasicTy(Kind);

    std::string to_string() const override;

    std::unique_ptr<Type> clone() const override;
};


struct FunctionTy final : Type {
    std::vector<Type*> arg_types;
    Type* return_type;

    FunctionTy(std::vector<Type*>, Type*);

    std::string to_string() const override;

    std::unique_ptr<Type> clone() const override;
};
