#pragma once
#include <vector>
#include <string>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "codegen_visitor.h"
#include "scope.h"
#include "type_checker.h"


struct TypeChecker;

struct Module {
    std::string name;
    Scope scope;
    std::vector<StmtPtr> ast;
    llvm::LLVMContext& ctx;
    std::unique_ptr<llvm::Module> llvm_module;
    llvm::IRBuilder<> builder;
    TypeChecker type_checker;
    CodegenVisitor codegen_visitor;



    Module(std::string name, llvm::LLVMContext& ctx, TypeChecker& ty);

    void parse(SourceFile& src);

    void run_sema();

    void run_type_checker();

    void run_codegen();
};
