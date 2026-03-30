#ifndef CHECKSUM_H
#define CHECKSUM_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Items/max_block_size.h"

class CHECKSUM : public Node
{
public:
    CHECKSUM(Node *parentNode);
    ~CHECKSUM();
    QMap<std::string, std::string> *getParameters();
    static Factory<Node, CHECKSUM> nodeFactory;
    char* getPar(std::string str);

private:

    // Fix parameters
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
    void parseFixPar(QList<TokenTyp> *typePar);

    QHash<QString, Occurence> *occOptPar;
    QMap<std::string, FactoryPlant<Node> *>  *factoryOptNode;
    QMap<std::string, FactoryPlant<Item> *>  *factoryOptItem;
    TokenTyp parseOptPar();
};

#endif // CHECKSUM_H
