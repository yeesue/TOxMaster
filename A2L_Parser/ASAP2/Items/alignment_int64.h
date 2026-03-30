#ifndef ALIGNMENT_INT64_H
#define ALIGNMENT_INT64_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class ALIGNMENT_INT64 : public Item
{
public:
    ALIGNMENT_INT64(Node *parentNode);
    ~ALIGNMENT_INT64();

    QMap<std::string, std::string> getParameters();
    static Factory<Item, ALIGNMENT_INT64> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // ALIGNMENT_INT64_H
