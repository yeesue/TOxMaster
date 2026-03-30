#include "mdf3.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QDateTime>

//#pragma pack(2)

MDF3::MDF3()
{

}


bool MDF3::mdf3_Demo(QString fileName)
{

    bool status = mdf3_createAndOpenFile(fileName);            // create and open file
    if (!status)
        return false;

    status = mdf3_addIDHDToFile(2);                   // add ID HD TX modules
    if (!status)
        return false;

    quint64 curPos = mdf3_File.pos();
    qDebug()<<"mdf3 File add ID HD, current position="<<curPos;

    char *data = new char;
    int size = 0;

    demoDataGen(data, &size, 0.1);

    qDebug()<<"demo dataSize = "<<size;

    SigAttr *sig0 = new SigAttr;
    memset(sig0, 0, sizeof(SigAttr));
    m3ccopy(sig0->sig_shortname, 32, "sig0", true);
    m3ccopy(sig0->sig_desc, 128, "sig0 description", true);

    sig0->sig_cntype = 1;
    sig0->sig_datatype = 3;
    sig0->sig_startbit = 0;
    sig0->sig_lengthbit = 64;
    sig0->sig_min = 0;
    sig0->sig_max = 100;
    sig0->sig_sample_rate = 0.1;

    SigAttr *sig1 = new SigAttr;
    memset(sig1, 0, sizeof(SigAttr));
    m3ccopy(sig1->sig_shortname, 32, "sig1", true);
    m3ccopy(sig1->sig_desc, 128, "sig1 description", true);

    sig1->sig_cntype = 0;
    sig1->sig_datatype = 0;
    sig1->sig_startbit = 64;
    sig1->sig_lengthbit = 8;
    sig1->sig_min = 0;
    sig1->sig_max = 100;
    sig1->sig_sample_rate = 0.1;

    QList<SigAttr*> sigList;
    sigList<<sig0<<sig1;

    status = mdf3_addDGToFile(curPos, data, size, sigList, false);     //add first DG module
    if (!status)
        return false;


    curPos = mdf3_File.pos();
    qDebug()<<"mdf3 File add 1st DG, current position="<<curPos;

    char *data1 = new char;
    int size1 = 0;

    demoDataGen(data1, &size1, 0.01);

    qDebug()<<"demo1 dataSize = "<<size1;

    SigAttr *sig2 = new SigAttr;
    memset(sig2, 0, sizeof(SigAttr));
    m3ccopy(sig2->sig_shortname, 32, "sig2", true);
    m3ccopy(sig2->sig_desc, 128, "sig2 description", true);

    sig2->sig_cntype = 1;
    sig2->sig_datatype = 3;
    sig2->sig_startbit = 0;
    sig2->sig_lengthbit = 64;
    sig2->sig_min = 0;
    sig2->sig_max = 100;
    sig2->sig_sample_rate = 0.01;

    SigAttr *sig3 = new SigAttr;
    memset(sig3, 0, sizeof(SigAttr));
    m3ccopy(sig3->sig_shortname, 32, "sig3", true);
    m3ccopy(sig3->sig_desc, 128, "sig3 description", true);

    sig3->sig_cntype = 0;
    sig3->sig_datatype = 0;
    sig3->sig_startbit = 64;
    sig3->sig_lengthbit = 8;
    sig3->sig_min = 0;
    sig3->sig_max = 100;
    sig3->sig_sample_rate = 0.01;

    QList<SigAttr*> sigList1;
    sigList1<<sig2<<sig3;

    status = mdf3_addDGToFile(curPos, data1, size1, sigList1, true);   // add second DG module
    if (!status)
        return false;

    status = mdf3_closeFile();    // file close
    if (!status)
        return false;
}

bool MDF3::demoDataGen(char *data, int *size, qreal rate)
{
    qreal sig0 = 0;
    quint8 sig1 = 0;
    int rcdSize = sizeof(qreal)+sizeof(quint8);
    QByteArray bytes;
    *size = 0;

    for (int i = 0; i < 100; i++)
    {
        char* tempData = new char[rcdSize];
        sig0 += rate;
        sig1 += 1;
        if (sig1 >= 256)
            sig1 = 0;
        memcpy(tempData, (char*)&sig0, sizeof(qreal));
        memcpy(tempData+sizeof(qreal), (char*)&sig1, sizeof(quint8));

        QByteArray rcdBytes(tempData, rcdSize);
        bytes.append(rcdBytes);
        *size += rcdSize;
    }

    data = bytes.data();

    return true;

}

