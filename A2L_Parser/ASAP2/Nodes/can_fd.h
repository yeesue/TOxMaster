#ifndef CAN_FD_H
#define CAN_FD_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Items/max_dlc.h"
#include "Items/can_fd_data_transfer_baudrate.h"
#include "Items/sample_point.h"
#include "Items/sjw.h"
#include "Items/sync_edge.h"
#include "Items/secondary_sample_point.h"
#include "Items/transceiver_delay_compensation.h"
#include "Items/max_dlc_required.h"

class CAN_FD : public Node
{
public:
    CAN_FD(Node *parentNode);
    ~CAN_FD();
    QMap<std::string, std::string> *getParameters();
    static Factory<Node, CAN_FD> nodeFactory;
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

#endif // CAN_FD_H
