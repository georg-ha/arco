#include "parser.h"

#include <gtest/gtest.h>

#include "expr_nodes.h"
#include "source_file.h"


class ParserTest : public ::testing::Test {
protected:
    std::unique_ptr<SourceFile> file;
    std::unique_ptr<Parser> parser;

    void SetUpInput(const std::vector<std::string>& in) {
        file = std::make_unique<SourceFile>(in);
        parser = std::make_unique<Parser>(*file);
    }
};


TEST_F(ParserTest, HandlesEmptyFile) {
    SetUpInput({""});
    EXPECT_EQ(parser->parse_stmt(), nullptr);
}

TEST_F(ParserTest, ParsesIntConst) {
    SetUpInput({"2"});
    const std::unique_ptr<Expr> cons = parser->parse_expr();
    const auto int_const = dynamic_cast<IntConst*>(cons.get());
    ASSERT_NE(int_const, nullptr);
    EXPECT_EQ(int_const->val, 2);
}

TEST_F(ParserTest, ParsesFloatConst) {
    SetUpInput({"2.5"});
    const std::unique_ptr<Expr> cons = parser->parse_expr();
    const auto int_const = dynamic_cast<FloatConst*>(cons.get());
    ASSERT_NE(int_const, nullptr);
    EXPECT_EQ(int_const->val, 2.5);
}


TEST_F(ParserTest, HandlesEmptyBlock) {
    SetUpInput({"{}"});
    const ExprPtr block = parser->parse_expr();
    const auto block_expr = dynamic_cast<BlockExpr*>(block.get());
    ASSERT_NE(block_expr, nullptr);
    EXPECT_EQ(block_expr->body.size(), 0);
}


TEST_F(ParserTest, HandlesOperatorPrecedence) {
    SetUpInput({"2 + 4 * 3"});
    const ExprPtr expr = parser->parse_expr();
    const auto binary_expr = dynamic_cast<BinaryExpr*>(expr.get());
    ASSERT_NE(binary_expr, nullptr);
    EXPECT_EQ(binary_expr->op, BinaryOp::Add);
}