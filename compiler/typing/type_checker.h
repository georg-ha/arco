#pragma once
#include <unordered_map>
#include "type.h"
#include "visitor.h"

struct TypeChecker final : Visitor {
    std::unordered_map<std::string, std::unique_ptr<Type>>& type_env;

    explicit TypeChecker(std::unordered_map<std::string, std::unique_ptr<Type>>& type_env);

    Type* add_type(const Type&) const;

    Type* int_ty() const { return type_env.at("int").get(); }
    Type* float_ty() const { return type_env.at("float").get(); }
    Type* char_ty() const { return type_env.at("char").get(); }
    Type* string_ty() const { return type_env.at("string").get(); }
    Type* bool_ty() const { return type_env.at("bool").get(); }
    Type* unit_ty() const { return type_env.at("unit").get(); }
    bool is_num(const Type* t) const {return t == int_ty() || t == float_ty();}

    void visit(TypeAnno &typeAnno) override;

    void visit(ParenExpr &expr) override;

    void visit(BlockExpr &expr) override;

    void visit(UnaryExpr &expr) override;

    void visit(BinaryExpr &expr) override;

    void visit(IdExpr &expr) override;

    void visit(IfElseExpr &expr) override;

    void visit(WhileExpr &expr) override;

    void visit(IntConst &expr) override;

    void visit(FloatConst &expr) override;

    void visit(CharConst &expr) override;

    void visit(StringConst &expr) override;

    void visit(BoolConst &expr) override;

    void visit(FunCall &expr) override;

    void visit(ExprStmt &stmt) override;

    void visit(VarInit &stmt) override;

    void visit(Assignment &stmt) override;

    void visit(DefArg &arg) override;

    void visit(FunSignature &signature) override;

    void visit(FunDef &def) override;

    void visit(ExternalStmt &stmt) override;

private:
    void handle_printf(FunCall& expr) const;
};

