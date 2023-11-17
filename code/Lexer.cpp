#include "Lexer.h"


namespace charinfo {

    LLVM_READNONE inline bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\f' ||
               c == '\v' || c == '\r' || c == '\n';
    }

    LLVM_READNONE inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    LLVM_READNONE inline bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z');
    }

    LLVM_READNONE inline bool isOperator(char c) {
        return c == '+' || c == '-' || c == '*' ||
               c == '/' || c == '^' || c == '=' ||
               c == '<' || c == '>' || c == '!' || 
               c == ':' || c == ',';
    }
}
void Lexer::next(Token &token) {

    while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {      // Skips whitespace like " "
        ++BufferPtr;
    }

    if (!*BufferPtr) {              // since end of context is 0 -> !0 = true -> end of context
        token.Kind = Token::eoi;
        return;
    }

    if (charinfo::isLetter(*BufferPtr)) {   // looking for keywords or identifiers like "int", a123 , ...

        const char *end = BufferPtr + 1;

        while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
            ++end;                          // until reaches the end of lexeme
                                            // example: ".int " -> "i.nt " -> "in.t " -> "int. "

        llvm::StringRef Context(BufferPtr, end - BufferPtr);  // start of lexeme, length of lexeme

        Token::TokenKind kind;

        switch(context) {       // checks if lexeme is a keyword or identifier

            case "int":
                kind = Token::KW_int;
                break;
            
            case "if":
                kind = Token::KW_if;
                break;

            case "elif":
                kind = Token::KW_elif;
                break;
            
            case "else":
                kind = Token::KW_else;
                break;
            
            case "else:":
                kind = Token::KW_else_colon;
                break;
            
            case "loopc":
                kind = Token::KW_loopc;
                break;
            
            case "and":
                kind = Token::KW_and;
                break;
            
            case "or":
                kind = Token::KW_or;
                break;
            
            case "true":
                kind = Token::KW_true;
                break;
            
            case "false":
                kind = Token::KW_false;
                break;

            default:
                kind = Token::ident;
                break;

        }

        formToken(token, end, kind);
        return;
    }

    else if (charinfo::isDigit(*BufferPtr)) {

        const char *end = BufferPtr + 1;

        while (charinfo::isDigit(*end))
            ++end;

        formToken(token, end, Token::number);
        return;
    }

    else {

        if(*BufferPtr == '=') && *(BufferPtr + 1) == '=' {      // ==
            formToken(token, BufferPtr + 2, Token::equal_equal);
            break;
        }

        if(*BufferPtr == '+' && *(BufferPtr + 1) == '=') {      // +=
            formToken(token, BufferPtr + 2, Token::plus_equal);
            break;
        }

        if(*BufferPtr == '-' && *(BufferPtr + 1) == '=') {      // -=
            formToken(token, BufferPtr + 2, Token::minus_equal);
            break;
        }

        if(*BufferPtr == '*' && *(BufferPtr + 1) == '=') {      // *=
            formToken(token, BufferPtr + 2, Token::star_equal);
            break;
        }

        if(*BufferPtr == '/' && *(BufferPtr + 1) == '=') {      // /=
            formToken(token, BufferPtr + 2, Token::slash_equal);
            break;
        }

        if(*BufferPtr == '!' && *(BufferPtr + 1) == '=') {      // !=
            formToken(token, BufferPtr + 2, Token::not_equal);
            break;
        }

        if(*BufferPtr == '<' && *(BufferPtr + 1) == '=') {      // <=
            formToken(token, BufferPtr + 2, Token::less_equal);
            break;
        }

        if(*BufferPtr == '>' && *(BufferPtr + 1) == '=') {      // >=
            formToken(token, BufferPtr + 2, Token::greater_equal);
            break;
        }

        if(charinfo::isOperator(*BufferPtr + 1)) {   // in case of invalid operator like "*->"

            const char *end = BufferPtr + 1;

            while (charinfo::isOperator(*end))
                ++end;

            formToken(token, end, Token::unknown);
            return;
        }

        switch (*BufferPtr) {

            #define CASE(ch, tok) \
            case ch: formToken(token, BufferPtr + 1, tok); break

            CASE('+', Token::plus);
            CASE('-', Token::minus);
            CASE('*', Token::star);
            CASE('/', Token::slash);
            CASE('(', Token::l_paren);
            CASE(')', Token::r_paren);
            CASE(':', Token::colon);
            CASE(',', Token::comma);
            CASE('^', Token::power);
            CASE('>', Token::greater);
            CASE('<', Token::less);
            CASE(';', Token::eoi);

            #undef CASE

            default:
                formToken(token, BufferPtr + 1, Token::unknown);
            }
        return;
        }
    }

void Lexer::formToken(Token &Tok, const char *TokEnd, Token::TokenKind Kind) {

    Tok.Kind = Kind;
    Tok.Text = llvm::StringRef(BufferPtr, TokEnd - BufferPtr);
    BufferPtr = TokEnd;
}