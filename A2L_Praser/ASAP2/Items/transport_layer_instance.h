#ifndef TRANSPORT_LAYER_INSTANCE_H
#define TRANSPORT_LAYER_INSTANCE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class TRANSPORT_LAYER_INSTANCE : public Item
{
public:
    TRANSPORT_LAYER_INSTANCE(Node *parentNode);
    ~TRANSPORT_LAYER_INSTANCE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,TRANSPORT_LAYER_INSTANCE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // TRANSPORT_LAYER_INSTANCE_H
