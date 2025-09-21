#pragma once
#include <llvm/IR/IRBuilder.h>
#include "stmt.h"
#include "expr.h"

struct TypeChecker;

namespace llvm {
class LLVMContext;
class Module;
class Value;
    class Function;
} 


struct CodegenVisitor {
    llvm::LLVMContext& context;
    llvm::IRBuilder<>& builder;
    llvm::Module& module;
    Scope& module_scope;
    TypeChecker& type_checker;

    llvm::Type* get_llvm_type(const Type* arco_ty) const;

    llvm::Value* visit(const ParenExpr& expr);
    llvm::Value* visit(const BlockExpr& expr);
    llvm::Value* visit(const UnaryExpr& expr);
    llvm::Value* visit(const BinaryExpr& expr);

    llvm::Value* visit(const IdExpr& expr) const;
    llvm::Value* visit(const IntConst& expr) const;
    llvm::Value* visit(const FloatConst& expr) const;
    llvm::Value* visit(const CharConst& expr) const;
    llvm::Value* visit(const StringConst& expr) const;
    llvm::Value* visit(const BoolConst& expr) const;
    llvm::Value* visit(const FunCall& expr);
    llvm::Value* visit(const IfElseExpr& expr);
    llvm::Value* visit(const WhileExpr& expr);

    llvm::Value* visit(const ExprStmt& stmt);
    void visit(const VarInit& stmt);
    void visit(const Assignment& stmt);
    void visit(FunDef& def);
    llvm::Function* get_llvm_signature(const FunSignature& sig) const;
    void visit(const ExternalStmt& stmt) const;
};