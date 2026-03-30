#ifndef EVENT_FIXED_H
#define EVENT_FIXED_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class EVENT_FIXED : public Item
{
public:
    EVENT_FIXED(Node *parentNode);
    ~EVENT_FIXED();
    QMap<std::string, std::string> getParameters();
    static Factory<Item, EVENT_FIXED> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // EVENT_FIXED_H
