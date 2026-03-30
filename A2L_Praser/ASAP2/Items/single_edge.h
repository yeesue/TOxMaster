#ifndef SINGLE_EDGE_H
#define SINGLE_EDGE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SINGLE_EDGE : public Item
{
public:
    SINGLE_EDGE(Node *parentNode);
    ~SINGLE_EDGE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,SINGLE_EDGE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};


#endif // SINGLE_EDGE_H
