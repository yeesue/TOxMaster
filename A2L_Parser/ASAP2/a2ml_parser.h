// A2ML Parser - A2ML Grammar Definition Language Parser
// Part of A2L Parser for TOxMaster

#ifndef A2ML_PARSER_H
#define A2ML_PARSER_H

#include "a2ml_lexer.h"
#include <QString>
#include <QList>
#include <QVariant>
#include <QHash>
#include <memory>

// Forward declarations
class A2mlNode;
class A2mlStruct;
class A2mlEnum;
class A2mlTaggedStruct;
class A2mlTaggedUnion;
class A2mlBlock;
class A2mlDataType;
class A2mlMember;

// A2ML AST Node Types
enum class A2mlNodeType {
    Root,
    Struct,
    Enum,
    TaggedStruct,
    TaggedUnion,
    Block,
    DataType,
    EnumValue,
    TaggedMember,
    ArrayType
};

// Base class for A2ML AST nodes
class A2mlNode
{
public:
    A2mlNode(A2mlNodeType type) : nodeType(type) {}
    virtual ~A2mlNode() = default;
    
    A2mlNodeType getNodeType() const { return nodeType; }
    virtual QString toString(int indent = 0) const = 0;
    
protected:
    A2mlNodeType nodeType;
    QString indentStr(int level) const { return QString(level * 2, ' '); }
};

// Data type representation (uchar, uint, char[n], etc.)
class A2mlDataType : public A2mlNode
{
public:
    enum PrimitiveType {
        UChar, UInt, ULong,
        SByte, SWord, SLong,
        Float, Double,
        Char, UByte, UWord
    };
    
    A2mlDataType(PrimitiveType type, int arraySize = -1)
        : A2mlNode(A2mlNodeType::DataType)
        , primitiveType(type)
        , arraySize(arraySize)
    {}
    
    PrimitiveType getPrimitiveType() const { return primitiveType; }
    int getArraySize() const { return arraySize; }
    bool isArray() const { return arraySize > 0; }
    
    QString toString(int indent = 0) const override;
    QString getTypeName() const;
    
private:
    PrimitiveType primitiveType;
    int arraySize;
};

// Enum value: "NAME" = value
class A2mlEnumValue : public A2mlNode
{
public:
    A2mlEnumValue(const QString &name, int value)
        : A2mlNode(A2mlNodeType::EnumValue)
        , name(name)
        , value(value)
    {}
    
    QString getName() const { return name; }
    int getValue() const { return value; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString name;
    int value;
};

// Struct member: type name; or just type;
class A2mlMember : public A2mlNode
{
public:
    A2mlMember(std::shared_ptr<A2mlNode> type, const QString &name = QString())
        : A2mlNode(A2mlNodeType::DataType)
        , type(type)
        , name(name)
    {}
    
    std::shared_ptr<A2mlNode> getType() const { return type; }
    QString getName() const { return name; }
    
    QString toString(int indent = 0) const override;
    
private:
    std::shared_ptr<A2mlNode> type;
    QString name;
};

// Tagged member: "tag" type; or ("tag" type)*
class A2mlTaggedMember : public A2mlNode
{
public:
    A2mlTaggedMember(const QString &tag, std::shared_ptr<A2mlNode> type, bool repeated = false)
        : A2mlNode(A2mlNodeType::TaggedMember)
        , tag(tag)
        , type(type)
        , repeated(repeated)
    {}
    
    QString getTag() const { return tag; }
    std::shared_ptr<A2mlNode> getType() const { return type; }
    bool isRepeated() const { return repeated; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString tag;
    std::shared_ptr<A2mlNode> type;
    bool repeated;
};

// Struct definition
class A2mlStruct : public A2mlNode
{
public:
    A2mlStruct(const QString &name = QString())
        : A2mlNode(A2mlNodeType::Struct)
        , name(name)
    {}
    
    void addMember(std::shared_ptr<A2mlNode> member) { members.append(member); }
    QList<std::shared_ptr<A2mlNode>> getMembers() const { return members; }
    QString getName() const { return name; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString name;
    QList<std::shared_ptr<A2mlNode>> members;
};

// Enum definition
class A2mlEnum : public A2mlNode
{
public:
    A2mlEnum(const QString &name = QString())
        : A2mlNode(A2mlNodeType::Enum)
        , name(name)
    {}
    
    void addValue(const QString &name, int value) { 
        values.append(std::make_shared<A2mlEnumValue>(name, value)); 
    }
    QList<std::shared_ptr<A2mlEnumValue>> getValues() const { return values; }
    QString getName() const { return name; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString name;
    QList<std::shared_ptr<A2mlEnumValue>> values;
};

// TaggedStruct definition
class A2mlTaggedStruct : public A2mlNode
{
public:
    A2mlTaggedStruct(const QString &name = QString())
        : A2mlNode(A2mlNodeType::TaggedStruct)
        , name(name)
    {}
    
    void addMember(std::shared_ptr<A2mlTaggedMember> member) { members.append(member); }
    QList<std::shared_ptr<A2mlTaggedMember>> getMembers() const { return members; }
    QString getName() const { return name; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString name;
    QList<std::shared_ptr<A2mlTaggedMember>> members;
};

// TaggedUnion definition
class A2mlTaggedUnion : public A2mlNode
{
public:
    A2mlTaggedUnion(const QString &name = QString())
        : A2mlNode(A2mlNodeType::TaggedUnion)
        , name(name)
    {}
    
    void addMember(std::shared_ptr<A2mlTaggedMember> member) { members.append(member); }
    QList<std::shared_ptr<A2mlTaggedMember>> getMembers() const { return members; }
    QString getName() const { return name; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString name;
    QList<std::shared_ptr<A2mlTaggedMember>> members;
};

// Block definition: block "name" type
class A2mlBlock : public A2mlNode
{
public:
    A2mlBlock(const QString &name, std::shared_ptr<A2mlNode> type)
        : A2mlNode(A2mlNodeType::Block)
        , name(name)
        , type(type)
    {}
    
    QString getName() const { return name; }
    std::shared_ptr<A2mlNode> getType() const { return type; }
    
    QString toString(int indent = 0) const override;
    
private:
    QString name;
    std::shared_ptr<A2mlNode> type;
};

// A2ML Parser class
class A2mlParser
{
public:
    A2mlParser(A2mlLexer &lexer);
    ~A2mlParser();
    
    // Parse A2ML block content
    std::shared_ptr<A2mlStruct> parse();
    
    // Error handling
    bool hasError() const { return error; }
    QString getErrorMessage() const { return errorMessage; }
    int getErrorLine() const { return errorLine; }
    
private:
    A2mlLexer &lexer;
    A2mlToken currentToken;
    std::string currentLexeme;
    
    bool error;
    QString errorMessage;
    int errorLine;
    
    void nextToken();
    void expect(A2mlToken expected);
    void setError(const QString &msg);
    
    // Parsing methods
    std::shared_ptr<A2mlNode> parseType();
    std::shared_ptr<A2mlStruct> parseStruct();
    std::shared_ptr<A2mlEnum> parseEnum();
    std::shared_ptr<A2mlTaggedStruct> parseTaggedStruct();
    std::shared_ptr<A2mlTaggedUnion> parseTaggedUnion();
    std::shared_ptr<A2mlBlock> parseBlock();
    std::shared_ptr<A2mlDataType> parseDataType();
    std::shared_ptr<A2mlTaggedMember> parseTaggedMember();
    std::shared_ptr<A2mlNode> parseMemberType();
};

#endif // A2ML_PARSER_H