bool MDF3::mdf3_createAndOpenFile(QString fileName)
{
    if (fileName.isEmpty())
        return false;

    mdf3_FileName = fileName;    
    mdf3_File.setFileName(fileName);
    mdf3_File.remove();

    if(!mdf3_File.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        qDebug()<<"Open MDF3 File "<<fileName<<" Error.";
        return false;
    }

    return true;
}

bool MDF3::mdf3_closeFile()
{
    mdf3_File.flush();
    mdf3_File.close();

    return true;
}



bool MDF3::mdf3_addIDHDToFile(int numDG)
{
    if (!mdf3_File.isOpen())
        return false;

    mdf3FileId idblock;
    memset(&idblock, 0, SIZE_ID);
    m3ccopy(idblock.id_file, 8, "MDF", true);
    m3ccopy(idblock.id_format, 8, "3.30", true);
    m3ccopy(idblock.id_prog, 8, "Prog", true);
    idblock.id_order = 0;
    idblock.id_float = 0;
    idblock.id_ver = 330;
    idblock.id_codepage = 0;

    QDateTime now = QDateTime::currentDateTime();
    QString date_str = now.date().toString("dd:MM:yyyy");
    QString time_str = now.time().toString("HH:mm:ss");
    qint16  time_utcoffset = now.offsetFromUtc()/3600;
    quint64 time_ns = (now.toSecsSinceEpoch()+now.offsetFromUtc()) * 1000000000;

    mdf3HDBlock hdblock;
    memset(&hdblock, 0, SIZE_HD);

    //m3ccopy(hdblock.hd_id, 2, "HD", true);
    QString hdstr = "HD";
    hdblock.hd_size = SIZE_HD;
    hdblock.hd_dg_link = SIZE_ID + SIZE_HD + SIZE_TX;
    hdblock.hd_tx_link = SIZE_ID + SIZE_HD;
    hdblock.hd_pr_link = 0;
    hdblock.hd_num_dg = numDG;
    m3ccopy(hdblock.hd_id, 2, hdstr.toLocal8Bit().data(), true);
    m3ccopy(hdblock.hd_start_date, 10, date_str.toLocal8Bit().data(), true);
    m3ccopy(hdblock.hd_start_time, 8, time_str.toLocal8Bit().data(), true);
    m3ccopy(hdblock.hd_author, 32, "Liu Yunxiao", true);
    m3ccopy(hdblock.hd_org, 32, "SAIC-SEAT-TA", true);
    m3ccopy(hdblock.hd_proj, 32, "SAIC-Project", true);
    m3ccopy(hdblock.hd_subj, 32, "SAIC-Subject", true);
    hdblock.hd_timestamp_ns = time_ns;
    hdblock.hd_time_UTC = time_utcoffset;
    hdblock.hd_time_quality = 0;
    m3ccopy(hdblock.hd_time_id, 32, "Local PC Reference Time", true);

    mdf3TXBlock txblock;
    memset(&txblock, 0, SIZE_TX);

    QString txstr = "TX";
    QString comment = "This is recorded by LYX.";
    txblock.tx_size = SIZE_TX;
    m3ccopy(txblock.tx_id, 2, txstr.toLocal8Bit().data(), true);
    m3ccopy(txblock.tx_comment, 196, comment.toLocal8Bit().data(), true);
    txblock.tx_comment[195] = 0;

    mdf3DGBlock dgblock;
    memset(&dgblock, 0, SIZE_DG);
    QString dgstr = "DG";
    m3ccopy(dgblock.dg_id, 2, dgstr.toLocal8Bit().data(), true);
    dgblock.dg_size = SIZE_DG;
    dgblock.dg_nextdg_link = 0;
    dgblock.dg_cg_link = 0;
    dgblock.dg_data_link = 0;
    dgblock.dg_numcg = 1;
    dgblock.dg_num_rcdid = 0;

    QDataStream out(&mdf3_File);
    //out.setVersion(QDataStream::Qt_5_4);

    int numByte_w = 0;
    numByte_w = out.writeRawData((char*)(&idblock), sizeof(idblock));
    if (numByte_w <= 0)
        return false;
    //qDebug()<<"write ID bytes = "<<numByte_w;

    numByte_w = out.writeRawData((char*)(&hdblock), sizeof(hdblock));
    if (numByte_w <= 0)
        return false;
    //qDebug()<<"write HD bytes = "<<numByte_w;

    numByte_w = out.writeRawData((char*)(&txblock), sizeof(txblock));
    if (numByte_w <= 0)
        return false;
    //qDebug()<<"write TX bytes = "<<numByte_w;

    return true;
}

