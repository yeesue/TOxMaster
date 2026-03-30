// A2ML Lexer - A2ML Grammar Definition Language Lexer
// Part of A2L Parser for TOxMaster

#ifndef A2ML_LEXER_H
#define A2ML_LEXER_H

#include <QString>
#include <QTextStream>
#include <QHash>
#include <string>

// A2ML Token Types
enum A2mlToken {
    A2ML_EOF,
    A2ML_UNKNOWN,
    
    // Keywords
    A2ML_STRUCT,
    A2ML_ENUM,
    A2ML_TAGGEDSTRUCT,
    A2ML_TAGGEDUNION,
    A2ML_BLOCK,
    
    // Data Types
    A2ML_UCHAR,
    A2ML_UINT,
    A2ML_ULONG,
    A2ML_CHAR,          // char[n]
    A2ML_FLOAT,
    A2ML_DOUBLE,
    A2ML_SBYTE,
    A2ML_SWORD,
    A2ML_SLONG,
    A2ML_UBYTE,
    A2ML_UWORD,
    
    // Symbols
    A2ML_LBRACE,        // {
    A2ML_RBRACE,        // }
    A2ML_LBRACKET,      // [
    A2ML_RBRACKET,      // ]
    A2ML_LPAREN,        // (
    A2ML_RPAREN,        // )
    A2ML_SEMICOLON,     // ;
    A2ML_EQUAL,         // =
    A2ML_COMMA,         // ,
    A2ML_ASTERISK,      // *
    
    // Values
    A2ML_IDENTIFIER,
    A2ML_STRING,        // "..."
    A2ML_INTEGER,
    A2ML_FLOAT_VALUE,
    
    // Comments
    A2ML_COMMENT
};

class A2mlLexer
{
public:
    A2mlLexer(QTextStream &input);
    ~A2mlLexer();
    
    A2mlToken getNextToken();
    std::string getLexeme() const { return lexeme; }
    int getLine() const { return line; }
    void backward();
    
private:
    QTextStream *in;
    std::string lexeme;
    int line;
    int position;
    char peekChar;
    bool hasPeek;
    
    QHash<QString, A2mlToken> keywords;
    
    void initKeywords();
    A2mlToken scanToken();
    A2mlToken scanIdentifier(char firstChar);
    A2mlToken scanNumber(char firstChar);
    A2mlToken scanString();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    char readChar();
    char peek();
    bool isLetter(char c);
    bool isDigit(char c);
    bool isWhitespace(char c);
};

#endif // A2ML_LEXER_H
