#include "type_checker.h"

#include <iostream>

#include "anno_mismatch_error.h"
#include "expr_nodes.h"
#include "format_specifier.h"
#include "stmt_nodes.h"
#include "token_error.h"
#include "type_error.h"

TypeChecker::TypeChecker(std::unordered_map<std::string, std::unique_ptr<Type>>& type_env)
    : type_env(type_env){}


Type* TypeChecker::add_type(const Type& type) const {
    const std::string type_string = type.to_string();
    if (!type_env.contains(type_string)) {
        type_env[type_string] = type.clone();
    }
    return type_env.at(type_string).get();
}

void TypeChecker::visit(TypeAnno &typeAnno) {
    using enum TypeAnno::Kind;
    switch (typeAnno.kind) {
        case Int: typeAnno.type = int_ty(); break;
        case Float: typeAnno.type = float_ty(); break;
        case Bool: typeAnno.type = bool_ty(); break;
        case Char: typeAnno.type = char_ty(); break;
        case String: typeAnno.type = string_ty(); break;
        case Unit: typeAnno.type = unit_ty(); break;
    }
}

void TypeChecker::visit(ParenExpr &expr) {
    expr.expr->accept(*this);
    expr.type = expr.expr->type;
}

void TypeChecker::visit(BlockExpr &expr) {
    if (expr.body.empty()) {
        expr.type = unit_ty();
        return;
    }
    const size_t len = expr.body.size();
    for (size_t i = 0; i < len; i++) {
        expr.body[i]->accept(*this);
        if (i != len-1 && expr.body[i]->type != unit_ty()) {
            throw TypeError(expr.body[i]->loc, "Non unit expressions can only come at the end of a block."
                                               "Consider using \"let\" or \"var\"");
        }
    }

    expr.type = expr.body.back()->type;
}

void TypeChecker::visit(UnaryExpr &expr) {
    expr.operand->accept(*this);
    using enum UnaryOp;
    switch (expr.op) {
        case Not: if (expr.operand->type != bool_ty()) {
            throw TypeError(expr.operand->loc, "Expected an expression of type bool!");
        }
        case Plus:
        case Minus: if (!is_num(expr.operand->type)) {
            throw TypeError(expr.operand->loc, "Expected an expression of type int or float!");
        }
    }
    expr.type = expr.operand->type;
}

void TypeChecker::visit(BinaryExpr &expr) {
    expr.lhs->accept(*this);
    expr.rhs->accept(*this);
    auto* l_ty = expr.lhs->type;
    auto* r_ty = expr.rhs->type;

    using enum BinaryOp;
    switch (expr.op) {
        case Concat: if (l_ty != string_ty() || r_ty != string_ty()) {
            throw TypeError(expr.loc, "'^' expects two expressions of type string!");
        }
            break;
        case Equals:
        case NotEquals: if (l_ty != r_ty) {
            throw TypeError(expr.loc, "Only expressions of the same type can be checked for (in)equality!");
        }
            expr.type = bool_ty();
            break;
        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod: if (l_ty != r_ty || !is_num(l_ty)) {
            throw TypeError(expr.loc, "Arithmetic operations only work on expressions of type int or float!");
            }
            expr.type = l_ty;
            break;
        case Less:
        case Greater:
        case LessEquals:
        case GreaterEquals: if (l_ty != r_ty) {
            throw TypeError(expr.loc, "Only expressions of the same type can be compared");
                }
            if (l_ty == unit_ty() || l_ty == bool_ty()) {
            throw TypeError(expr.loc, "Expressions of type " + l_ty->to_string() + "can't be compared");
            }
            expr.type = bool_ty();
            break;
        case And:
        case Or:
            if (l_ty != bool_ty() || r_ty != bool_ty()) {
                throw TypeError(expr.loc, str_of_binary_op(expr.op) + " expects two expressions of type bool");
            }
            expr.type = bool_ty();
    }
}

void TypeChecker::visit(IdExpr &expr) {
    expr.type = expr.parent_scope->get_symbol(expr.id).type;

}

void TypeChecker::visit(IfElseExpr &expr) {
    expr.condition->accept(*this);
    if (expr.condition->type != bool_ty()) {
        throw TypeError(expr.condition->loc, "Expected an expression of type bool!");
    }
    expr.if_branch->accept(*this);
    if (expr.if_branch->type != unit_ty() && !expr.else_branch.has_value()) {
        throw TypeError(expr.loc, "if branch isn't of type unit but there is no else branch!");
    }
    if (expr.else_branch.has_value()) {
        expr.else_branch.value()->accept(*this);

        if (expr.if_branch->type != expr.else_branch.value()->type) {
            throw TypeError(expr.loc, "Both branches need to have the same type!");
        }
    }
    expr.type = expr.if_branch->type;
}

void TypeChecker::visit(WhileExpr &expr) {
    expr.condition->accept(*this);
    if (expr.condition->type != bool_ty()) {
        throw TypeError(expr.condition->loc, "Expected an expression of type bool!");
    }

    expr.body->accept(*this);
    expr.type = expr.body->type;
}

