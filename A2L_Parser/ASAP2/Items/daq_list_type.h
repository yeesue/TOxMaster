#ifndef DAQ_LIST_TYPE_H
#define DAQ_LIST_TYPE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class DAQ_LIST_TYPE : public Item
{
public:
    DAQ_LIST_TYPE(Node *parentNode);
    ~DAQ_LIST_TYPE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item, DAQ_LIST_TYPE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // DAQ_LIST_TYPE_H
