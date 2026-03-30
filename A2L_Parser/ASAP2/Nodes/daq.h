#ifndef DAQ_H
#define DAQ_H

#include "node.h"
#include "item.h"
#include "lexer.h"
#include "basefactory.h"
#include "Nodes/daq_memory_consumption.h"
#include "Nodes/event.h"
#include "Nodes/daq_list.h"

class DAQ : public Node
{
public:
    DAQ(Node *parentNode);
    ~DAQ();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node, DAQ> nodeFactory;
    char* getPar(std::string str);

private:

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

#endif // DAQ_H
