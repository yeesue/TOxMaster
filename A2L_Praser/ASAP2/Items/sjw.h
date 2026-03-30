#ifndef SJW_H
#define SJW_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SJW : public Item
{
public:
    SJW(Node *parentNode);
    ~SJW();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,SJW> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // SJW_H
