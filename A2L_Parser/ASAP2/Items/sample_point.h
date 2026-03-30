#ifndef SAMPLE_POINT_H
#define SAMPLE_POINT_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class SAMPLE_POINT : public Item
{
public:
    SAMPLE_POINT(Node *parentNode);
    ~SAMPLE_POINT();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,SAMPLE_POINT> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // SAMPLE_POINT_H
