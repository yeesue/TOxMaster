#include "daq_list.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include "qdebug.h"

Factory<Node,DAQ_LIST> DAQ_LIST::nodeFactory;

DAQ_LIST::DAQ_LIST(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->daq_list.namePar;
    typePar = &gram->daq_list.typePar;
    factoryOptNode = &gram->daq_list.factoryOptNode;
    factoryOptItem = &gram->daq_list.factoryOptItem;

    //opt Parameters
    occOptPar = new QHash<QString, Occurence>;

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);

    name = (char*)"DAQ_LIST";
    /*
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"DAQ";
    */

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();


    //End
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "DAQ_LIST")
        {

        }
        else
        {
            QString s(lex->toString(token).c_str());
            this->showError("expected token : BlockEnd DAQ_LIST\nfind token : " + s );
        }
    }
    else
    {
        QString s1(lex->toString(token).c_str());
        QString s2(lex->getLexem().c_str());
        this->showError("expected end DAQ_LIST\nfind : " + s1 + " " + s2 );
    }

    this->sortChildrensName();
}

DAQ_LIST::~DAQ_LIST()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> *DAQ_LIST::getParameters()
{
    QMap<std::string, std::string> *par = new QMap<std::string, std::string>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *DAQ_LIST::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void DAQ_LIST::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;

    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken();
        if (token == typePar->at(i))
        {
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

TokenTyp DAQ_LIST::parseOptPar()
{
    //opt parameters
     QHash<QString, Occurence> nameOptPar;
     nameOptPar.insert("DAQ_LIST_TYPE", ZeroOrOne);
     nameOptPar.insert("MAX_ODT", ZeroOrOne);
     nameOptPar.insert("MAX_ODT_ENTRIES", ZeroOrOne);
     nameOptPar.insert("FIRST_PID", ZeroOrOne);
     nameOptPar.insert("EVENT_FIXED", ZeroOrOne);

    if (nameOptPar.isEmpty())
        return nextToken();
    else
    {
        TokenTyp token = nextToken();
        while (token == BlockBegin || token == Keyword)
        {
            //Nodes
            if (token == BlockBegin)
            {
                token = this->nextToken();
                if (token == Keyword)
                {
                    std::string lexem = lex->getLexem();
                    if (nameOptPar.contains(lexem.c_str()))
                    {
                        if (nameOptPar.value(lexem.c_str()) == ZeroOrOne)
                        {
                           nameOptPar.insert(lexem.c_str(), Zero);
                           Node  *instance = factoryOptNode->value(lexem)->createInstance(this);
                           this->addChildNode(instance);
                           token = nextToken();
                        }
                        else if (nameOptPar.value(lexem.c_str()) == ZeroOrMore)
                        {
                            Node  *instance = factoryOptNode->value(lexem)->createInstance(this);
                            this->addChildNode(instance);
                            token = nextToken();
                        }
                        else
                        {
                            QString s(lexem.c_str());
                            this->showError(" Keyword : " + s + " can only be once declared");
                            return token;
                        }
                    }
                    else
                    {
                        QString s(lexem.c_str());
                        this->showError("unknown Keyword : " + s );
                        return token;
                    }
                }
                else
                {
                    QString s(lex->toString(token).c_str());
                    this->showError("expected token : BlockBegin or Keyword\nfind token : " + s );
                    return token;
                }
            }
            //Items
            else if (token == Keyword)
            {
                std::string lexem = lex->getLexem();
                //qDebug()<<"daq_list item:"<<lexem.c_str();
                if (nameOptPar.contains(lexem.c_str()))
                {
                    if (nameOptPar.value(lexem.c_str()) == ZeroOrOne)
                    {
                        nameOptPar.insert(lexem.c_str(), Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        token = nextToken();
                    }
                    else if (nameOptPar.value(lexem.c_str()) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        token = nextToken();
                    }
                    else
                    {
                        QString s(lexem.c_str());
                        this->showError(" Keyword : " + s + " can only be declared once");
                        return token;
                    }
                }
                else
                {
                    QString s(lexem.c_str());
                    this->showError("unknown Keyword : " + s );
                    return token;
                }
            }

        }
        return token;
    }
}
