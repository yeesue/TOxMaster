#ifndef MAX_BLOCK_SIZE_H
#define MAX_BLOCK_SIZE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class MAX_BLOCK_SIZE : public Item
{
public:
    MAX_BLOCK_SIZE(Node *parentNode);
    ~MAX_BLOCK_SIZE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,MAX_BLOCK_SIZE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // MAX_BLOCK_SIZE_H
