#include "name_resolution.h"

#include "expr_nodes.h"
#include "module.h"
#include "stmt_nodes.h"
#include "token_error.h"
#include "unknown_id_error.h"

void NameResolution::visit(TypeAnno &typeAnno) {
    typeAnno.scope = scope;
}

void NameResolution::visit(ParenExpr &expr) {
    expr.parent_scope = scope;
    expr.expr->accept(*this);
}

void NameResolution::visit(BlockExpr &expr) {
    expr.parent_scope = scope;
    scope = &expr.scope;
    scope->parent_scope = expr.parent_scope;
    for (const auto& stmt: expr.body) {
        stmt->accept(*this);
    }
    scope = expr.parent_scope;
}

void NameResolution::visit(UnaryExpr &expr) {
    expr.parent_scope = scope;
    expr.operand->accept(*this);
}

void NameResolution::visit(BinaryExpr &expr) {
    expr.parent_scope = scope;
    expr.lhs->accept(*this);
    expr.rhs->accept(*this);
}

void NameResolution::visit(IdExpr &expr) {
    expr.parent_scope = scope;
    if (!expr.parent_scope->resolve(expr.id).has_value()) {
        throw UnknownIdError(expr.loc, expr.id);
    }
    auto sym = expr.parent_scope->get_symbol(expr.id).kind;
    if (!std::holds_alternative<VarSymbol>(sym) && !std::holds_alternative<ParamSymbol>(sym)) {
        throw TokenError(expr.id + " is a " + string_of_symbol_type(sym) + " and can't be used in this context", expr.loc);
    }
}

void NameResolution::visit(IfElseExpr &expr) {
    expr.parent_scope = scope;
    expr.condition->accept(*this);
    expr.if_branch->accept(*this);
    if (expr.else_branch.has_value()) {
        expr.else_branch.value()->accept(*this);
    }
}

void NameResolution::visit(WhileExpr &expr) {
    expr.parent_scope = scope;
    expr.condition->accept(*this);
    expr.body->accept(*this);
}

void NameResolution::visit(IntConst &expr) {

}

void NameResolution::visit(FloatConst &expr) {
}

void NameResolution::visit(CharConst &expr) {
}

void NameResolution::visit(StringConst &expr) {
}

void NameResolution::visit(BoolConst &expr) {
}

void NameResolution::visit(FunCall &expr) {
    if (expr.callee == "main") {
        throw TokenError("The \"main\" function can't be called", expr.loc);
    }
    expr.parent_scope = scope;
    for (const auto& arg: expr.args) {
        arg->accept(*this);
    }

    if (expr.callee == "printf") {
        return;
    }
    if (!scope->resolve(expr.callee).has_value()) {
        throw UnknownIdError(expr.loc, expr.callee);
    }
    if (!expr.parent_scope->is_function(expr.callee)) {
        throw TokenError(expr.callee + " is a " +
            string_of_symbol_type(expr.parent_scope->get_symbol(expr.callee).kind)
            + " and can't be used in this context", expr.loc);
    }
}

void NameResolution::visit(ExprStmt &stmt) {
    stmt.parent_scope = scope;
    stmt.expr->accept(*this);
}

void NameResolution::visit(VarInit &stmt) {
    stmt.parent_scope = scope;
    scope->add_var(stmt.loc, stmt);
    if (stmt.anno.has_value()) {
        stmt.anno.value().accept(*this);
    }
    stmt.val->accept(*this);
}

void NameResolution::visit(Assignment &stmt) {
    stmt.parent_scope = scope;
    if (!stmt.parent_scope->resolve(stmt.assignee).has_value()) {
        throw UnknownIdError(stmt.loc, stmt.assignee);
    }
    if (!stmt.parent_scope->can_be_reassigned(stmt.assignee)) {
        throw TokenError(stmt.assignee + " can't be reassigned!", stmt.loc);
    }
    stmt.val->accept(*this);

}

void NameResolution::visit(DefArg &arg) {
    arg.parent_scope = scope;
    scope->add_param(arg);

}

void NameResolution::visit(FunSignature &signature) {
    signature.parent_scope = scope;
    for (auto& arg : signature.args) {
        arg.accept(*this);
    }
    signature.anno.accept(*this);
}

void NameResolution::visit(FunDef &def) {
    def.parent_scope = scope;
    if (scope->kind != Scope::Kind::Module) {
        scope->add_function(def.loc, def.signature);
    }
    scope = &def.scope;
    scope->parent_scope = def.parent_scope;
    def.signature.accept(*this);
    def.body->accept(*this);
    scope = def.parent_scope;
}

void NameResolution::visit(ExternalStmt &stmt) {
    if (stmt.signature.id == "main") {
        throw TokenError("function \"main\" can't be declared as external",stmt.signature.loc);
    }
    stmt.parent_scope = scope;
    if (scope->kind != Scope::Kind::Module) {
        scope->add_function(stmt.loc, stmt.signature);
    }
    stmt.signature.accept(*this);
}



