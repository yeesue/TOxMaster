#include "max_odt_entries.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,MAX_ODT_ENTRIES> MAX_ODT_ENTRIES::itemFactory;

MAX_ODT_ENTRIES::MAX_ODT_ENTRIES(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->max_odt_entries.namePar;
    typePar = &gram->max_odt_entries.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    /*
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"MAX_ODT_ENTRIES";
    */
    name = (char*)"MAX_ODT_ENTRIES";

}

MAX_ODT_ENTRIES::~MAX_ODT_ENTRIES()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> MAX_ODT_ENTRIES::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *MAX_ODT_ENTRIES::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void MAX_ODT_ENTRIES::parseFixPar(QList<TokenTyp> *typePar)
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
        else if (token == Integer && typePar->at(i) == Hex)
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
