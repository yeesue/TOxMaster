#include "can_fd_data_transfer_baudrate.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include <QDebug>

//initialise static variables
Factory<Item,CAN_FD_DATA_TRANSFER_BAUDRATE> CAN_FD_DATA_TRANSFER_BAUDRATE::itemFactory;

CAN_FD_DATA_TRANSFER_BAUDRATE::CAN_FD_DATA_TRANSFER_BAUDRATE(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->can_fd_data_transfer_baudrate.namePar;
    typePar = &gram->can_fd_data_transfer_baudrate.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"CAN_FD_DATA_TRANSFER_BAUDRATE";

}

CAN_FD_DATA_TRANSFER_BAUDRATE::~CAN_FD_DATA_TRANSFER_BAUDRATE()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> CAN_FD_DATA_TRANSFER_BAUDRATE::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *CAN_FD_DATA_TRANSFER_BAUDRATE::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void CAN_FD_DATA_TRANSFER_BAUDRATE::parseFixPar(QList<TokenTyp> *typePar)
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
            //qDebug()<<"XCP_ON_CAN can_fd_data_transfer_baudrate="<<QString(c);
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
