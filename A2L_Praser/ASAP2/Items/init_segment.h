#ifndef INIT_SEGMENT_H
#define INIT_SEGMENT_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class INIT_SEGMENT : public Item
{
public:
    INIT_SEGMENT(Node *parentNode);
    ~INIT_SEGMENT();
    QMap<std::string, std::string> getParameters();
    static Factory<Item,INIT_SEGMENT> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // INIT_SEGMENT_H
