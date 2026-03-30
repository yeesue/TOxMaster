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

#include "Nodes/a2ml.h"
#include "lexer.h"
#include "a2ml_lexer.h"
#include "a2ml_parser.h"
#include <QTextStream>
#include <QDebug>

Factory<Node, A2ML> A2ML::nodeFactory;

A2ML::A2ML( Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
    , parsed(false)
{
    this->parse();
}

void A2ML::parse()
{
    this->name = (char*)"A2ML";
    this->a2lLine = lex->getLine();
    parsed = false;
    rawContent.clear();

    // Lexer is already positioned AFTER "/begin A2ML"
    // Collect A2ML content until we find "/end A2ML"
    
    TokenTyp token = lex->getNextToken();
    
    while (token != Eof)
    {
        std::string lexem = lex->getLexem();
        
        // Check for block end - this is the termination condition
        if (token == BlockEnd && lexem == "/end")
        {
            // Peek next token to see if it's A2ML
            token = lex->getNextToken();
            lexem = lex->getLexem();
            // A2ML is a Keyword token, not Identifier
            if ((token == Keyword || token == Identifier) && lexem == "A2ML")
            {
                // Found /end A2ML - we're done
                break;
            }
            else
            {
                // Not /end A2ML, this is part of A2ML content (e.g., enum value)
                rawContent += "/end ";
                if (token == Identifier || token == Keyword)
                {
                    rawContent += QString::fromStdString(lexem) + " ";
                }
            }
        }
        else
        {
            // Build A2ML content string
            switch (token)
            {
                case Identifier:
                case Keyword:
                case DataType:
                case Datasize:
                case Type:
                case Attribute:
                    rawContent += QString::fromStdString(lexem) + " ";
                    break;
                    
                case String:
                    rawContent += "\"" + QString::fromStdString(lexem) + "\" ";
                    break;
                    
                case Integer:
                case Float:
                case Hex:
                    rawContent += QString::fromStdString(lexem) + " ";
                    break;
                    
                case BlockBegin:
                    // This shouldn't happen in A2ML content, but handle it
                    rawContent += "/begin ";
                    break;
                    
                case Comment:
                    // Skip comments, add newline for readability
                    rawContent += "\n";
                    break;
                    
                default:
                    // Handle punctuation and other symbols
                    if (lexem == "{")
                    {
                        rawContent += "{\n";
                    }
                    else if (lexem == "}")
                    {
                        rawContent += "}\n";
                    }
                    else if (lexem == ";")
                    {
                        rawContent += ";\n";
                    }
                    else if (lexem == "=" || lexem == "," || lexem == "[" || lexem == "]" ||
                             lexem == "(" || lexem == ")" || lexem == "*" || lexem == ":")
                    {
                        rawContent += QString::fromStdString(lexem);
                    }
                    else
                    {
                        // Unknown token - try to include it
                        rawContent += QString::fromStdString(lexem) + " ";
                    }
                    break;
            }
        }
        
        token = lex->getNextToken();
    }
    
    // Skip A2ML parsing for now - just store raw content
    // The A2ML parser needs more work to handle complex cases
    parsed = true;
    qDebug() << "A2ML content collected, length:" << rawContent.length();
}

