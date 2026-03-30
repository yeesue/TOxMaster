#ifndef FIRST_PID_H
#define FIRST_PID_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class FIRST_PID : public Item
{
public:
    FIRST_PID(Node *parentNode);
    ~FIRST_PID();
    QMap<std::string, std::string> getParameters();
    static Factory<Item, FIRST_PID> itemFactory;
    char* getPar(std::string str);

private:
    void parseFixPar(QList<TokenTyp> *typePar);
    QList<TokenTyp> *typePar;
    QList<std::string> *namePar;
    QList<char*> parameters;
};

#endif // FIRST_PID_H
