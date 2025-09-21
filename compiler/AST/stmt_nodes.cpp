#include "stmt_nodes.h"
#include "visitor.h"
#include "codegen_visitor.h"

void ExprStmt::accept(Visitor& visitor)  { return visitor.visit(*this); }
void ExprStmt::codegen_accept(CodegenVisitor& visitor) { visitor.visit(*this); }

void VarInit::accept(Visitor& visitor)  { return visitor.visit(*this); }
void VarInit::codegen_accept(CodegenVisitor& visitor) { visitor.visit(*this); }

void Assignment::accept(Visitor& visitor)  { return visitor.visit(*this); }
void Assignment::codegen_accept(CodegenVisitor& visitor) { visitor.visit(*this); }

void DefArg::accept(Visitor& visitor) { return visitor.visit(*this); }

void FunSignature::accept(Visitor& visitor)  { return visitor.visit(*this); }

void FunDef::accept(Visitor& visitor)  { return visitor.visit(*this); }
void FunDef::codegen_accept(CodegenVisitor& visitor) { visitor.visit(*this); }

void ExternalStmt::accept(Visitor& visitor) { return visitor.visit(*this); }
void ExternalStmt::codegen_accept(CodegenVisitor& visitor) { visitor.visit(*this); }