/*
bool MDF3::mdf3_readFile(QString fileName)
{
    //QFile::remove(fileName);
    QFile mdf3File(fileName);

    if(!mdf3File.open(QIODevice::ReadWrite))
    {
        qDebug()<<"Open MDF3 File "<<fileName<<" Error.";
        return false;
    }

    msleep(2000);

    QDataStream in(&mdf3File);
    quint32 data_read = 0;
    qDebug()<<mdf3File.fileName();

    in.readRawData((char*)&data_read, 4);
    //in.writeRawData((char*)&data_read, 4);

    qDebug()<<data_read;
    mdf3File.flush();
    mdf3File.close();
    return true;
}
*/


bool MDF3::mdf3_addDGToFile(quint64 curFilePos, char *data, int size, QList<SigAttr*> sigList, bool lastDG)
{
    if (!mdf3_File.isOpen() || sigList.isEmpty())
        return false;

    QDataStream out(&mdf3_File);

    int sigCount = sigList.count();
    int cnNum = sigCount;

    quint64 dgPos = curFilePos;

    mdf3DGBlock dgblock;
    memset(&dgblock, 0, SIZE_DG);
    QString dgstr = "DG";
    m3ccopy(dgblock.dg_id, 2, dgstr.toLocal8Bit().data(), true);
    dgblock.dg_size = SIZE_DG;
    dgblock.dg_nextdg_link = 0;
    dgblock.dg_cg_link = dgPos + SIZE_DG;
    dgblock.dg_data_link = 0;
    dgblock.dg_numcg = 1;
    dgblock.dg_num_rcdid = 0;
    dgblock.dg_res = 0;

    quint64 cgPos = dgPos + SIZE_DG;


    mdf3CGBlock cgblock;
    memset(&cgblock, 0, SIZE_CG);
    QString cgstr = "CG";
    m3ccopy(cgblock.cg_id, 2, cgstr.toLocal8Bit().data(), true);
    cgblock.cg_size = SIZE_CG;
    cgblock.cg_nextcg_link = 0;
    cgblock.cg_cn_link = cgPos + SIZE_CG;
    cgblock.cg_tx_link = 0;
    cgblock.cg_rcd_id = 0;
    cgblock.cg_numcn = cnNum;
    cgblock.cg_size_data = 0;
    cgblock.cg_sample = 0;

    quint64 cnPos = cgPos + SIZE_CG;

    int size_rcd = 0;
    QList<mdf3CNBlock> cnblockList;
    for (int i = 0; i < sigCount; i++)
    {
        SigAttr *sig = sigList.at(i);

        mdf3CNBlock cnblock;
        memset(&cnblock, 0, SIZE_CN);
        QString cnstr = "CN";
        m3ccopy(cnblock.cn_id, 2, cnstr.toLocal8Bit().data(), true);
        cnblock.cn_size = SIZE_CN;

        cnblock.cn_nextcn_link = (i == sigCount-1) ? 0 : (cnPos + SIZE_CN);
        cnblock.cn_cc_link = 0;
        cnblock.cn_ce_link = 0;
        cnblock.cn_cdb_link = 0;
        cnblock.cn_tx_link = 0;
        cnblock.cn_range_valid = 1;
        cnblock.cn_longname_link = 0;
        cnblock.cn_displayname_link = 0;
        cnblock.cn_byteoffset = 0;


        cnblock.cn_cn_type = sig->sig_cntype;
        memcpy(cnblock.cn_shortname, sig->sig_shortname, 32);
        cnblock.cn_shortname[31] = 0;
        memcpy(cnblock.cn_sig_desc, sig->sig_desc, 128);
        cnblock.cn_sig_desc[127] = 0;
        cnblock.cn_startbit = sig->sig_startbit;
        cnblock.cn_lengthbit = sig->sig_lengthbit;
        cnblock.cn_datatype = sig->sig_datatype;
        cnblock.cn_min = sig->sig_min;
        cnblock.cn_max = sig->sig_max;
        cnblock.cn_sample_rate = sig->sig_sample_rate;

        cnblockList.append(cnblock);

        size_rcd += (sig->sig_lengthbit / 8 + (sig->sig_lengthbit % 8 > 0 ? 1 : 0));

        cnPos = cnPos + SIZE_CN;
    }
    qDebug()<<"size = "<<size;
    qDebug()<<"size_rcd="<<size_rcd;

    //block info update
    cgblock.cg_size_data = size_rcd;
    cgblock.cg_num_rcd = size/size_rcd;
    dgblock.dg_nextdg_link = lastDG ? 0 : cnPos + size;
    dgblock.dg_data_link = cnPos;

    int numBytes = out.writeRawData((char*)&dgblock, SIZE_DG);
    qDebug()<<"write DG bytes="<<numBytes;

    numBytes = out.writeRawData((char*)&cgblock, SIZE_CG);
    qDebug()<<"write CG bytes="<<numBytes;

    foreach (mdf3CNBlock cnblock, cnblockList) {
        numBytes = out.writeRawData((char*)&cnblock, SIZE_CN);
        qDebug()<<"write CN bytes="<<numBytes;
    }

    numBytes = out.writeRawData(data, size);
    qDebug()<<"write data bytes="<<numBytes;

    return true;
}

