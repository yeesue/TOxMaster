#ifndef EVENT_CAN_ID_LIST_H
#define EVENT_CAN_ID_LIST_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Items/fixed.h"

class EVENT_CAN_ID_LIST : public Node
{
public:
    EVENT_CAN_ID_LIST(Node *parentNode);
    ~EVENT_CAN_ID_LIST();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node, EVENT_CAN_ID_LIST> nodeFactory;
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
#endif // EVENT_CAN_ID_LIST_H
