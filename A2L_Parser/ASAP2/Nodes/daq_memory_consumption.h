#ifndef DAQ_MEMORY_CONSUMPTION_H
#define DAQ_MEMORY_CONSUMPTION_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"

class DAQ_MEMORY_CONSUMPTION : public Node
{
public:
    DAQ_MEMORY_CONSUMPTION(Node *parentNode);
    ~DAQ_MEMORY_CONSUMPTION();

    QMap<std::string, std::string> *getParameters();
    static Factory<Node, DAQ_MEMORY_CONSUMPTION> nodeFactory;
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

#endif // DAQ_MEMORY_CONSUMPTION_H
