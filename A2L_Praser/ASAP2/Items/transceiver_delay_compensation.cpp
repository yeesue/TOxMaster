#include "transceiver_delay_compensation.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,TRANSCEIVER_DELAY_COMPENSATION> TRANSCEIVER_DELAY_COMPENSATION::itemFactory;

TRANSCEIVER_DELAY_COMPENSATION::TRANSCEIVER_DELAY_COMPENSATION(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->transceiver_delay_compensation.namePar;
    typePar = &gram->transceiver_delay_compensation.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"TRANSCEIVER_DELAY_COMPENSATION";

}

TRANSCEIVER_DELAY_COMPENSATION::~TRANSCEIVER_DELAY_COMPENSATION()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> TRANSCEIVER_DELAY_COMPENSATION::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *TRANSCEIVER_DELAY_COMPENSATION::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void TRANSCEIVER_DELAY_COMPENSATION::parseFixPar(QList<TokenTyp> *typePar)
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
