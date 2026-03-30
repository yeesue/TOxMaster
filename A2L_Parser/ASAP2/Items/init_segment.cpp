#include "init_segment.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include <QDebug>

//initialise static variables
Factory<Item,INIT_SEGMENT> INIT_SEGMENT::itemFactory;

INIT_SEGMENT::INIT_SEGMENT(Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->init_segment.namePar;
    typePar = &gram->init_segment.typePar;

    //qDebug()<<"In INIT_SEGMENT";

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"INIT_SEGMENT";

    //Page node中的INIT_SEGMENT可能存在导致影响了跳出的token，此步next一�?
    //保证跳出父节点的token正确
    //TokenTyp token = this->nextToken();

    //qDebug()<<"end INIT_SEGMENT";
}

INIT_SEGMENT::~INIT_SEGMENT()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> INIT_SEGMENT::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *INIT_SEGMENT::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void INIT_SEGMENT::parseFixPar(QList<TokenTyp> *typePar)
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
        else if(token == Hex && typePar->at(i) == Integer)
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
