#include "can_id_master.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,CAN_ID_MASTER> CAN_ID_MASTER::itemFactory;

CAN_ID_MASTER::CAN_ID_MASTER(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->can_id_master.namePar;
    typePar = &gram->can_id_master.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"CAN_ID_MASTER";
}

CAN_ID_MASTER::~CAN_ID_MASTER()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> CAN_ID_MASTER::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *CAN_ID_MASTER::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void CAN_ID_MASTER::parseFixPar(QList<TokenTyp> *typePar)
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
