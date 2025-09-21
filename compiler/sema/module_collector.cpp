#include "module_collector.h"
#include "stmt_nodes.h"

#include "module_collector_error.h"

void ModuleCollector::visit(FunDef &def) {
    mod_scope.add_function(def.loc, def.signature);
}

void ModuleCollector::visit(ExternalStmt &stmt) {
    mod_scope.add_function(stmt.loc, stmt.signature);
}

void ModuleCollector::visit(VarInit &stmt) {
}

void ModuleCollector::visit(TypeAnno &typeAnno) {
}

void ModuleCollector::visit(ParenExpr &expr) {
}

void ModuleCollector::visit(BlockExpr &expr) {
}

void ModuleCollector::visit(UnaryExpr &expr) {
}

void ModuleCollector::visit(BinaryExpr &expr) {
}

void ModuleCollector::visit(IdExpr &expr) {
}

void ModuleCollector::visit(IfElseExpr &expr) {
}

void ModuleCollector::visit(WhileExpr &expr) {
}

void ModuleCollector::visit(IntConst &expr) {
}

void ModuleCollector::visit(FloatConst &expr) {
}

void ModuleCollector::visit(CharConst &expr) {
}

void ModuleCollector::visit(StringConst &expr) {
}

void ModuleCollector::visit(BoolConst &expr) {
}

void ModuleCollector::visit(FunCall &expr) {
}

void ModuleCollector::visit(ExprStmt &stmt) {
    throw ModuleCollectorError(stmt);
}

void ModuleCollector::visit(Assignment &stmt) {
    throw ModuleCollectorError(stmt);
}

void ModuleCollector::visit(DefArg &arg) {
}

void ModuleCollector::visit(FunSignature &signature) {
}