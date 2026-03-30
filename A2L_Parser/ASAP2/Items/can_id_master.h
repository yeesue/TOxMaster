#ifndef CAN_ID_MASTER_H
#define CAN_ID_MASTER_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class CAN_ID_MASTER : public Item
{
public:
    CAN_ID_MASTER(Node *parentNode);
    ~CAN_ID_MASTER();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,CAN_ID_MASTER> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // CAN_ID_MASTER_H
