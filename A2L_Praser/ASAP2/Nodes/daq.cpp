#include "Nodes/daq.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include "qdebug.h"


//initialise static variables
Factory<Node,DAQ> DAQ::nodeFactory;

DAQ::DAQ(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->daq.namePar;
    typePar = &gram->daq.typePar;
    factoryOptNode = &gram->daq.factoryOptNode;
    factoryOptItem = &gram->daq.factoryOptItem;

    //opt Parameters
    occOptPar = new QHash<QString, Occurence>;
    //occOptPar->insert("FORMAT", ZeroOrOne);


    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);

    name = (char*)"DAQ";
    /*
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"DAQ";
    */

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();

    /*
    //End
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "DAQ")
        {

        }
        else
        {
            QString s(lex->toString(token).c_str());
            this->showError("expected token : BlockEnd DAQ\nfind token : " + s );
        }
    }
    else
    {
        QString s1(lex->toString(token).c_str());
        QString s2(lex->getLexem().c_str());
        this->showError("expected end DAQ\nfind : " + s1 + " " + s2 );
    }
    */
    this->sortChildrensName();
}

DAQ::~DAQ()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> *DAQ::getParameters()
{
    QMap<std::string, std::string> *par = new QMap<std::string, std::string>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char *DAQ::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

void DAQ::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    //parameters = new QMap<const char*, const char*>;
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
        else if (token == Integer && typePar->at(i) == Hex)
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

TokenTyp DAQ::parseOptPar()
{
    //opt parameters
     QHash<QString, Occurence> nameOptPar;
     nameOptPar.insert("DAQ_MEMORY_CONSUMPTION", ZeroOrOne);
     nameOptPar.insert("DAQ_LIST", ZeroOrMore);
     nameOptPar.insert("EVENT", ZeroOrMore);

    if (nameOptPar.isEmpty())
        return nextToken();
    else
    {
        TokenTyp token = nextToken();
        while (lex->getLexem() != "DAQ")
        {            
            if (token == BlockEnd)
            {
                token = this->nextToken();
                if (token == Keyword && lex->getLexem() == "DAQ")
                {
                    //token = this->nextToken();
                    return token;
                }                
            }
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
                           //token = nextToken();
                        }
                        else if (nameOptPar.value(lexem.c_str()) == ZeroOrMore)
                        {

                            /*
                            Node *instance = 0;
                            if (lexem == "EVENT")
                            {
                                if (!isChild("EVENT"))
                                {
                                    Node *node = new Node(this, lex, errorList);
                                    node->name = (char*)"EVENT";
                                    addChildNode(node);
                                    node->_pixmap = "";
                                }
                                instance = factoryOptNode->value(lexem)->createInstance( child("EVENT", false));
                                child("EVENT", false)->addChildNode(instance);
                            }
                            else if (lexem == "DAQ_LIST")
                            {
                                if (!isChild("DAQ_LIST"))
                                {
                                    Node *node = new Node(this, lex, errorList);
                                    node->name = (char*)"DAQ_LIST";
                                    addChildNode(node);
                                    node->_pixmap = "";
                                }
                                instance = factoryOptNode->value(lexem)->createInstance( child("DAQ_LIST", false));
                                child("DAQ_LIST", false)->addChildNode(instance);
                            }
                            else
                            {
                                instance = factoryOptNode->value(lexem)->createInstance( this);
                                this->addChildNode(instance);
                            }
                            */

                            Node  *instance = factoryOptNode->value(lexem)->createInstance(this);
                            this->addChildNode(instance);

                            //token = nextToken();
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
                if (nameOptPar.contains(lexem.c_str()))
                {
                    if (nameOptPar.value(lexem.c_str()) == ZeroOrOne)
                    {
                        nameOptPar.insert(lexem.c_str(), Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        //token = nextToken();
                    }
                    else if (nameOptPar.value(lexem.c_str()) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        //token = nextToken();
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
                    //this->showError("unknown Keyword : " + s );
                    //return token;
                }
            }
            token = nextToken();
        }
        return token;
    }
}
