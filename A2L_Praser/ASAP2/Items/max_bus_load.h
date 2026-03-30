#ifndef MAX_BUS_LOAD_H
#define MAX_BUS_LOAD_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class MAX_BUS_LOAD : public Item
{
public:
    MAX_BUS_LOAD(Node *parentNode);
    ~MAX_BUS_LOAD();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,MAX_BUS_LOAD> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // MAX_BUS_LOAD_H
