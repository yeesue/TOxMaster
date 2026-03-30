#include "max_grad.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,MAX_GRAD> MAX_GRAD::itemFactory;

MAX_GRAD::MAX_GRAD(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->max_grad.namePar;
    typePar = &gram->max_grad.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"MAX_GRAD";
}

MAX_GRAD::~MAX_GRAD()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> MAX_GRAD::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *MAX_GRAD::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void MAX_GRAD::parseFixPar(QList<TokenTyp> *typePar)
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
        }
    }
}


