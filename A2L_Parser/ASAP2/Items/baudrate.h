#ifndef BAUDRATE_H
#define BAUDRATE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class BAUDRATE : public Item
{
public:
    BAUDRATE(Node *parentNode);
    ~BAUDRATE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,BAUDRATE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // BAUDRATE_H
