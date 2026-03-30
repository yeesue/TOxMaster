#ifndef TRANSCEIVER_DELAY_COMPENSATION_H
#define TRANSCEIVER_DELAY_COMPENSATION_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class TRANSCEIVER_DELAY_COMPENSATION : public Item
{
public:
    TRANSCEIVER_DELAY_COMPENSATION(Node *parentNode);
    ~TRANSCEIVER_DELAY_COMPENSATION();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,TRANSCEIVER_DELAY_COMPENSATION> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // TRANSCEIVER_DELAY_COMPENSATION_H
