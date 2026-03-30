#include "dbcparser.h"
#include "common.h"
#include <QFile>
#include <QRegExp>
#include <QDebug>

DbcParser::DbcParser()
{

}

DbcParser::~DbcParser()
{

}

bool DbcParser::doParsering(const QString &file){
    QFile fileVal(file);
    if (!fileVal.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    m_cluster = new CLUSTER_();
    m_cluster->m_name = "Cluster";

    QList<VAL_TABLE> valTblList;
    QList<PERIOD> periodList;
    QList<TX_TYPE> txTypeList;
    QList<SG_START_VALUE> sgStartValueList;

    QStringList context;
    //VAL_TABLE_ ([0-9a-zA-Z_]+) (\d{0,8} "[\S ]+" )+;
    QRegExp rx_enum(tr("VAL_TABLE_ ([0-9a-zA-Z_]+) (\\d{0,8} \"[\\S ]+\" )+;"));
    //BO_ \d{1,4} \S+: \d{1} Vector__XXX
    QRegExp rx_frame(tr("BO_ (\\d{1,4}) (\\S+): (\\d{1,2}) ([\\S]+)"));
    //SG_ \S+ : \d{1,2}\|\d{1,2}@[01]{1}\+ \([0-9.]+,[0-9.]+\) \[\d{1,8}\|\d{1,8}\] "\S{0,}" Vector__XXX
    //SG_ (\S+) : (\d{1,2})\|(\d{1,2})@([01]{1})\+ \(([0-9.\-]+),([0-9.\-]+)\) \[([0-9.\-]+)\|([0-9.\-]+)\] \"(\S{0,})\" Vector__XXX
    QRegExp rx_item(tr("SG_ (\\S+)( [mM]\\d{1,4}){0,1} : (\\d{1,2})\\|(\\d{1,2})@([01]{1})[\\+\\-] \\(([0-9.\\-eE\\+]+),([0-9.\\-eE\\+]+)\\) \\[([0-9.\\-eE\\+]+)\\|([0-9.\\-eE\\+]+)\\] \"([\\S ]{0,})\" (Vector__XXX| [\\S]+)"));
    //BA_ "GenMsgCycleTime" BO_ 657 10;
    QRegExp rx_item_2nd(tr("SG_ (\\S+)( [mM]\\d{1,4}){0,1} : (\\d{1,3})\\|(\\d{1,2})@([01]{1})([\\+\\-]) \\(([0-9.\\-eE\\+]+),([0-9.\\-eE\\+]+)\\) \\[([0-9.\\-eE\\+]+)\\|([0-9.\\-eE\\+]+)\\] \"(.*)\" (.*)"));
    //QRegExp rx_item_3rd(tr("SG_ (\\S+)( [mM]\\d{1,4}){0,1} : (\\d{1,3})\\|(\\d{1,2})@([01]{1})([\\+\\-]) \\(([0-9.\\-eE\\+]+),([0-9.\\-eE\\+]+)\\) \\[([0-9.\\-eE\\+]+)\\|([0-9.\\-eE\\+]+)\\] \"(.*)\" ([\\S]+)"));

    QRegExp rx_period(tr("BA_ \"GenMsgCycleTime\" BO_ (\\d+)[\\s]{1,}(\\d+);"));
    QRegExp rx_tx_type(tr("BA_ \"GenMsgSendType\" BO_ (\\d+)[\\s]{1,}(\\d+);"));
    QRegExp rx_sigStartValue(tr("BA_ \"GenSigStartValue\" SG_ (\\d+) (\\S+) (\\d+);"));
    QRegExp rx_busType(tr("BA_DEF_DEF_ \"BusType\" \"(CAN|CAN FD)\""));
    QRegExp rx_canfdBrs(tr("BA_DEF_DEF_ \"CANFD_BRS\" \"(0|1)\""));

    while (!fileVal.atEnd()) {
        //context.append(fileVal.readLine());
        //process_line(line);

        QString line = fileVal.readLine();
        //qDebug()<<"line:"<<line;
        if(line.size()<=1){
            continue;
        }
        //int pos = rx_frame.indexIn(line);
        if (rx_frame.indexIn(line) > -1) {
            QStringList mathstr = rx_frame.capturedTexts();
            //qDebug()<<"cnt:"<<mathstr.size();
            //int len = mathstr.size();
            //QString output;
            //for(int i=1;i<len;i++){
            //    output+=mathstr.at(i)+" ";
            //}
            //qDebug()<<output;
            BO_ *tempBo = new BO_();
            tempBo->m_id = static_cast<quint16>(mathstr.at(1).toInt());
            tempBo->m_name = mathstr.at(2);
            tempBo->m_length = static_cast<quint8>(mathstr.at(3).toInt());
            tempBo->m_trans = mathstr.at(4);

            while(!fileVal.atEnd()){
                QString line_item = fileVal.readLine();
                //qDebug()<<line_item;
                int pos = rx_item_2nd.indexIn(line_item);
                if (pos > -1) {
                    QStringList mathstr = rx_item_2nd.capturedTexts();
                    //qDebug()<<"cnt:"<<mathstr.size();
                    //int len = mathstr.size();
                    //QString output;
                    //for(int i=1;i<len;i++){
                    //    output+=mathstr.at(i)+" ";
                    //}
                    //qDebug()<<output;

                    SG_ *tempSg = new SG_();
                    tempSg->m_name = mathstr.at(1);
                    if(mathstr.at(2).size()>0){
                        tempSg->m_multiplexer = mathstr.at(2).mid(1);
                    }
                    else{
                        tempSg->m_multiplexer = "";
                    }
                    //qDebug()<<"[5]:"<<mathstr.at(5)<<"[6]:"<<mathstr.at(6)<<"[7]:"<<mathstr.at(7);

                    tempSg->m_startBit = static_cast<quint16>(mathstr.at(3).toInt());
                    tempSg->m_bitLen = static_cast<quint8>(mathstr.at(4).toInt());
                    tempSg->m_type = static_cast<quint8>(mathstr.at(5).toInt());     // 0 = moto, 1 = intel
                    tempSg->m_byteorder = tempSg->m_type;
                    QString valueType = mathstr.at(6);
                    if(valueType == "+")
                    {
                        tempSg->m_valType = 1;    // unsigned
                    } else if(valueType == "-")
                    {
                        tempSg->m_valType = 0;    //signed
                    }
                    tempSg->m_factor = mathstr.at(7).toDouble();
                    tempSg->m_offset = mathstr.at(8).toDouble();
                    tempSg->m_min = mathstr.at(9).toDouble();
                    tempSg->m_max = mathstr.at(10).toDouble();
                    tempSg->m_unit = mathstr.at(11);
                    tempSg->m_recv = mathstr.at(12);
                    //tempBo.m_sgList.append(tempSg);

                    if(tempSg->m_type == 0)
                    {
                      quint16 sb_origin = tempSg->m_startBit;
                      tempSg->m_startBit_oth = sb_origin;

                      quint16 sb_real;
                      if(tempBo->m_length <= 8)
                      {
                          sb_real = sbMotoConv(tempSg->m_startBit, tempSg->m_bitLen);
                      }
                      else
                      {
                          sb_real = sbMotoConv_FD(tempSg->m_startBit, tempSg->m_bitLen);
                      }
                      tempSg->m_startBit = sb_real;

                      if(sb_origin/8 == sb_real/8)
                      {
                          tempSg->m_startBit_oth = sb_real;
                      }
                      else
                      {
                          tempSg->m_startBit_oth = sb_origin/8*8 + sb_real%8;
                      }


                      //tempSg->m_startBit_oth = can_format_motorola2intel_sb_lb(sb_real, tempSg->m_bitLen);
                      //qDebug()<<"sg:"<<tempSg->m_name<<"sb_origin:"<<sb_origin<<", lb:"<<tempSg->m_bitLen
                      //       <<", sb_real:"<<sb_real<<", sb_oth:"<<tempSg->m_startBit_oth;
                    }
                    tempBo->m_sgList.append(tempSg);
                }
                else{
                    break;
                }
            }
            m_boList.append(tempBo);
        }
        else if(rx_enum.indexIn(line) > -1){
            QStringList mathstr = rx_enum.capturedTexts();
            //qDebug()<<"cnt:"<<mathstr.size();
            int len = mathstr.size();
            QString output;
            VAL_TABLE tempTbl;
            for(int i=1;i<len;i++){
                if(i>1){
                    int pos = 0;
                    QString str = mathstr.at(i);
                    QRegExp rx_param(tr("(\\d{0,8}) \"([\x0A-\x21\x23-\xff]+)\" "));
                    while ((pos = rx_param.indexIn(str, pos)) != -1) {
                          //qDebug() << rx_param.cap(1) << " " << rx_param.cap(2);
                        //output+=rx_param.cap(1) + " " + rx_param.cap(2);
                        VAL val;
                        val.val = static_cast<quint64>(rx_param.cap(1).toInt());
                        val.name=rx_param.cap(2);
                          tempTbl.m_valList.append(val);
                          pos += rx_param.matchedLength();
                      }
                }
                else{
                    //output+=mathstr.at(i)+" ";
                    tempTbl.m_name = mathstr.at(i);
                }
            }
            valTblList.append(tempTbl);
        }
        else if(rx_period.indexIn(line) > -1){
            //qDebug()<<"period: "<<rx_period.cap(1)<<" "<<rx_period.cap(2);
            PERIOD pd;
            pd.m_id = static_cast<quint16>(rx_period.cap(1).toInt());
            pd.m_period = static_cast<quint16>(rx_period.cap(2).toInt());
            periodList.append(pd);
        }
        else if(rx_tx_type.indexIn(line) > -1){
            //qDebug()<<"tx type: "<<rx_tx_type.cap(1)<<" "<<rx_tx_type.cap(2);
            TX_TYPE tx;
            tx.m_id = static_cast<quint16>(rx_tx_type.cap(1).toInt());
            tx.m_tx_type = static_cast<quint16>(rx_tx_type.cap(2).toInt());
            txTypeList.append(tx);
        }
        else if(rx_sigStartValue.indexIn(line) > -1){
            SG_START_VALUE sgStartValue;
            //qDebug()<<"start Value:"<<rx_sigStartValue.cap(3);
            sgStartValue.m_id = static_cast<quint16>(rx_sigStartValue.cap(1).toInt());
            sgStartValue.sgName = rx_sigStartValue.cap(2);
            sgStartValue.sgStartValue = rx_sigStartValue.cap(3).toDouble();
            sgStartValueList.append(sgStartValue);
        }
        else if(rx_busType.indexIn(line) > -1)
        {
            QString busType = rx_busType.cap(1);
            qDebug()<<"BusType:"<<busType;
            if(busType.contains("CAN FD") && m_cluster->can_io_mode == 0)
            {
                m_cluster->can_io_mode = 1;
            }
            m_cluster->bus_type = busType.contains("CAN FD") ? 1 : 0;

        }
        else if(rx_canfdBrs.indexIn(line) > -1)
        {
            quint8 canfdBrs = static_cast<quint8>(rx_canfdBrs.cap(1).toInt());
            qDebug()<<"CANFD_BRS:"<<canfdBrs;

            if(canfdBrs)
            {
                m_cluster->can_io_mode = 2;
            }
        }
    }
    //QString context = fileVal.readAll();

    for(auto& item:m_boList){
        for(auto& var:item->m_sgList){
            int index_val = findValTblIndex(var->m_name,valTblList);
            if(index_val>=0){
                //qDebug()<<"match name:"<<var.m_name << " index:"<<index;
                for(auto enumvar:valTblList.at(index_val).m_valList){
                    var->m_valList.append(enumvar);
                }
            }
            int index_start_val = findStartValueIndex(var->m_name,sgStartValueList);
            if(index_start_val>=0){
                var->m_value_default = sgStartValueList.value(index_start_val).sgStartValue;
                if(var->m_name.contains("headerID_") || var->m_name.contains("headerDLC_"))
                {
                    var->m_value = var->m_value_default;
                }
            }
        }

        int index_period = findPeriodIndex(item->m_id,periodList);
        if(index_period>=0){
            item->m_period = periodList.at(index_period).m_period;
        }

        int index_tx_type = findSendTypeIndex(item->m_id,txTypeList);
        if(index_tx_type>=0){
            item->m_send_type = txTypeList.at(index_tx_type).m_tx_type;
        }
    }


    m_cluster->m_boList = m_boList;

    return true;
}

int DbcParser::findValTblIndex(QString name,const QList<VAL_TABLE>& valtablist){
    int i=0;
    for(auto item:valtablist){
        if(item.m_name == name){
            return i;
        }
        i++;
    }
    return -1;
}

int DbcParser::findStartValueIndex(QString name, QList<SG_START_VALUE> &startValueList){
    int i=0;
    for(auto item:startValueList){
        if(item.sgName == name){
            return i;
        }
        i++;
    }
    return -1;
}

int DbcParser::findPeriodIndex(quint16 id, const QList<PERIOD> &pdList){
    int i=0;
    for(auto item:pdList){
        if(item.m_id == id){
            return i;
        }
        i++;
    }
    return -1;
}

int DbcParser::findSendTypeIndex(quint16 id, const QList<TX_TYPE> &txList)
{
    int i=0;
    for(auto item:txList){
        if(item.m_id == id){
            return i;
        }
        i++;
    }
    return -1;
}

quint8 DbcParser::sbMotoConv(quint8 sb, quint8 lb)
{
    quint8 lsbbit = 0;
    quint8 lsbbyte = 0;
    quint8 msbbit = 0;
    quint8 msbbyte = 0;

    msbbyte = sb >> 3;
    msbbit = sb & 7;

    if(lb <= msbbit)
    {
        lsbbit = msbbit - lb;
        lsbbyte = msbbyte;
    }
    else
    {
        //lsbbit = 7 - ((lb - msbbit - 1) & 7);
        quint8 leftbit = lb - msbbit - 1;
        quint8 byteOffset = (leftbit % 8 == 0) ? (leftbit >> 3) : (leftbit >> 3 + 1);
        lsbbit = (leftbit % 8 == 0) ? 0 : (7 - leftbit % 8);
        lsbbyte = msbbyte + byteOffset;
    }
    //qDebug()<<"msbbit:"<<msbbit<<", msbbyte:"<<msbbyte<<", lsbbit:"<<lsbbit<<", lsbbyte:"<<lsbbyte;

    quint8 sb_real = 8 * (lsbbyte + 1) - 1 - (7 - lsbbit);

    return sb_real;
}

quint16 DbcParser::sbMotoConv_FD(quint16 sb, quint8 lb)
{
    quint16 lsbbit = 0;
    quint16 lsbbyte = 0;
    quint16 msbbit = 0;
    quint16 msbbyte = 0;
    quint16 sb_real = 0;

    msbbyte = sb >> 3;
    msbbit = sb & 7;

    if(lb <= msbbit)
    {
        lsbbit = msbbit - lb + 1;
        lsbbyte = msbbyte;
    }
    else
    {        
        quint8 leftbit = lb - msbbit - 1;
        quint8 left = leftbit % 8;
        quint8 byteOffset = leftbit >> 3;
        if(left != 0) byteOffset += 1;
        //qDebug()<<"byte offset:"<<byteOffset;
        lsbbit = (left == 0) ? 0 : (8 - left);
        lsbbyte = msbbyte + byteOffset;

    }
    //qDebug()<<"sb:"<<sb<<", lb:"<<lb<<", msbbit:"<<msbbit<<", msbbyte:"<<msbbyte<<", lsbbit:"<<lsbbit<<", lsbbyte:"<<lsbbyte;

    sb_real = 8 * (lsbbyte + 1) - 1 - (7 - lsbbit);

    return sb_real;
}


CLUSTER_ *DbcParser::getCluster() const
{
    return m_cluster;
}

QList<BO_ *> &DbcParser::getBOList(){
    return m_boList;
}

CPos DbcParser::posCalc(quint8 start, quint8 offset, quint8 format){
    //7,15,23,31,39,47,55,63
    CPos pos;
    int stabyte = start/8;
    int sta = stabyte*8;

    int tempOffset = start+offset-sta;

    int offByte = tempOffset/8;
    pos.bitIndex = tempOffset%8;

    if(format == 1){
        pos.byteIndex = stabyte+offByte;
    }
    else{
        pos.byteIndex = stabyte-offByte;
    }
    return pos;
}

CPos DbcParser::offsetToIndex(quint8 start, quint8 offset, quint8 format){
    CPos pos;
    int stabyte = start/8;
    int sta = stabyte*8+7;
    int len =sta- start + offset;

    int deltByte = len/8;
    pos.bitIndex = len%8;

    if(format == 1){
        pos.byteIndex = sta/8+deltByte;
    }
    else{
        pos.byteIndex = sta/8-deltByte;
    }
    if(pos.byteIndex<0){
        pos.byteIndex = 7;
    }
    else if(pos.byteIndex>7){
        pos.byteIndex = 0;
    }
    return pos;
}

CPos DbcParser::indexToPos(int index){
    CPos pos;
    pos.byteIndex = index/8;
    pos.bitIndex = 7- (index%8);

    return pos;
}

