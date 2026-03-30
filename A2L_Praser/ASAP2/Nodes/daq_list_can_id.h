#ifndef DAQ_LIST_CAN_ID_H
#define DAQ_LIST_CAN_ID_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Items/fixed.h"

class DAQ_LIST_CAN_ID : public Node
{
public:
    DAQ_LIST_CAN_ID(Node *parentNode);
    ~DAQ_LIST_CAN_ID();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node, DAQ_LIST_CAN_ID> nodeFactory;
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


#endif // DAQ_LIST_CAN_ID_H
