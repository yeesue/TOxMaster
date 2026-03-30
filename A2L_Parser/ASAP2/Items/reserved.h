#ifndef RESERVED_H
#define RESERVED_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class RESERVED : public Item
{
public:
    RESERVED(Node *parentNode);
    ~RESERVED();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,RESERVED> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // RESERVED_H
