#ifndef SEGMENT_H
#define SEGMENT_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Nodes/checksum.h"
#include "Nodes/page.h"

class SEGMENT : public Node
{
public:
    SEGMENT(Node *parentNode);
    ~SEGMENT();
    QMap<std::string, std::string> *getParameters();
    static Factory<Node, SEGMENT> nodeFactory;
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

#endif // SEGMENT_H
