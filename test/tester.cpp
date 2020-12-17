#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../src/DialLexer.h"


using namespace dial;


enum class dial::TokenType : int {
    IF,
    ELSE,
    WHILE,
    NUMBER,
    STRING, 
    IDENTIFIER
};

TEST_CASE("Testing Raw Lexer") {

    DialLexer dial_lexer;

    //define syntax tokens
    dial_lexer.add_token({ TokenType::IF, "if", DIAL_LEXER_VALUE::DIAL_NONE });
    dial_lexer.add_token({ TokenType::WHILE, "while", DIAL_LEXER_VALUE::DIAL_NONE });
    dial_lexer.add_token({ TokenType::NUMBER, IS_NUMBER, DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE });
    dial_lexer.add_token({ TokenType::STRING, "\"", DIAL_LEXER_VALUE::DIAL_STRING_START });
    dial_lexer.add_token({ TokenType::STRING, "\"", DIAL_LEXER_VALUE::DIAL_STRING_END });
    dial_lexer.add_token({ TokenType::IDENTIFIER, IS_IDENTIFIER_ALPHA_LOWER + "|_", DIAL_LEXER_VALUE::DIAL_IDENTIFIER });
    
    //load up source code
    string sc = "if while 8.54 \n \"this is a string\" identifier_var";
    //split the grammer into mapped tokens
    vector<Token> splitted_tokens = dial_lexer.split(sc);

    REQUIRE(dial_lexer.input_token_size() == 6);
    REQUIRE(splitted_tokens.size() == 5);
    
    SUBCASE("test output tokens") {
        //check for token types
        CHECK(static_cast<int>(splitted_tokens.at(0).get_type()) == 0);
        CHECK(static_cast<int>(splitted_tokens.at(1).get_type()) == 2);
        CHECK(static_cast<int>(splitted_tokens.at(2).get_type()) == 3);
        CHECK(static_cast<int>(splitted_tokens.at(3).get_type()) == 4);
        CHECK(static_cast<int>(splitted_tokens.at(4).get_type()) == 5);

        //check for token lexemes
        CHECK(splitted_tokens.at(0).get_lexeme() == "if");
        CHECK(splitted_tokens.at(1).get_lexeme() == "while");
        CHECK(splitted_tokens.at(2).get_lexeme() == "8.54");
        CHECK(splitted_tokens.at(3).get_lexeme() == "\"this is a string\"");
        CHECK(splitted_tokens.at(4).get_lexeme() == "identifier_var");

        //check for token line
        CHECK(splitted_tokens.at(0).get_line() == 1);
        CHECK(splitted_tokens.at(1).get_line() == 1);
        CHECK(splitted_tokens.at(2).get_line() == 1);
        CHECK(splitted_tokens.at(3).get_line() == 2);
        CHECK(splitted_tokens.at(4).get_line() == 2);

        //check for token value
        CHECK(splitted_tokens.at(0).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
        CHECK(splitted_tokens.at(1).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
        CHECK(splitted_tokens.at(2).get_value() == DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE);
        CHECK(splitted_tokens.at(3).get_value() == DIAL_LEXER_VALUE::DIAL_STRING);
        CHECK(splitted_tokens.at(4).get_value() == DIAL_LEXER_VALUE::DIAL_IDENTIFIER);

    }
    
    SUBCASE("test comment") {
        string comment_sc;
        SUBCASE("one token comment") {
            comment_sc = sc.append(" #This is super fun, hahahaha\n");
            dial_lexer.set_comment("#");
            vector<Token> splitted_tokens = dial_lexer.split(sc);
            //check for token types
            CHECK(static_cast<int>(splitted_tokens.at(0).get_type()) == 0);
            CHECK(static_cast<int>(splitted_tokens.at(1).get_type()) == 2);
            CHECK(static_cast<int>(splitted_tokens.at(2).get_type()) == 3);
            CHECK(static_cast<int>(splitted_tokens.at(3).get_type()) == 4);
            CHECK(static_cast<int>(splitted_tokens.at(4).get_type()) == 5);

            //check for token lexemes
            CHECK(splitted_tokens.at(0).get_lexeme() == "if");
            CHECK(splitted_tokens.at(1).get_lexeme() == "while");
            CHECK(splitted_tokens.at(2).get_lexeme() == "8.54");
            CHECK(splitted_tokens.at(3).get_lexeme() == "\"this is a string\"");
            CHECK(splitted_tokens.at(4).get_lexeme() == "identifier_var");

            //check for token line
            CHECK(splitted_tokens.at(0).get_line() == 1);
            CHECK(splitted_tokens.at(1).get_line() == 1);
            CHECK(splitted_tokens.at(2).get_line() == 1);
            CHECK(splitted_tokens.at(3).get_line() == 2);
            CHECK(splitted_tokens.at(4).get_line() == 2);

            //check for token value
            CHECK(splitted_tokens.at(0).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
            CHECK(splitted_tokens.at(1).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
            CHECK(splitted_tokens.at(2).get_value() == DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE);
            CHECK(splitted_tokens.at(3).get_value() == DIAL_LEXER_VALUE::DIAL_STRING);
            CHECK(splitted_tokens.at(4).get_value() == DIAL_LEXER_VALUE::DIAL_IDENTIFIER);
        }
        
        SUBCASE("two token comment") {
            dial_lexer.set_comment("/*", "*/");
            comment_sc = sc.append(" /*This*/  8.5");
            vector<Token> splitted_tokens = dial_lexer.split(sc);
            //check for token types
            CHECK(static_cast<int>(splitted_tokens.at(0).get_type()) == 0);
            CHECK(static_cast<int>(splitted_tokens.at(1).get_type()) == 2);
            CHECK(static_cast<int>(splitted_tokens.at(2).get_type()) == 3);
            CHECK(static_cast<int>(splitted_tokens.at(3).get_type()) == 4);
            CHECK(static_cast<int>(splitted_tokens.at(4).get_type()) == 5);

            //check for token lexemes
            CHECK(splitted_tokens.at(0).get_lexeme() == "if");
            CHECK(splitted_tokens.at(1).get_lexeme() == "while");
            CHECK(splitted_tokens.at(2).get_lexeme() == "8.54");
            CHECK(splitted_tokens.at(3).get_lexeme() == "\"this is a string\"");
            CHECK(splitted_tokens.at(4).get_lexeme() == "identifier_var");

            //check for token line
            CHECK(splitted_tokens.at(0).get_line() == 1);
            CHECK(splitted_tokens.at(1).get_line() == 1);
            CHECK(splitted_tokens.at(2).get_line() == 1);
            CHECK(splitted_tokens.at(3).get_line() == 2);
            CHECK(splitted_tokens.at(4).get_line() == 2);

            //check for token value
            CHECK(splitted_tokens.at(0).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
            CHECK(splitted_tokens.at(1).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
            CHECK(splitted_tokens.at(2).get_value() == DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE);
            CHECK(splitted_tokens.at(3).get_value() == DIAL_LEXER_VALUE::DIAL_STRING);
            CHECK(splitted_tokens.at(4).get_value() == DIAL_LEXER_VALUE::DIAL_IDENTIFIER);
        }

    }

    SUBCASE("test invalid input") {
        string invalid_input = "this & * \n while if + -- ^ % @ !";
        try {
            vector<Token> splitted_invalid_tokens = dial_lexer.split(invalid_input);
        }
        catch (DialLexerException ex) {
            CHECK(string(ex.what()) == "Lexer Error :\n& *  at position 9 at line 1+ -- ^ % @ ! at position 32 at line 2");
        }
    }

    SUBCASE("test validation") {
        SUBCASE("string token must have starting and ending  token") {
            DialLexer dial_lexer;

            dial_lexer.add_token({ TokenType::STRING, "\"", DIAL_LEXER_VALUE::DIAL_STRING_START });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\nstring tokens must have starting and ending tokens \n  occured while validating tokens ");
            }
        }
        SUBCASE("invalid use of value DIAL_STRING") {
            DialLexer dial_lexer;

            dial_lexer.add_token({ TokenType::STRING, "\"", DIAL_LEXER_VALUE::DIAL_STRING });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\nstring token is not valid for use \n  occured while validating tokens ");
            }
        }
        SUBCASE("same token can't be used for start and end comment") {
            DialLexer dial_lexer;

            dial_lexer.set_comment("#", "#");
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\ncant have same token as start and end comment occured while validating tokens ");
            }
        }

        SUBCASE("Token Values must be unique (exception -> DIAL_NONE)") {
            DialLexer dial_lexer;

            dial_lexer.add_token({ TokenType::NUMBER, IS_NUMBER, DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE });
            dial_lexer.add_token({ TokenType::NUMBER, IS_NUMBER, DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\ntokens must be unique except for NONE value -> IS_NUMBER\ntokens must be unique except for NONE value -> IS_NUMBER\n occured while validating tokens ");
            }
        }
        SUBCASE("Input Token Identifier Must Have Value") {
            DialLexer dial_lexer;

            dial_lexer.add_token({ TokenType::IDENTIFIER, "", DIAL_LEXER_VALUE::DIAL_IDENTIFIER });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\ntoken identifier must have identifiable characters or use the special constant values \n occured while validating tokens ");
            }
        }
        SUBCASE("Input Token Identifier Must Be Unique") {
            DialLexer dial_lexer;

            dial_lexer.add_token({ TokenType::IDENTIFIER, "tok|tok", DIAL_LEXER_VALUE::DIAL_IDENTIFIER });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\nidentifier constants must be unique except  \nidentifier constants must be unique except  \n occured while validating tokens ");
            }
        }
        SUBCASE("Input Token Identifier Must Have One Use Case For Special IS_IDENTIFIER_ALPHA_NUM") {
            DialLexer dial_lexer;

            dial_lexer.add_token({ TokenType::IDENTIFIER, IS_IDENTIFIER_ALPHA_LOWER + "|" + IS_IDENTIFIER_ALPHA_NUM, DIAL_LEXER_VALUE::DIAL_IDENTIFIER });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\nidentifier reserved must not use 'IS_IDENTIFIER_ALPHA_NUM' with the others \n  occured while validating tokens ");
            }
        }
    }
}

TEST_CASE("Testing Regex Lexer") {
    DialLexer dial_lexer{ LexerType::REGEX };

    //define syntax tokens
    dial_lexer.add_token({ TokenType::IF, "if", DIAL_LEXER_VALUE::DIAL_NONE });
    dial_lexer.add_token({ TokenType::WHILE, "while", DIAL_LEXER_VALUE::DIAL_NONE });
    dial_lexer.add_token({ TokenType::NUMBER, "[0-9]+", DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE });

    //load up source code
    string sc = "if while 85645 \n if";
    //split the grammer into mapped tokens
    vector<Token> splitted_tokens = dial_lexer.split(sc);

    REQUIRE(dial_lexer.input_token_size() == 3);
    REQUIRE(splitted_tokens.size() == 4);

    SUBCASE("test output tokens") {
        //check for token types
        CHECK(static_cast<int>(splitted_tokens.at(0).get_type()) == 0);
        CHECK(static_cast<int>(splitted_tokens.at(1).get_type()) == 2);
        CHECK(static_cast<int>(splitted_tokens.at(2).get_type()) == 3);
        CHECK(static_cast<int>(splitted_tokens.at(3).get_type()) == 0);

        //check for token lexemes
        CHECK(splitted_tokens.at(0).get_lexeme() == "if");
        CHECK(splitted_tokens.at(1).get_lexeme() == "while");
        CHECK(splitted_tokens.at(2).get_lexeme() == "85645");
        CHECK(splitted_tokens.at(3).get_lexeme() == "if");

        //check for token line
        CHECK(splitted_tokens.at(0).get_line() == 1);
        CHECK(splitted_tokens.at(1).get_line() == 1);
        CHECK(splitted_tokens.at(2).get_line() == 1);
        CHECK(splitted_tokens.at(3).get_line() == 2);

        //check for token value
        CHECK(splitted_tokens.at(0).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
        CHECK(splitted_tokens.at(1).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);
        CHECK(splitted_tokens.at(2).get_value() == DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE);
        CHECK(splitted_tokens.at(3).get_value() == DIAL_LEXER_VALUE::DIAL_NONE);

    }

    SUBCASE("test invalid input") {
        string invalid_input = "this & * \n while if + -- ^ % @ !";
        try {
            vector<Token> splitted_invalid_tokens = dial_lexer.split(invalid_input);
        }
        catch (DialLexerException ex) {
            CHECK(string(ex.what()) == "Lexer Error :\n& *  at position 9 at line 1+ -- ^ % @ ! at position 32 at line 2this & * \n  at position 16 at line 1 + -- ^ % @ ! at position 19 at line 2");
        }
    }

    SUBCASE("test validation") {
        SUBCASE("string token must have starting and ending  token") {
            DialLexer dial_lexer{ LexerType::REGEX };

            dial_lexer.add_token({ TokenType::NUMBER, "[0-9]+", DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE });
            dial_lexer.add_token({ TokenType::NUMBER, "[0-9]+", DIAL_LEXER_VALUE::DIAL_NUMBER_DOUBLE });
            try {
                vector<Token> splitted_invalid_tokens = dial_lexer.split(sc);
            }
            catch (DialLexerException ex) {
                CHECK(string(ex.what()) == "Lexer Error :\nregex tokens must be unique  -> [0-9]+\nregex tokens must be unique  -> [0-9]+\n occured while validating tokens ");
            }
        }
        
    }
}