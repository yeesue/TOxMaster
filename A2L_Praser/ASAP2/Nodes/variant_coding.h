#ifndef VARIANT_CODING_H
#define VARIANT_CODING_H

#include "node.h"
#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class VARIANT_CODING : public Node
{
public:
    VARIANT_CODING(Node *parentNode);
    ~VARIANT_CODING();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node,VARIANT_CODING> nodeFactory;
    char* getPar(std::string str);

private:
    // Fix parameters
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
    void parseFixPar(QList<TokenTyp> *typePar);

    // Opt parameters
    QMap<std::string, Occurence> *occOptPar;
    QMap<std::string, FactoryPlant<Node> *>  *factoryOptNode;
    QMap<std::string, FactoryPlant<Item> *>  *factoryOptItem;
    TokenTyp parseOptPar(QMap<std::string, Occurence> *nameOptPar);
};

#endif // VARIANT_CODING_H