bool MDF3::mdf3_addDGToFile(quint64 curFilePos, char *data, int size, QList<PARAM *> pamList, bool lastDG)
{
    if (!mdf3_File.isOpen() || pamList.isEmpty())
        return false;

    QDataStream out(&mdf3_File);

    int pamCount = pamList.count();
    int cnNum = pamCount;

    quint64 dgPos = curFilePos;

    mdf3DGBlock dgblock;
    memset(&dgblock, 0, SIZE_DG);
    QString dgstr = "DG";
    m3ccopy(dgblock.dg_id, 2, dgstr.toLocal8Bit().data(), true);
    dgblock.dg_size = SIZE_DG;
    dgblock.dg_nextdg_link = 0;
    dgblock.dg_cg_link = dgPos + SIZE_DG;
    dgblock.dg_data_link = 0;
    dgblock.dg_numcg = 1;
    dgblock.dg_num_rcdid = 0;
    dgblock.dg_res = 0;

    quint64 cgPos = dgPos + SIZE_DG;


    mdf3CGBlock cgblock;
    memset(&cgblock, 0, SIZE_CG);
    QString cgstr = "CG";
    m3ccopy(cgblock.cg_id, 2, cgstr.toLocal8Bit().data(), true);
    cgblock.cg_size = SIZE_CG;
    cgblock.cg_nextcg_link = 0;
    cgblock.cg_cn_link = cgPos + SIZE_CG;
    cgblock.cg_tx_link = 0;
    cgblock.cg_rcd_id = 0;
    cgblock.cg_numcn = cnNum;
    cgblock.cg_size_data = 0;
    cgblock.cg_sample = 0;

    quint64 cnPos = cgPos + SIZE_CG;

    int size_rcd = 0;
    QList<mdf3CNBlock> cnblockList;
    for (int i = 0; i < pamCount; i++)
    {
        PARAM *pam = pamList.at(i);

        mdf3CNBlock cnblock;
        memset(&cnblock, 0, SIZE_CN);
        QString cnstr = "CN";
        m3ccopy(cnblock.cn_id, 2, cnstr.toLocal8Bit().data(), true);
        cnblock.cn_size = SIZE_CN;

        cnblock.cn_nextcn_link = (i == pamCount-1) ? 0 : (cnPos + SIZE_CN);
        cnblock.cn_cc_link = 0;
        cnblock.cn_ce_link = 0;
        cnblock.cn_cdb_link = 0;
        cnblock.cn_tx_link = 0;
        cnblock.cn_range_valid = 1;
        cnblock.cn_longname_link = 0;
        cnblock.cn_displayname_link = 0;
        cnblock.cn_byteoffset = 0;


        cnblock.cn_cn_type = (i==0) ? 1 : 0;
        memcpy(cnblock.cn_shortname, pam->Name.toLocal8Bit().data(), 32);
        cnblock.cn_shortname[31] = 0;
        memcpy(cnblock.cn_sig_desc, pam->LongIdentifier.toLocal8Bit().data(), 128);
        cnblock.cn_sig_desc[127] = 0;
        cnblock.cn_startbit = (quint16)pam->StartBit;
        cnblock.cn_lengthbit = (quint16)pam->LengthBit;
        cnblock.cn_datatype = (quint16)pam->Type;
        cnblock.cn_min = pam->LowerLimit;
        cnblock.cn_max = pam->UpperLimit;
        cnblock.cn_sample_rate = pam->rate_ms / 1000;   //ms transfer to s

        cnblockList.append(cnblock);

        size_rcd += (pam->LengthBit / 8 + (pam->LengthBit % 8 > 0 ? 1 : 0));

        cnPos = cnPos + SIZE_CN;
    }
    qDebug()<<"size = "<<size;
    qDebug()<<"size_rcd="<<size_rcd;

    //block info update
    cgblock.cg_size_data = size_rcd;
    cgblock.cg_num_rcd = size/size_rcd;
    dgblock.dg_nextdg_link = lastDG ? 0 : cnPos + size;
    dgblock.dg_data_link = cnPos;

    int numBytes = out.writeRawData((char*)&dgblock, SIZE_DG);
    qDebug()<<"write DG bytes="<<numBytes;

    numBytes = out.writeRawData((char*)&cgblock, SIZE_CG);
    qDebug()<<"write CG bytes="<<numBytes;

    foreach (mdf3CNBlock cnblock, cnblockList) {
        numBytes = out.writeRawData((char*)&cnblock, SIZE_CN);
        qDebug()<<"write CN bytes="<<numBytes;
    }

    numBytes = out.writeRawData(data, size);
    qDebug()<<"write data bytes="<<numBytes;

    return true;
}

