#ifndef DAQ_LIST_H
#define DAQ_LIST_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Items/daq_list_type.h"
#include "Items/max_odt.h"
#include "Items/max_odt_entries.h"
#include "Items/first_pid.h"
#include "Items/event_fixed.h"

class DAQ_LIST : public Node
{
public:
    DAQ_LIST(Node *parentNode);
    ~DAQ_LIST();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node, DAQ_LIST> nodeFactory;
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

#endif // DAQ_LIST_H
