#include <unordered_map>
#include <llvm/IR/LLVMContext.h>
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Support/TargetSelect.h"

#include "module.h"
#include "print_visitor.h"
#include "source_file.h"
#include "type.h"
#include "type_checker.h"


enum class CompilerFlags {
    PrintAst, EmitIR, None
};

int main(int argc, char** argv) {
    SourceFile src(argv[1]);
    std::unordered_map<std::string, std::unique_ptr<Type>> type_env;
    type_env["int"] = std::make_unique<BasicTy>(BasicTy::Kind::Int);
    type_env["float"] = std::make_unique<BasicTy>(BasicTy::Kind::Float);
    type_env["char"] = std::make_unique<BasicTy>(BasicTy::Kind::Char);
    type_env["string"] = std::make_unique<BasicTy>(BasicTy::Kind::String);
    type_env["bool"] = std::make_unique<BasicTy>(BasicTy::Kind::Bool);
    type_env["unit"] = std::make_unique<BasicTy>(BasicTy::Kind::Unit);
    auto ctx = std::make_unique<llvm::LLVMContext>();
    TypeChecker ty(type_env);

    auto flag = CompilerFlags::None;

    for (int i = 1; i < argc; i++) {
        const std::string arg = argv[i];
        if (arg == "--ast") {
            std::cout << "AST\n";
            flag = CompilerFlags::PrintAst;
            break;
        }
        if (arg == "--llvmIR") {
            flag = CompilerFlags::EmitIR;
            break;
        }
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    auto JIT = cantFail(llvm::orc::LLJITBuilder().create());

    auto &JD = JIT->getMainJITDylib();
    JD.addGenerator(
        cantFail(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
            JIT->getDataLayout().getGlobalPrefix())));



    Module m("module", *ctx, ty);

    try {
        m.parse(src);
    } catch (const std::exception& e) {
        std::cout << e.what();
        return 1;
    }

    if (flag == CompilerFlags::PrintAst) {
        PrintVisitor visitor;
        for (const auto& node : m.ast) {
            node->accept(visitor);
            return 0;
        }
    }
    try {
        m.run_sema();
        m.run_type_checker();
        m.run_codegen();
    } catch (const std::exception& e) {
        std::cout << e.what();
        return 1;
    }


    if (flag == CompilerFlags::EmitIR) {
        m.llvm_module->print(llvm::outs(), nullptr);
        return 0;
    }

    cantFail(JIT->addIRModule(llvm::orc::ThreadSafeModule(std::move(m.llvm_module), std::move(ctx))));

    // Look up your "main" or entry function
    const auto MainAddr = cantFail(JIT->lookup("main"));
    using MainFn = int();
    auto *Entry = MainAddr.toPtr<MainFn>();

    return Entry();
    return 0;

}
