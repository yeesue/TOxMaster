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

#ifndef A2ML_H
#define A2ML_H

#include "node.h"
#include "basefactory.h"
#include <memory>

// Forward declaration
class A2mlStruct;

class A2ML : public Node
{
    public:
        A2ML(Node *parentNode);
        static Factory<Node, A2ML> nodeFactory;       
        
        // Get the parsed A2ML structure
        std::shared_ptr<A2mlStruct> getA2mlStructure() const { return a2mlStructure; }
        
        // Check if A2ML was parsed successfully
        bool isParsed() const { return parsed; }
        
        // Get raw A2ML content
        QString getRawContent() const { return rawContent; }

    private:
        void parse();
        std::shared_ptr<A2mlStruct> a2mlStructure;
        bool parsed;
        QString rawContent;
};

#endif // BLOCKA2ML_H
