#ifndef MAX_ODT_H
#define MAX_ODT_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class MAX_ODT : public Item
{
public:
    MAX_ODT(Node *parentNode);
    ~MAX_ODT();
    QMap<std::string, std::string> getParameters();
    static Factory<Item, MAX_ODT> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // MAX_ODT_H
