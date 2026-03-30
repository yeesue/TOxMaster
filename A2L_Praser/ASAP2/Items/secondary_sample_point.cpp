#include "secondary_sample_point.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,SECONDARY_SAMPLE_POINT> SECONDARY_SAMPLE_POINT::itemFactory;

SECONDARY_SAMPLE_POINT::SECONDARY_SAMPLE_POINT(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->secondary_sample_point.namePar;
    typePar = &gram->secondary_sample_point.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"SECONDARY_SAMPLE_POINT";
}

SECONDARY_SAMPLE_POINT::~SECONDARY_SAMPLE_POINT()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> SECONDARY_SAMPLE_POINT::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *SECONDARY_SAMPLE_POINT::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void SECONDARY_SAMPLE_POINT::parseFixPar(QList<TokenTyp> *typePar)
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
        else if(typePar->at(i) == Hex && token == Integer)
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

