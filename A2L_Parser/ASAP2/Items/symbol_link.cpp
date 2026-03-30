#include "symbol_link.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include "qdebug.h"

//initialise static variables
Factory<Item,SYMBOL_LINK> SYMBOL_LINK::itemFactory;

SYMBOL_LINK::SYMBOL_LINK(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->symbol_link.namePar;
    typePar = &gram->symbol_link.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"Symbol_link";
}

SYMBOL_LINK::~SYMBOL_LINK()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> SYMBOL_LINK::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *SYMBOL_LINK::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void SYMBOL_LINK::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken();
        if (token == typePar->at(i))
        {
            char *c = new char[parentNode->lex->getLexem().length()+1];
            strcpy(c, parentNode->lex->getLexem().c_str());
            parameters.append(c);
        }
        else
        {
            QString t(this->parentNode->lex->toString(typePar->at(i)).c_str());
            QString s(this->parentNode->lex->toString(token).c_str());
            this->showError("expected token : " + t +"\nfind token : " + s);
            qDebug()<<parentNode->lex->getLexem().c_str();
        }
    }
}
