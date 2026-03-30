#ifndef XCP_ON_CAN_H
#define XCP_ON_CAN_H

#include "node.h"
#include "lexer.h"
#include "basefactory.h"
#include "Nodes/daq.h"
#include "Nodes/event_can_id_list.h"
#include "Nodes/daq_list_can_id.h"
#include "Items/can_id_master.h"
#include "Items/can_id_slave.h"
#include "Items/baudrate.h"
#include "Items/sample_point.h"
#include "Items/sample_rate.h"
#include "Items/sjw.h"
#include "Items/btl_cycles.h"
#include "Items/sync_edge.h"
#include "Items/max_bus_load.h"
#include "Items/transport_layer_instance.h"
#include "Nodes/can_fd.h"

class XCP_ON_CAN : public Node
{
public:
    XCP_ON_CAN(Node *parentNode);
    ~XCP_ON_CAN();
    QMap<std::string, std::string> *getParameters();
    static Factory<Node, XCP_ON_CAN> nodeFactory;
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

#endif // XCP_ON_CAN_H
