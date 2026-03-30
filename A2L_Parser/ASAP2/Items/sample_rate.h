#ifndef SAMPLE_RATE_H
#define SAMPLE_RATE_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SAMPLE_RATE : public Item
{
public:
    SAMPLE_RATE(Node *parentNode);
    ~SAMPLE_RATE();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,SAMPLE_RATE> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // SAMPLE_RATE_H
