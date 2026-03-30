// A2ML Parser Implementation
// Part of A2L Parser for TOxMaster

#include "a2ml_parser.h"
#include <QDebug>

// ==================== A2mlDataType ====================

QString A2mlDataType::getTypeName() const
{
    static const char* typeNames[] = {
        "uchar", "uint", "ulong",
        "sbyte", "sword", "slong",
        "float", "double",
        "char", "ubyte", "uword"
    };
    
    QString result = typeNames[static_cast<int>(primitiveType)];
    if (arraySize > 0) {
        result += QString("[%1]").arg(arraySize);
    }
    return result;
}

QString A2mlDataType::toString(int indent) const
{
    return indentStr(indent) + getTypeName();
}

// ==================== A2mlEnumValue ====================

QString A2mlEnumValue::toString(int indent) const
{
    return indentStr(indent) + QString("\"%1\" = %2").arg(name).arg(value);
}

// ==================== A2mlMember ====================

QString A2mlMember::toString(int indent) const
{
    QString result = indentStr(indent);
    if (type) {
        result += type->toString();
    }
    if (!name.isEmpty()) {
        result += " " + name;
    }
    result += ";";
    return result;
}

// ==================== A2mlTaggedMember ====================

QString A2mlTaggedMember::toString(int indent) const
{
    QString result = indentStr(indent);
    if (repeated) {
        result += "(";
    }
    result += QString("\"%1\" ").arg(tag);
    if (type) {
        result += type->toString();
    }
    if (repeated) {
        result += ")*";
    } else {
        result += ";";
    }
    return result;
}

// ==================== A2mlStruct ====================

QString A2mlStruct::toString(int indent) const
{
    QString result = indentStr(indent) + "struct";
    if (!name.isEmpty()) {
        result += " " + name;
    }
    result += " {\n";
    
    for (const auto &member : members) {
        result += member->toString(indent + 1) + "\n";
    }
    
    result += indentStr(indent) + "};";
    return result;
}

// ==================== A2mlEnum ====================

QString A2mlEnum::toString(int indent) const
{
    QString result = indentStr(indent) + "enum";
    if (!name.isEmpty()) {
        result += " " + name;
    }
    result += " {\n";
    
    bool first = true;
    for (const auto &value : values) {
        if (!first) {
            result += ",\n";
        }
        result += value->toString(indent + 1);
        first = false;
    }
    
    result += "\n" + indentStr(indent) + "};";
    return result;
}

// ==================== A2mlTaggedStruct ====================

QString A2mlTaggedStruct::toString(int indent) const
{
    QString result = indentStr(indent) + "taggedstruct";
    if (!name.isEmpty()) {
        result += " " + name;
    }
    result += " {\n";
    
    for (const auto &member : members) {
        result += member->toString(indent + 1) + "\n";
    }
    
    result += indentStr(indent) + "};";
    return result;
}

// ==================== A2mlTaggedUnion ====================

QString A2mlTaggedUnion::toString(int indent) const
{
    QString result = indentStr(indent) + "taggedunion";
    if (!name.isEmpty()) {
        result += " " + name;
    }
    result += " {\n";
    
    for (const auto &member : members) {
        result += member->toString(indent + 1) + "\n";
    }
    
    result += indentStr(indent) + "};";
    return result;
}

// ==================== A2mlBlock ====================

QString A2mlBlock::toString(int indent) const
{
    QString result = indentStr(indent) + QString("block \"%1\" ").arg(name);
    if (type) {
        result += type->toString();
    }
    return result;
}

// ==================== A2mlParser ====================

A2mlParser::A2mlParser(A2mlLexer &lexer)
    : lexer(lexer)
    , currentToken(A2ML_EOF)
    , error(false)
    , errorLine(0)
{
    nextToken();
}

A2mlParser::~A2mlParser()
{
}

void A2mlParser::nextToken()
{
    currentToken = lexer.getNextToken();
    currentLexeme = lexer.getLexeme();
}

void A2mlParser::expect(A2mlToken expected)
{
    if (currentToken != expected) {
        setError(QString("Expected token %1, got %2 at line %3")
                 .arg(static_cast<int>(expected))
                 .arg(static_cast<int>(currentToken))
                 .arg(lexer.getLine()));
    }
    nextToken();
}

void A2mlParser::setError(const QString &msg)
{
    error = true;
    errorMessage = msg;
    errorLine = lexer.getLine();
    qWarning() << "A2ML Parser Error:" << msg;
}

std::shared_ptr<A2mlStruct> A2mlParser::parse()
{
    auto root = std::make_shared<A2mlStruct>();
    
    while (currentToken != A2ML_EOF && !error) {
        auto type = parseType();
        if (type) {
            root->addMember(type);
        }
    }
    
    return root;
}

std::shared_ptr<A2mlNode> A2mlParser::parseType()
{
    switch (currentToken) {
        case A2ML_STRUCT:
            return parseStruct();
            
        case A2ML_ENUM:
            return parseEnum();
            
        case A2ML_TAGGEDSTRUCT:
            return parseTaggedStruct();
            
        case A2ML_TAGGEDUNION:
            return parseTaggedUnion();
            
        case A2ML_BLOCK:
            return parseBlock();
            
        case A2ML_SEMICOLON:
            // Empty member
            nextToken();
            return nullptr;
            
        default:
            // Could be a data type
            return parseDataType();
    }
}

std::shared_ptr<A2mlStruct> A2mlParser::parseStruct()
{
    expect(A2ML_STRUCT);
    
    auto str = std::make_shared<A2mlStruct>();
    expect(A2ML_LBRACE);
    
    while (currentToken != A2ML_RBRACE && !error) {
        auto member = parseMemberType();
        if (member) {
            str->addMember(member);
        }
    }
    
    expect(A2ML_RBRACE);
    
    // Optional semicolon
    if (currentToken == A2ML_SEMICOLON) {
        nextToken();
    }
    
    return str;
}

