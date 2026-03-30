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

#ifndef IF_DATA_H
#define IF_DATA_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Nodes/daq.h"
#include "Nodes/xcp_on_can.h"
#include "Nodes/segment.h"

class IF_DATA : public Node
{
    public:
        IF_DATA(Node *parentNode);
        ~IF_DATA();
        static Factory<Node, IF_DATA> nodeFactory;

    private:
        //void parse();

        // Fix parameters
        QList<TokenTyp> *typePar;
        QList<std::string> *namePar;
        QList<char*> parameters;
        void parseFixPar(QList<TokenTyp> *typePar);

        QHash<QString, Occurence> *occOptPar;
        QMap<std::string, FactoryPlant<Node> *>  *factoryOptNode;
        QMap<std::string, FactoryPlant<Item> *>  *factoryOptItem;
        TokenTyp parseOptPar();
};

#endif // IF_DATA_H
