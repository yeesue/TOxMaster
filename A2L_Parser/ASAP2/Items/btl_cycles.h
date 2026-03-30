#ifndef BTL_CYCLES_H
#define BTL_CYCLES_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class BTL_CYCLES : public Item
{
public:
    BTL_CYCLES(Node *parentNode);
    ~BTL_CYCLES();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,BTL_CYCLES> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // BTL_CYCLES_H