std::shared_ptr<A2mlEnum> A2mlParser::parseEnum()
{
    expect(A2ML_ENUM);
    auto en = std::make_shared<A2mlEnum>();
    
    expect(A2ML_LBRACE);
    
    bool first = true;
    while (currentToken != A2ML_RBRACE && !error) {
        if (!first) {
            expect(A2ML_COMMA);
        }
        first = false;
        
        if (currentToken == A2ML_STRING) {
            QString name = QString::fromStdString(currentLexeme);
            nextToken();
            expect(A2ML_EQUAL);
            
            if (currentToken == A2ML_INTEGER) {
                int value = QString::fromStdString(currentLexeme).toInt();
                en->addValue(name, value);
                nextToken();
            }
        }
    }
    
    expect(A2ML_RBRACE);
    
    // Optional semicolon
    if (currentToken == A2ML_SEMICOLON) {
        nextToken();
    }
    
    return en;
}

std::shared_ptr<A2mlTaggedStruct> A2mlParser::parseTaggedStruct()
{
    expect(A2ML_TAGGEDSTRUCT);
    auto ts = std::make_shared<A2mlTaggedStruct>();
    
    expect(A2ML_LBRACE);
    
    while (currentToken != A2ML_RBRACE && !error) {
        auto member = parseTaggedMember();
        if (member) {
            ts->addMember(member);
        }
    }
    
    expect(A2ML_RBRACE);
    
    // Optional semicolon
    if (currentToken == A2ML_SEMICOLON) {
        nextToken();
    }
    
    return ts;
}

std::shared_ptr<A2mlTaggedUnion> A2mlParser::parseTaggedUnion()
{
    expect(A2ML_TAGGEDUNION);
    auto tu = std::make_shared<A2mlTaggedUnion>();
    
    expect(A2ML_LBRACE);
    
    while (currentToken != A2ML_RBRACE && !error) {
        auto member = parseTaggedMember();
        if (member) {
            tu->addMember(member);
        }
    }
    
    expect(A2ML_RBRACE);
    
    // Optional semicolon
    if (currentToken == A2ML_SEMICOLON) {
        nextToken();
    }
    
    return tu;
}

std::shared_ptr<A2mlBlock> A2mlParser::parseBlock()
{
    expect(A2ML_BLOCK);
    
    QString name;
    if (currentToken == A2ML_STRING) {
        name = QString::fromStdString(currentLexeme);
        nextToken();
    }
    
    auto type = parseType();
    
    return std::make_shared<A2mlBlock>(name, type);
}

std::shared_ptr<A2mlDataType> A2mlParser::parseDataType()
{
    A2mlDataType::PrimitiveType primType;
    
    switch (currentToken) {
        case A2ML_UCHAR:   primType = A2mlDataType::UChar; break;
        case A2ML_UINT:    primType = A2mlDataType::UInt; break;
        case A2ML_ULONG:   primType = A2mlDataType::ULong; break;
        case A2ML_SBYTE:   primType = A2mlDataType::SByte; break;
        case A2ML_SWORD:   primType = A2mlDataType::SWord; break;
        case A2ML_SLONG:   primType = A2mlDataType::SLong; break;
        case A2ML_FLOAT:   primType = A2mlDataType::Float; break;
        case A2ML_DOUBLE:  primType = A2mlDataType::Double; break;
        case A2ML_UBYTE:   primType = A2mlDataType::UByte; break;
        case A2ML_UWORD:   primType = A2mlDataType::UWord; break;
        case A2ML_CHAR:
            primType = A2mlDataType::Char;
            break;
        default:
            setError(QString("Unexpected token for data type: %1").arg(currentToken));
            return nullptr;
    }
    
    nextToken();
    
    int arraySize = -1;
    
    // Check for array size: [n]
    if (currentToken == A2ML_LBRACKET) {
        nextToken();
        if (currentToken == A2ML_INTEGER) {
            arraySize = QString::fromStdString(currentLexeme).toInt();
            nextToken();
        }
        expect(A2ML_RBRACKET);
    }
    
    // Expect semicolon
    if (currentToken == A2ML_SEMICOLON) {
        nextToken();
    }
    
    return std::make_shared<A2mlDataType>(primType, arraySize);
}

std::shared_ptr<A2mlTaggedMember> A2mlParser::parseTaggedMember()
{
    bool repeated = false;
    
    // Check for repeated member: ( ... )*
    if (currentToken == A2ML_LPAREN) {
        repeated = true;
        nextToken();
    }
    
    QString tag;
    if (currentToken == A2ML_STRING) {
        tag = QString::fromStdString(currentLexeme);
        nextToken();
    } else {
        setError("Expected tag string in taggedstruct/taggedunion");
        return nullptr;
    }
    
    auto type = parseType();
    
    if (repeated) {
        expect(A2ML_RPAREN);
        if (currentToken == A2ML_ASTERISK) {
            nextToken();
        }
    }
    
    // Optional semicolon
    if (currentToken == A2ML_SEMICOLON) {
        nextToken();
    }
    
    return std::make_shared<A2mlTaggedMember>(tag, type, repeated);
}

std::shared_ptr<A2mlNode> A2mlParser::parseMemberType()
{
    switch (currentToken) {
        case A2ML_STRUCT:
            return parseStruct();
        case A2ML_ENUM:
            return parseEnum();
        case A2ML_TAGGEDSTRUCT:
            return parseTaggedStruct();
        case A2ML_TAGGEDUNION:
            return parseTaggedUnion();
        case A2ML_BLOCK:
            return parseBlock();
        default:
            return parseDataType();
    }
}
