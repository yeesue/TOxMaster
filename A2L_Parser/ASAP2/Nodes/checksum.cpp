#include "checksum.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include <QDebug>

Factory<Node,CHECKSUM> CHECKSUM::nodeFactory;

CHECKSUM::CHECKSUM(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{

    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->checksum.namePar;
    typePar = &gram->checksum.typePar;
    factoryOptNode = &gram->checksum.factoryOptNode;
    factoryOptItem = &gram->checksum.factoryOptItem;

    //qDebug()<<"In CHECKSUM";

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);

    name = (char*)"CHECKSUM";


    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();


    this->sortChildrensName();
}

CHECKSUM::~CHECKSUM()
{
    delete[] name;
}

QMap<std::string, std::string> *CHECKSUM::getParameters()
{
    QMap<std::string, std::string> *par = new QMap<std::string, std::string>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *CHECKSUM::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void CHECKSUM::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;

    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken();
        if (token == typePar->at(i))
        {
            //parameters.append(lex->getLexem());
            char *c = new char[lex->getLexem().length()+1];
            strcpy(c, lex->getLexem().c_str());
            parameters.append(c);
        }
        else
        {
            QString t(lex->toString(typePar->at(i)).c_str());
            QString s(lex->toString(token).c_str());
            this->showError("expected token : " + t +"\nfind token : " + s);
            qDebug()<<parentNode->lex->getLexem().c_str();
        }
    }
}

TokenTyp CHECKSUM::parseOptPar()
{
    //opt parameters
    QHash<QString, Occurence> nameOptPar;
    nameOptPar.insert("MAX_BLOCK_SIZE", ZeroOrOne);

    if (nameOptPar.isEmpty())
        return nextToken();
    else
    {
        TokenTyp token = nextToken();
        while (lex->getLexem() != "CHECKSUM")
        {

            //Nodes
            if (token == BlockBegin)
            {
                token = this->nextToken();
                if (token == Keyword)
                {
                    std::string lexem = lex->getLexem();
                    //qDebug()<<"checksum node lexem = "<<QString::fromStdString(lexem);
                    if (nameOptPar.contains(lexem.c_str()))
                    {
                        if (nameOptPar.value(lexem.c_str()) == ZeroOrOne)
                        {
                           nameOptPar.insert(lexem.c_str(), Zero);
                           Node *instance = factoryOptNode->value(lexem)->createInstance(this);
                           this->addChildNode(instance);
                           //token = nextToken();
                        }

                        else if (nameOptPar.value(lexem.c_str()) == ZeroOrMore)
                        {
                            Node *instance = factoryOptNode->value(lexem)->createInstance(this);
                            this->addChildNode(instance);
                            //token = nextToken();
                        }
                        else
                        {
                            QString s(lexem.c_str());
                            //this->showError(" Keyword : " + s + " can only be once declared");
                            //return token;
                        }
                    }
                    else
                    {
                        QString s(lexem.c_str());
                        //this->showError("unknown Keyword : " + s );
                        //return token;
                    }
                }
                else
                {
                    QString s(lex->toString(token).c_str());
                    //this->showError("expected token : BlockBegin or Keyword\nfind token : " + s );
                    //return token;
                }
            }
            //Items
            else if (token == Keyword)
            {
                std::string lexem = lex->getLexem();
                //qDebug()<<"checksum item lexem="<<QString::fromStdString(lexem);
                if (nameOptPar.contains(lexem.c_str()))
                {
                    if (nameOptPar.value(lexem.c_str()) == ZeroOrOne)
                    {
                        nameOptPar.insert(lexem.c_str(), Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(this);
                        this->addOptItem(instance);
                        //token = nextToken();
                    }
                    else if (nameOptPar.value(lexem.c_str()) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(this);
                        this->addOptItem(instance);
                        //token = nextToken();
                    }
                    else
                    {
                        QString s(lexem.c_str());
                        //this->showError(" Keyword : " + s + " can only be declared once");
                        //return token;
                    }
                }
                else
                {
                    QString s(lexem.c_str());
                    //this->showError("unknown Keyword : " + s );
                    //return token;
                }
            }
            token = nextToken();
        }

        return token;
    }
}


