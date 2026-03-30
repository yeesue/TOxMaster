#ifndef CAN_FD_DATA_TRANSFER_BAUDRATE_H
#define CAN_FD_DATA_TRANSFER_BAUDRATE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class CAN_FD_DATA_TRANSFER_BAUDRATE : public Item
{
public:
    CAN_FD_DATA_TRANSFER_BAUDRATE(Node *parentNode);
    ~CAN_FD_DATA_TRANSFER_BAUDRATE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,CAN_FD_DATA_TRANSFER_BAUDRATE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // CAN_FD_DATA_TRANSFER_BAUDRATE_H
