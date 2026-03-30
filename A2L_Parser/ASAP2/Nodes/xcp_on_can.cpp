#include "xcp_on_can.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include <QDebug>

Factory<Node,XCP_ON_CAN> XCP_ON_CAN::nodeFactory;

XCP_ON_CAN::XCP_ON_CAN(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{

    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->xcp_on_can.namePar;
    typePar = &gram->xcp_on_can.typePar;
    factoryOptNode = &gram->xcp_on_can.factoryOptNode;
    factoryOptItem = &gram->xcp_on_can.factoryOptItem;

    //qDebug()<<"In XCP_ON_CAN";

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);

    name = (char*)"XCP_ON_CAN";


    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();

    this->sortChildrensName();
}

XCP_ON_CAN::~XCP_ON_CAN()
{
    delete[] name;
}

QMap<std::string, std::string> *XCP_ON_CAN::getParameters()
{
    QMap<std::string, std::string> *par = new QMap<std::string, std::string>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *XCP_ON_CAN::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void XCP_ON_CAN::parseFixPar(QList<TokenTyp> *typePar)
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

TokenTyp XCP_ON_CAN::parseOptPar()
{
    //opt parameters
     QHash<QString, Occurence> nameOptPar;
     nameOptPar.insert("DAQ", ZeroOrOne);
     nameOptPar.insert("EVENT_CAN_ID_LIST", ZeroOrMore);

     nameOptPar.insert("DAQ_LIST_CAN_ID", ZeroOrMore);

     nameOptPar.insert("CAN_ID_MASTER", ZeroOrOne);
     nameOptPar.insert("CAN_ID_SLAVE", ZeroOrOne);
     nameOptPar.insert("BAUDRATE", ZeroOrOne);
     nameOptPar.insert("SAMPLE_POINT", ZeroOrOne);
     nameOptPar.insert("SAMPLE_RATE", ZeroOrOne);
     nameOptPar.insert("BTL_CYCLES", ZeroOrOne);
     nameOptPar.insert("SJW", ZeroOrOne);
     nameOptPar.insert("SYNC_EDGE", ZeroOrOne);
     nameOptPar.insert("MAX_BUS_LOAD", ZeroOrOne);
     nameOptPar.insert("TRANSPORT_LAYER_INSTANCE", ZeroOrOne);
     nameOptPar.insert("CAN_FD", ZeroOrOne);


    if (nameOptPar.isEmpty())
        return nextToken();
    else
    {
        TokenTyp token = nextToken();
        while (lex->getLexem() != "XCP_ON_CAN")
        {
            //Nodes
            if (token == BlockBegin)
            {
                token = this->nextToken();
                if (token == Keyword)
                {
                    std::string lexem = lex->getLexem();
                    //qDebug()<<"xcp_on_can node lexem = "<<QString::fromStdString(lexem);
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
                //qDebug()<<"xcp_on_can item lexem="<<QString::fromStdString(lexem);
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


