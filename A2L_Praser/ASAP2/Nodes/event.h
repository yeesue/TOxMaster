#ifndef EVENT_H
#define EVENT_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"

class EVENT : public Node
{
public:
    EVENT(Node *parentNode);
    ~EVENT();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node, EVENT> nodeFactory;
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

#endif // EVENT_H
