// HEXplorer is an Asap and HEX file editor
// Copyright (C) 2011  <Christophe Hoel>
//
// This file is part of HEXplorer.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// please contact the author at : christophe.hoel@gmail.com

#include "Nodes/if_data.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include <QDebug>

Factory<Node,IF_DATA> IF_DATA::nodeFactory;

IF_DATA::IF_DATA(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //parse();

    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->if_data.namePar;
    typePar = &gram->if_data.typePar;
    factoryOptNode = &gram->if_data.factoryOptNode;
    factoryOptItem = &gram->if_data.factoryOptItem;

    //opt Parameters
    //occOptPar = new QHash<QString, Occurence>;
    //occOptPar->insert("DAQ", ZeroOrOne);

    //qDebug()<<"In IF_DATA";

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);

    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (char*)"if_data";

    //qDebug()<<"if_data name="<<name;
    //qDebug()<<"typePar count"<<typePar->count();

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();

/*
    //End
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "IF_DATA")
        {

        }
        else
        {
            QString s(lex->toString(token).c_str());
            this->showError("expected token : BlockEnd IF_DATA\nfind token : " + s );
        }
    }
    else
    {
        QString s1(lex->toString(token).c_str());
        QString s2(lex->getLexem().c_str());
        this->showError("expected end IF_DATA\nfind : " + s1 + " " + s2 );
    }
*/

    this->sortChildrensName();

    //qDebug()<<"end IF_DATA";
}

IF_DATA::~IF_DATA()
{
    delete[] name;
}

void IF_DATA::parseFixPar(QList<TokenTyp> *typePar)
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

TokenTyp IF_DATA::parseOptPar()
{
    //opt parameters
     QHash<QString, Occurence> nameOptPar;
     nameOptPar.insert("DAQ", ZeroOrOne);
     nameOptPar.insert("XCP_ON_CAN", ZeroOrMore);
     nameOptPar.insert("SEGMENT", ZeroOrOne);


    if (nameOptPar.isEmpty())
        return nextToken();
    else
    {
        TokenTyp token = nextToken();
        while (lex->getLexem() != "IF_DATA")
        {
            //qDebug()<<"if_data current lexem = "<<QString::fromStdString(lex->getLexem());
            //Nodes
            if (token == BlockBegin)
            {
                token = this->nextToken();
                if (token == Keyword)
                {
                    std::string lexem = lex->getLexem();
                    //qDebug()<<"if_data node lexem = "<<QString::fromStdString(lexem);
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
                            Node  *instance = factoryOptNode->value(lexem)->createInstance(this);
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
                //qDebug()<<"if_data item lexem = "<<QString::fromStdString(lexem);
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

/*
TokenTyp IF_DATA::parseOptPar()
{
    //opt parameters
     QHash<QString, Occurence> nameOptPar;
     nameOptPar.insert("DAQ", ZeroOrOne);


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
                            Node *instance = 0;
                            if (lexem == "AXIS_DESCR")
                            {
                                if (!isChild("AXIS_DESCR"))
                                {
                                    Node *node = new Node(this, lex, errorList);
                                    node->name = (char*)"AXIS_DESCR";
                                    addChildNode(node);
                                    node->_pixmap = "";
                                }
                                instance = factoryOptNode->value(lexem)->createInstance( child("AXIS_DESCR", false));
                                child("AXIS_DESCR", false)->addChildNode(instance);
                            }

                            else
                            {
                                instance = factoryOptNode->value(lexem)->createInstance( this);
                                this->addChildNode(instance);
                            }
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
*/

/*
void IF_DATA::parse()
{

    TokenTyp token = lex->getNextToken();
    if (token == Identifier)
    {
        name = new char[lex->getLexem().length() + 1];
        strcpy(name, lex->getLexem().c_str());
    }
    else
    {
        QString s(lex->toString(token).c_str());
        this->showError("expected token : Identifier\nfind token : " + s);
    }
    this->a2lLine = lex->getLine();

    token = lex->getNextToken();

    while (lex->getLexem() != "IF_DATA")
    {
        token = lex->getNextToken();
    }
}
*/
