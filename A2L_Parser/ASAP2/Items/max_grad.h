#ifndef MAX_GRAD_H
#define MAX_GRAD_H

#include "item.h"
#include "lexer.h"
#include "basefactory.h"

class MAX_GRAD : public Item
{
    public:
        MAX_GRAD(Node *parentNode);
        ~MAX_GRAD();
        QMap<std::string, std::string> getParameters();
        static Factory<Item,MAX_GRAD> itemFactory;
        char* getPar(std::string str);

    private:
        void parseFixPar(QList<TokenTyp> *typePar);
        QList<TokenTyp> *typePar;
        QList<std::string> *namePar;
        QList<char*> parameters;
};

#endif // MAX_GRAD_H