void TypeChecker::visit(IntConst &expr) {
    expr.type = int_ty();
}

void TypeChecker::visit(FloatConst &expr) {
    expr.type = float_ty();
}

void TypeChecker::visit(CharConst &expr) {
    expr.type = char_ty();
}

void TypeChecker::visit(StringConst &expr) {
    expr.type = string_ty();
}

void TypeChecker::visit(BoolConst &expr) {
    expr.type = bool_ty();
}



void TypeChecker::visit(FunCall &expr) {
    for (const auto& arg: expr.args) {
        arg->accept(*this);
    }
    if (expr.callee == "printf") {
        handle_printf(expr);
        return;
    }

    // Necessary for all functions not defined at the module level
    if (!expr.parent_scope->get_symbol(expr.callee).type) {
        const auto& sym = std::get<FunSymbol>(expr.parent_scope->get_symbol(expr.callee).kind);
        sym.signature.accept(*this);
        expr.parent_scope->get_symbol(expr.callee).type = sym.signature.type;
    }

    const auto f_type = dynamic_cast<FunctionTy*>(expr.parent_scope->get_symbol(expr.callee).type);
    if (expr.args.size() != f_type->arg_types.size()) {
        throw TypeError(expr.loc,
            expr.callee + " expects " + std::to_string(expr.args.size()) + " arguments, found " +
           std::to_string( f_type->arg_types.size()));
    }
    for (size_t i = 0; i < expr.args.size(); i++) {
        if (expr.args[i]->type != f_type->arg_types[i]) {
            throw TypeError(expr.args[i]->loc,
                "Expected an argument of type " + f_type->arg_types[i]->to_string());
        }
    }
    expr.type = f_type->return_type;
}



void TypeChecker::handle_printf(FunCall &expr) const {
    if (expr.args.empty()) {
        throw TypeError(expr.loc, "printf expects at least one string literal");
    }
    const auto* str = dynamic_cast<const StringConst*>(expr.args[0].get());
    if (!str) {
        throw TypeError(expr.args[0]->loc, "printf expects a string literal as its first argument!");
    }
    expr.type = unit_ty();
    std::vector<FormatSpecifier> specifiers = get_format_specifiers(*str);
    if (expr.args.size() - 1 != specifiers.size()) {
        throw TypeError(expr.loc,
            "printf expected " + std::to_string(specifiers.size())  +
            " arguments after the string literal, but found " + std::to_string(expr.args.size() - 1));
    }
    for (size_t i = 1; i < expr.args.size(); i++) {
        if (!expr_matches_format(*this, *expr.args[i], specifiers[i-1])) {
            throw TypeError(specifiers[i-1].loc, "Format specifier and expression type don't match!");
        }
    }
}

void TypeChecker::visit(ExprStmt &stmt) {
    stmt.expr->accept(*this);
    stmt.type = stmt.expr->type;
}

void TypeChecker::visit(VarInit &stmt) {
    stmt.val->accept(*this);
    if (stmt.anno.has_value()) {
        stmt.anno.value().accept(*this);
        if (stmt.anno.value().type != stmt.val->type) {
            throw AnnoMismatchError(stmt);
        }
    }
    stmt.parent_scope->get_symbol(stmt.id).type = stmt.val->type;
    stmt.type = unit_ty();
}

void TypeChecker::visit(Assignment &stmt) {
    stmt.val->accept(*this);
    if (stmt.val->type != stmt.parent_scope->get_symbol(stmt.assignee).type) {
        throw TypeError(stmt.loc, stmt.assignee + " has type " +
            stmt.parent_scope->get_symbol(stmt.assignee).type->to_string());
    }
    stmt.type = unit_ty();
}

void TypeChecker::visit(DefArg &arg) {
    arg.anno.accept(*this);
    arg.parent_scope->get_symbol(arg.id).type = arg.anno.type;
}

void TypeChecker::visit(FunSignature &signature) {
    std::vector<Type*> args;
    for (auto& arg: signature.args) {
        arg.accept(*this);
        args.push_back(arg.anno.type);
    }
    signature.anno.accept(*this);
    signature.type = add_type(FunctionTy(args, signature.anno.type));

}

void TypeChecker::visit(FunDef &def) {
    def.type = unit_ty();
    def.signature.accept(*this);
    def.body->accept(*this);

    if (def.signature.id == "main" && def.signature.anno.type != int_ty()) {
        throw TokenError("The main function has to have a return type of int", def.signature.anno.loc);
    }

    if (def.body->type != def.signature.anno.type) {
        throw TokenError("Type of function body doesn't match type annotation", def.signature.loc);
    }
    def.parent_scope->get_symbol(def.signature.id).type = def.signature.type;
}

void TypeChecker::visit(ExternalStmt &stmt) {
    stmt.type = unit_ty();
    stmt.signature.accept(*this);
    stmt.parent_scope->get_symbol(stmt.signature.id).type = stmt.signature.type;
}