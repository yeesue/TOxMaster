// A2ML Lexer Implementation
// Part of A2L Parser for TOxMaster

#include "a2ml_lexer.h"
#include <QChar>

A2mlLexer::A2mlLexer(QTextStream &input)
    : in(&input)
    , line(1)
    , position(0)
    , peekChar(0)
    , hasPeek(false)
{
    initKeywords();
}

A2mlLexer::~A2mlLexer()
{
}

void A2mlLexer::initKeywords()
{
    keywords["struct"] = A2ML_STRUCT;
    keywords["enum"] = A2ML_ENUM;
    keywords["taggedstruct"] = A2ML_TAGGEDSTRUCT;
    keywords["taggedunion"] = A2ML_TAGGEDUNION;
    keywords["block"] = A2ML_BLOCK;
    
    // Data types
    keywords["uchar"] = A2ML_UCHAR;
    keywords["uint"] = A2ML_UINT;
    keywords["ulong"] = A2ML_ULONG;
    keywords["char"] = A2ML_CHAR;
    keywords["float"] = A2ML_FLOAT;
    keywords["double"] = A2ML_DOUBLE;
    keywords["sbyte"] = A2ML_SBYTE;
    keywords["sword"] = A2ML_SWORD;
    keywords["slong"] = A2ML_SLONG;
    keywords["ubyte"] = A2ML_UBYTE;
    keywords["uword"] = A2ML_UWORD;
}

char A2mlLexer::readChar()
{
    char c;
    if (hasPeek) {
        c = peekChar;
        hasPeek = false;
    } else {
        if (in->atEnd()) {
            return 0;
        }
        *in >> c;
        if (in->atEnd() && c == 0) {
            return 0;
        }
    }
    position++;
    return c;
}

char A2mlLexer::peek()
{
    if (hasPeek) {
        return peekChar;
    }
    if (in->atEnd()) {
        return 0;
    }
    char c;
    *in >> c;
    if (in->atEnd() && c == 0) {
        return 0;
    }
    peekChar = c;
    hasPeek = true;
    return c;
}

bool A2mlLexer::isLetter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool A2mlLexer::isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool A2mlLexer::isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void A2mlLexer::skipWhitespace()
{
    char c = peek();
    while (isWhitespace(c)) {
        readChar();
        if (c == '\n') {
            line++;
        }
        c = peek();
    }
}

void A2mlLexer::skipLineComment()
{
    char c = readChar(); // skip first '/'
    c = readChar(); // skip second '/'
    while (c != '\n' && c != 0) {
        c = readChar();
    }
    if (c == '\n') {
        line++;
    }
}

void A2mlLexer::skipBlockComment()
{
    readChar(); // skip '/'
    readChar(); // skip '*'
    char c = readChar();
    while (c != 0) {
        if (c == '*') {
            char next = peek();
            if (next == '/') {
                readChar(); // skip '/'
                return;
            }
        }
        if (c == '\n') {
            line++;
        }
        c = readChar();
    }
}

A2mlToken A2mlLexer::scanIdentifier(char firstChar)
{
    lexeme.clear();
    lexeme += firstChar;
    
    char c = peek();
    while (isLetter(c) || isDigit(c)) {
        lexeme += readChar();
        c = peek();
    }
    
    QString id = QString::fromStdString(lexeme).toLower();
    if (keywords.contains(id)) {
        return keywords[id];
    }
    return A2ML_IDENTIFIER;
}

A2mlToken A2mlLexer::scanNumber(char firstChar)
{
    lexeme.clear();
    lexeme += firstChar;
    
    bool isFloat = false;
    bool isNegative = false;
    
    // Handle negative sign
    if (firstChar == '-') {
        isNegative = true;
        char c = peek();
        if (!isDigit(c)) {
            return A2ML_UNKNOWN;
        }
        lexeme += readChar();
    }
    
    // Integer part
    char c = peek();
    while (isDigit(c)) {
        lexeme += readChar();
        c = peek();
    }
    
    // Decimal part
    if (c == '.') {
        isFloat = true;
        lexeme += readChar(); // add '.'
        c = peek();
        while (isDigit(c)) {
            lexeme += readChar();
            c = peek();
        }
    }
    
    // Exponent
    if (c == 'e' || c == 'E') {
        isFloat = true;
        lexeme += readChar();
        c = peek();
        if (c == '+' || c == '-') {
            lexeme += readChar();
            c = peek();
        }
        while (isDigit(c)) {
            lexeme += readChar();
            c = peek();
        }
    }
    
    return isFloat ? A2ML_FLOAT_VALUE : A2ML_INTEGER;
}

A2mlToken A2mlLexer::scanString()
{
    lexeme.clear();
    readChar(); // skip opening quote
    
    char c = readChar();
    while (c != '"' && c != 0) {
        if (c == '\\') {
            // Handle escape sequence
            char next = readChar();
            if (next == 0) break;
            switch (next) {
                case 'n': lexeme += '\n'; break;
                case 't': lexeme += '\t'; break;
                case 'r': lexeme += '\r'; break;
                case '\\': lexeme += '\\'; break;
                case '"': lexeme += '"'; break;
                default: lexeme += next; break;
            }
        } else {
            lexeme += c;
        }
        c = readChar();
    }
    
    return A2ML_STRING;
}

A2mlToken A2mlLexer::scanToken()
{
    skipWhitespace();
    
    if (in->atEnd() && !hasPeek) {
        return A2ML_EOF;
    }
    
    char c = readChar();
    if (c == 0) {
        return A2ML_EOF;
    }
    
    // Single character tokens
    switch (c) {
        case '{': lexeme = "{"; return A2ML_LBRACE;
        case '}': lexeme = "}"; return A2ML_RBRACE;
        case '[': lexeme = "["; return A2ML_LBRACKET;
        case ']': lexeme = "]"; return A2ML_RBRACKET;
        case '(': lexeme = "("; return A2ML_LPAREN;
        case ')': lexeme = ")"; return A2ML_RPAREN;
        case ';': lexeme = ";"; return A2ML_SEMICOLON;
        case '=': lexeme = "="; return A2ML_EQUAL;
        case ',': lexeme = ","; return A2ML_COMMA;
        case '*': lexeme = "*"; return A2ML_ASTERISK;
        case '"': return scanString();
    }
    
    // Comments
    if (c == '/') {
        char next = peek();
        if (next == '/') {
            skipLineComment();
            return A2ML_COMMENT;
        } else if (next == '*') {
            skipBlockComment();
            return A2ML_COMMENT;
        }
        lexeme = "/";
        return A2ML_UNKNOWN;
    }
    
    // Identifier or keyword
    if (isLetter(c)) {
        return scanIdentifier(c);
    }
    
    // Number
    if (isDigit(c) || (c == '-' && isDigit(peek()))) {
        return scanNumber(c);
    }
    
    lexeme = c;
    return A2ML_UNKNOWN;
}

A2mlToken A2mlLexer::getNextToken()
{
    A2mlToken token;
    do {
        token = scanToken();
    } while (token == A2ML_COMMENT); // Skip comments
    
    return token;
}

void A2mlLexer::backward()
{
    // Simple implementation - just note that we need to re-parse
    // Full implementation would require buffering tokens
}
