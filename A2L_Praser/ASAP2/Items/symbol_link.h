#ifndef SYMBOL_LINK_H
#define SYMBOL_LINK_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SYMBOL_LINK : public Item
{
public:
    SYMBOL_LINK(Node *parentNode);
    ~SYMBOL_LINK();

    QMap<std::string, std::string> getParameters();
    static Factory<Item,SYMBOL_LINK> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // SYMBOL_LINK_H
