#ifndef FIXED_H
#define FIXED_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class FIXED : public Item
{
public:
    FIXED(Node *parentNode);
    ~FIXED();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,FIXED> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // FIXED_H
