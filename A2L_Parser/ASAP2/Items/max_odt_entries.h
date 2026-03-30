#ifndef MAX_ODT_ENTRIES_H
#define MAX_ODT_ENTRIES_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class MAX_ODT_ENTRIES : public Item
{
public:
    MAX_ODT_ENTRIES(Node *parentNode);
    ~MAX_ODT_ENTRIES();
    QMap<std::string, std::string> getParameters();
    static Factory<Item, MAX_ODT_ENTRIES> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // MAX_ODT_ENTRIES_H