bool MDF3::mdf3_addDGToFile(quint64 curFilePos, int size, int sizeBlock, QList<PARAM *> pamList, bool lastDG)
{
    if (!mdf3_File.isOpen() || pamList.isEmpty())
        return false;

    QDataStream out(&mdf3_File);

    int pamCount = pamList.count();
    int cnNum = pamCount;

    quint64 dgPos = curFilePos;

    mdf3DGBlock dgblock;
    memset(&dgblock, 0, SIZE_DG);
    QString dgstr = "DG";
    m3ccopy(dgblock.dg_id, 2, dgstr.toLocal8Bit().data(), true);
    dgblock.dg_size = SIZE_DG;
    dgblock.dg_nextdg_link = 0;
    dgblock.dg_cg_link = dgPos + SIZE_DG;
    dgblock.dg_data_link = 0;
    dgblock.dg_numcg = 1;
    dgblock.dg_num_rcdid = 0;
    dgblock.dg_res = 0;

    quint64 cgPos = dgPos + SIZE_DG;


    mdf3CGBlock cgblock;
    memset(&cgblock, 0, SIZE_CG);
    QString cgstr = "CG";
    m3ccopy(cgblock.cg_id, 2, cgstr.toLocal8Bit().data(), true);
    cgblock.cg_size = SIZE_CG;
    cgblock.cg_nextcg_link = 0;
    cgblock.cg_cn_link = cgPos + SIZE_CG;
    cgblock.cg_tx_link = 0;
    cgblock.cg_rcd_id = 0;
    cgblock.cg_numcn = cnNum;
    cgblock.cg_size_data = 0;
    cgblock.cg_sample = 0;

    quint64 cnPos = cgPos + SIZE_CG;

    int size_rcd = sizeBlock;
    QList<mdf3CNBlock> cnblockList;
    QList<mdf3CCBlock> ccblockList;
    for (int i = 0; i < pamCount; i++)
    {
        PARAM *pam = pamList.at(i);

        // CNBLOCK
        mdf3CNBlock cnblock;
        memset(&cnblock, 0, SIZE_CN);
        QString cnstr = "CN";
        m3ccopy(cnblock.cn_id, 2, cnstr.toLocal8Bit().data(), true);
        cnblock.cn_size = SIZE_CN;

        cnblock.cn_nextcn_link = (i == pamCount-1) ? 0 : (cnPos + SIZE_CN + SIZE_CC);
        cnblock.cn_cc_link = cnPos + SIZE_CN;
        cnblock.cn_ce_link = 0;
        cnblock.cn_cdb_link = 0;
        cnblock.cn_tx_link = 0;
        cnblock.cn_range_valid = 1;
        cnblock.cn_longname_link = 0;
        cnblock.cn_displayname_link = 0;
        cnblock.cn_byteoffset = 0;


        cnblock.cn_cn_type = (i==0) ? 1 : 0;
        memcpy(cnblock.cn_shortname, pam->Name.toLocal8Bit().data(), 32);
        cnblock.cn_shortname[31] = 0;
        memcpy(cnblock.cn_sig_desc, pam->LongIdentifier.toLocal8Bit().data(), 128);
        cnblock.cn_sig_desc[127] = 0;
        cnblock.cn_startbit = (quint16)pam->StartBit;
        cnblock.cn_lengthbit = (quint16)pam->LengthBit;
        cnblock.cn_datatype = (quint16)pam->Type;
        cnblock.cn_min = pam->LowerLimit;
        cnblock.cn_max = pam->UpperLimit;
        //cnblock.cn_sample_rate = pam->rate_ms / 1000;
        cnblock.cn_sample_rate = 0;   //cn_val_raster

        cnblockList.append(cnblock);
        //qDebug()<<"current nedpam:"<<pam->nedPam;

        if(!pam->isNedPam())
        {
            if(pam->ByteOrder == 0)
            {
                //size_rcd += (pam->LengthBit / 8 + (pam->LengthBit % 8 > 0 ? 1 : 0));
            }
            else
            {
                //size_rcd = pam->smSize;
            }
            //qDebug()<<"current size_rcd="<<size_rcd;
        }

        cnPos = cnPos + SIZE_CN;


        // CCBLOCK
        mdf3CCBlock ccblock;
        memset(&ccblock, 0, SIZE_CC);
        QString ccstr = "CC";
        m3ccopy(ccblock.cc_id, 2, ccstr.toLocal8Bit().data(), true);
        ccblock.cc_size = SIZE_CC;

        ccblock.cc_phy_rangevalid = 1;
        ccblock.cc_phy_min = pam->LowerLimit;
        ccblock.cc_phy_max = pam->UpperLimit;
        memcpy(ccblock.cc_phy_unit, pam->Unit.toLocal8Bit().data(), 20);
        ccblock.cc_convtype = 0;
        /*
        0 = parametric, linear
        1 = tabular with interpolation
        2 = tabular
        6 = polynomial function
        7 = exponential function
        8 = logarithmic function
        9 = rational conversion formula
        10 = ASAM-MCD2 Text formula
        11 = ASAM-MCD2 Text Table, (COMPU_VTAB)
        12 = ASAM-MCD2 Text Range Table (COMPU_VTAB_RANGE)
        132 = date (Based on 7 Byte Date data structure)
        133 = time (Based on 6 Byte Time data structure)
        65535 = 1:1 conversion formula (Int = Phys)
        */

        ccblock.cc_addsize_info = 2; //number of parameter :for linear conversion
        ccblock.cc_variables[0] = pam->ScalingOffset;
        ccblock.cc_variables[1] = pam->ScalingFactor;

        ccblockList.append(ccblock);

        cnPos += SIZE_CC;

    }
    qDebug()<<"size = "<<size;
    qDebug()<<"size_rcd="<<size_rcd;

    //block info update
    cgblock.cg_size_data = size_rcd;
    cgblock.cg_num_rcd = size/size_rcd;
    dgblock.dg_nextdg_link = lastDG ? 0 : cnPos + size;
    dgblock.dg_data_link = cnPos;

    int numBytes = out.writeRawData((char*)&dgblock, SIZE_DG);
    //qDebug()<<"write DG bytes="<<numBytes;

    numBytes = out.writeRawData((char*)&cgblock, SIZE_CG);
    //qDebug()<<"write CG bytes="<<numBytes;

    for(int i = 0; i < cnblockList.count(); i++)
    {
        mdf3CNBlock cnblock = cnblockList.at(i);
        mdf3CCBlock ccblock = ccblockList.at(i);

        numBytes = out.writeRawData((char*)&cnblock, SIZE_CN);
        //qDebug()<<"write CN bytes="<<numBytes;

        numBytes = out.writeRawData((char*)&ccblock, SIZE_CC);
        //qDebug()<<"write CC bytes="<<numBytes;

    }

    /*
    foreach (mdf3CNBlock cnblock, cnblockList) {
        numBytes = out.writeRawData((char*)&cnblock, SIZE_CN);
        qDebug()<<"write CN bytes="<<numBytes;
    }
    */

    //numBytes = out.writeRawData(data, size);
    //qDebug()<<"write data bytes="<<numBytes;

    return true;
}

bool MDF3::mdf3_addDGToFile(quint64 curFilePos, int size, QList<A2L_VarMeas *> measPamList, bool lastDG)
{
    return true;
}

bool MDF3::mdf3_addRawDataToFile(char *data, int size)
{
    if (!mdf3_File.isOpen())
        return false;

    QDataStream out(&mdf3_File);
    int numBytes = out.writeRawData(data, size);
    //qDebug()<<"write data bytes="<<numBytes;
}

quint64 MDF3::mdf3_getFileCurPos()
{
    return mdf3_File.pos();
}
