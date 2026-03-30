#ifndef SECONDARY_SAMPLE_POINT_H
#define SECONDARY_SAMPLE_POINT_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SECONDARY_SAMPLE_POINT : public Item
{
public:
    SECONDARY_SAMPLE_POINT(Node *parentNode);
    ~SECONDARY_SAMPLE_POINT();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,SECONDARY_SAMPLE_POINT> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // SECONDARY_SAMPLE_POINT_H
