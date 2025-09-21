#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

#include "codegen_visitor.h"
#include "stmt_nodes.h"
#include "token_error.h"
#include "type_checker.h"


llvm::Type* CodegenVisitor::get_llvm_type(const Type* arco_ty) const {
    if (arco_ty == type_checker.int_ty()) {
        return llvm::Type::getInt32Ty(context);
    }
    if (arco_ty == type_checker.float_ty()) {
        return llvm::Type::getFloatTy(context);
    }
    if (arco_ty == type_checker.bool_ty()) {
        return llvm::Type::getInt1Ty(context);
    }
    if (arco_ty == type_checker.unit_ty()) {
        return llvm::Type::getVoidTy(context);
    }
    throw std::runtime_error("Invalid type");
}

void CodegenVisitor::visit(FunDef &def) {
    auto* f = module.getFunction(def.signature.id);
    if (!f) {
        f = get_llvm_signature(def.signature);
    }

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "entry", f);
    builder.SetInsertPoint(bb);


    llvm::IRBuilder<> tmpB(&f->getEntryBlock(), f->getEntryBlock().begin());
    for (auto& arg : f->args()) {
        llvm::Type* argTy = arg.getType();

        llvm::AllocaInst* alloca = tmpB.CreateAlloca(argTy, nullptr, arg.getName());

        builder.CreateStore(&arg, alloca);

        def.scope.get_symbol(std::string(arg.getName())).val = alloca;
    }


    auto* v = def.body->codegen_accept(*this);
    if (!builder.GetInsertBlock()->getTerminator()) {
        if (v) {
            builder.CreateRet(v);
        } else if (def.signature.type) {
            builder.CreateRetVoid();
        } else if (f->getReturnType()->isIntegerTy() && def.signature.id == "main") {
            builder.CreateRet(llvm::ConstantInt::get(f->getReturnType(), 0));
        } else {
            throw TokenError("Missing Function terminator outside a main function", def.loc);
        }
    }

    llvm::verifyFunction(*f);
}

llvm::Function* CodegenVisitor::get_llvm_signature(const FunSignature &sig) const {
    std::vector<llvm::Type*> arg_types;
    arg_types.reserve(sig.args.size());
    for (const auto& arg : sig.args) {
        arg_types.push_back(get_llvm_type(arg.parent_scope->resolve(arg.id).value().type));
    }
    const auto return_type = get_llvm_type(sig.anno.type);

    const auto ft = llvm::FunctionType::get(return_type, arg_types, false);

    const auto function = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, sig.id, &module);
    int i = 0;
    for (auto& arg: function->args()) {
        arg.setName(sig.args[i++].id);
    }
    return function;

}

void CodegenVisitor::visit(const ExternalStmt &stmt) const {
    auto* f = get_llvm_signature(stmt.signature);
}

void CodegenVisitor::visit(const VarInit& stmt) {
    llvm::Function* fn = builder.GetInsertBlock()->getParent();

    llvm::IRBuilder<> tmpB(&fn->getEntryBlock(), fn->getEntryBlock().begin());

    llvm::Type* llvmTy = get_llvm_type(stmt.val->type);
    llvm::AllocaInst* alloca = tmpB.CreateAlloca(llvmTy, nullptr, stmt.id);

    llvm::Value* initVal = stmt.val->codegen_accept(*this);

    builder.CreateStore(initVal, alloca);

    stmt.parent_scope->get_symbol(stmt.id).val = alloca;
}

void CodegenVisitor::visit(const Assignment &stmt) {
    const auto& sym = stmt.parent_scope->get_symbol(stmt.assignee);
    llvm::Value* alloca = sym.val;

    llvm::Value* rhs = stmt.val->codegen_accept(*this);

    builder.CreateStore(rhs, alloca);
}
