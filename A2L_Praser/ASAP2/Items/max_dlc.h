#ifndef MAX_DLC_H
#define MAX_DLC_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class MAX_DLC : public Item
{
public:
    MAX_DLC(Node *parentNode);
    ~MAX_DLC();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,MAX_DLC> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // MAX_DLC_H
