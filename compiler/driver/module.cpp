#include "module.h"

#include <utility>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/CGSCCPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/Reassociate.h"

#include "module_collector.h"
#include "name_resolution.h"
#include "parser.h"
#include "scope.h"
#include "type_checker.h"
#include "codegen_visitor.h"

Module::Module(std::string name, llvm::LLVMContext& ctx, TypeChecker& ty)
    : name(std::move(name)),
    scope(Scope::Kind::Module),
    ctx(ctx),
    llvm_module(std::make_unique<llvm::Module>(name, ctx)),
    builder(ctx),
    type_checker(ty),
    codegen_visitor(ctx, builder, *llvm_module, scope, ty) {
    llvm::FunctionType *printfType = llvm::FunctionType::get(
        llvm::IntegerType::getInt32Ty(ctx),
        llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0),
        true// isVarArg
    );

    llvm::Function *func = llvm::Function::Create(
        printfType,
        llvm::Function::ExternalLinkage,
        "printf",
        *llvm_module
    );

    func->setCallingConv(llvm::CallingConv::C);
}

void Module::parse(SourceFile& src) {
    ast = parse_file(src);
}


void Module::run_sema() {
    ModuleCollector mc(scope);
    for (const auto& node : ast) {
        node->accept(mc);
    }
    NameResolution nr(&scope);
    for (const auto& node : ast) {
        node->accept(nr);
    }
}

void Module::run_type_checker() {
    for (const auto& node: ast) {
        node->accept(type_checker);
    }
}

void Module::run_codegen() {
    for (const auto& node : ast) {
        node->codegen_accept(codegen_visitor);
    }
    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;

    llvm::PassBuilder PB;

    llvm::FunctionPassManager FPM;

    FPM.addPass(llvm::PromotePass());
    FPM.addPass(llvm::InstCombinePass());
    FPM.addPass(llvm::ReassociatePass());

    // PB.registerModuleAnalyses(MAM);
    // PB.registerCGSCCAnalyses(CGAM);
    // PB.registerFunctionAnalyses(FAM);
    // PB.registerLoopAnalyses(LAM);
    // PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    // llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
    // MPM.run(*llvm_module, MAM);
    if (llvm::verifyModule(*llvm_module, &llvm::errs())) {
        llvm::errs() << "Invalid IR generated!\n";
        llvm_module->print(llvm::errs(), nullptr);
        return;
    }
}
