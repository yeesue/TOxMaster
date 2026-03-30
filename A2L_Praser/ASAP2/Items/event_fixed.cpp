#include "event_fixed.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,EVENT_FIXED> EVENT_FIXED::itemFactory;


EVENT_FIXED::EVENT_FIXED(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->event_fixed.namePar;
    typePar = &gram->event_fixed.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);

    /*
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"EVENT_FIXED";
    */
    name = (char*)"EVENT_FIXED";
}

EVENT_FIXED::~EVENT_FIXED()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> EVENT_FIXED::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *EVENT_FIXED::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void EVENT_FIXED::parseFixPar(QList<TokenTyp> *typePar)
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

