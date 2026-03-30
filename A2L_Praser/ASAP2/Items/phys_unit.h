#ifndef PHYS_UNIT_H
#define PHYS_UNIT_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class PHYS_UNIT : public Item
{
public:
    PHYS_UNIT(Node *parentNode);
    ~PHYS_UNIT();

    QMap<std::string, std::string> getParameters();
    static Factory<Item,PHYS_UNIT> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // PHYS_UNIT_H
