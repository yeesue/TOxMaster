#ifndef SYNC_EDGE_H
#define SYNC_EDGE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SYNC_EDGE : public Item
{
public:
    SYNC_EDGE(Node *parentNode);
    ~SYNC_EDGE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,SYNC_EDGE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // SYNC_EDGE_H
