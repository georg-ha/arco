#include <gtest/gtest.h>
#include "lexer.h"
#include "source_file.h"
#include "token.h"
#include "token_type.h"

class LexerTest : public ::testing::Test {
protected:
    std::unique_ptr<SourceFile> file;
    std::unique_ptr<Lexer> lexer;

    void SetUpInput(const std::vector<std::string>& input) {
        file = std::make_unique<SourceFile>(input);
        lexer = std::make_unique<Lexer>(*file);
    }
};


TEST_F(LexerTest, SkipsComments) {
    SetUpInput({"2.5#Comment", "2"});
    const Token t1 = lexer->advance();
    EXPECT_EQ(t1.type, TokenType::FloatConst);
    const Token t2 = lexer->advance();
    EXPECT_EQ(t2.type, TokenType::Newline);
}

TEST_F(LexerTest, HandlesNewlineAndEOF) {
    SetUpInput({"2"});
    lexer->advance();
    Token t = lexer->advance();
    EXPECT_EQ(t.type, TokenType::Newline);
    t = lexer->advance();
    EXPECT_EQ(t.type, TokenType::Eof);
}

TEST_F(LexerTest, RecognizesNumLiterals) {
    SetUpInput({"2 2.5"});
    const Token intLiteral = lexer->advance();
    const Token floatLiteral = lexer->advance();
    EXPECT_EQ(intLiteral.lexeme, "2");
    EXPECT_EQ(intLiteral.type, TokenType::IntConst);

    EXPECT_EQ(floatLiteral.lexeme, "2.5");
    EXPECT_EQ(floatLiteral.type, TokenType::FloatConst);
}

TEST_F(LexerTest, RecognizesKeywords) {
    SetUpInput({"let x"});
    const Token keyword = lexer->advance();
    const Token id = lexer->advance();

    EXPECT_EQ(keyword.type, TokenType::Let);
    EXPECT_EQ(id.type, TokenType::Id);
}


TEST_F(LexerTest, CharLiteralHandlesEscapeChar) {
    SetUpInput({"'\\t'"});
    const Token t= lexer->advance();
    EXPECT_EQ("\t", t.lexeme);
}

TEST_F(LexerTest, StrLiteralHandlesEscapeChar) {
    SetUpInput({R"("hi\t")"});
    Token t = lexer->advance();
    EXPECT_EQ("hi\t", t.lexeme);
}

TEST_F(LexerTest, HandlesTwoCharSymbols) {
    SetUpInput({"(<="});
    const Token lp = lexer->advance();
    const Token lt = lexer->advance();

    EXPECT_EQ(lp.type, TokenType::LParen);
    EXPECT_EQ(lt.type, TokenType::LessEqual);
}

TEST_F(LexerTest, SetsPosition) {
    SetUpInput({"let"});
    const Token l = lexer->advance();
    EXPECT_EQ(l.loc.start.column, 1);
    EXPECT_EQ(l.loc.start.line, 1);
    EXPECT_EQ(l.loc.end.column, 4);
}

TEST_F(LexerTest, PositionOfNewline) {
    SetUpInput({"2"});
    lexer->advance();
    const auto nl = lexer->advance();
    EXPECT_EQ(nl.loc.start, Position(1, 2));
    EXPECT_EQ(nl.loc.end, Position(1, 3));
}