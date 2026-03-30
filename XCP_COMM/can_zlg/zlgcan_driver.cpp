#include "zlgcan_driver.h"
#include <QDebug>

UINT ZCAN_SetCANFDStandard(DEVICE_HANDLE device_handle, UINT can_index, int canfd_standard)
{
    UINT status = 0;


    std::string str1 = std::to_string(can_index);
    std::string str2 = "/canfd_standard";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    //qDebug()<<"fd standard path:"<<QString::fromStdString(str);

    const char *value = std::to_string(canfd_standard).c_str();
    //const char *value = (const char*)&canfd_standard;

    //qDebug()<<"value:"<<QString(value);

    //char path[50] = {0};
    //sprintf_s(path, "%d/canfd_standard", can_index);

    //char value[10] = {0};
    //sprintf_s(value, "%d", canfd_standard);

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

UINT ZCAN_SetAbitBaud(DEVICE_HANDLE device_handle, UINT can_index, UINT abitbaudrate)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/canfd_abit_baud_rate";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    //qDebug()<<"abitbaud path:"<<QString::fromStdString(str);

    const char* value = std::to_string(abitbaudrate).c_str();


    //char path[50] = {0};
    //sprintf_s(path, "%d/canfd_abit_baud_rate", can_index);

    //char value[10] = {0};
    //sprintf_s(value, "%d", abitbaudrate);

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

UINT ZCAN_SetDbitBaud(DEVICE_HANDLE device_handle, UINT can_index, UINT dbitbaudrate)
{
    UINT status = 0;


    std::string str1 = std::to_string(can_index);
    std::string str2 = "/canfd_dbit_baud_rate";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    //qDebug()<<"dbitbaud path:"<<QString::fromStdString(str);

    const char* value = std::to_string(dbitbaudrate).c_str();

    //char path[50] = {0};
    //sprintf_s(path, "%d/canfd_dbit_baud_rate", can_index);

    //char value[10] = {0};
    //sprintf_s(value, "%d", dbitbaudrate);

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

UINT ZCAN_SetCustomBaudRate(DEVICE_HANDLE device_handle, UINT can_index, char *customBaud)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/baud_rate_custom";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    //qDebug()<<"baud_rate_custom path:"<<QString::fromStdString(str);

    const char* value = customBaud;

    /*
    char path[50] = {0};
    sprintf_s(path, "%d/baud_rate_custom", can_index);
    char value[10] = {0};
    sprintf_s(value, "%d", baudrate);
    */

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

UINT ZCAN_SetResistanceEnable(CHANNEL_HANDLE channel_handle, UINT can_index, int enable)
{
    UINT status = 0;


    std::string str1 = std::to_string(can_index);
    std::string str2 = "/initenal_resistance";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = std::to_string(enable).c_str();


    //char path[50] = {0};
    //sprintf_s(path, "%d/initenal_resistance", can_index);
    //char value[10] = {0};
    //sprintf_s(value, "%d", enable);

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}


UINT ZCAN_SetAutoSend(DEVICE_HANDLE device_handle, UINT can_index, ZCAN_AUTO_TRANSMIT_OBJ *pData)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/auto_send";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    char* value = (char*)pData;

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

UINT ZCAN_SetAutoSendFD(DEVICE_HANDLE device_handle, UINT can_index, ZCANFD_AUTO_TRANSMIT_OBJ *pData)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/auto_send_canfd";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = (const char*)pData;

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}


UINT ZCAN_ApplyAutoSend(DEVICE_HANDLE device_handle, UINT can_index)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/apply_auto_send";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = "0";

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

UINT ZCAN_ClearAutoSend(DEVICE_HANDLE device_handle, UINT can_index)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/clear_auto_send";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = "0";

    status = ZCAN_SetValue(device_handle, path, value);

    return status;
}

bool ZCAN_GetCANFDStandard(DEVICE_HANDLE device_handle, UINT can_index, int &canfd_standard)
{

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/canfd_standard";
    std::string str = str1 + str2;
    const char* path = str.c_str();


    //char path[50] = {0};
    //sprintf_s(path, "%d/canfd_standard", can_index);

    //int canfd_standard;

    const char* pRet = (const char*)ZCAN_GetValue(device_handle, path);
    if(pRet)
    {
        canfd_standard = *(int*)pRet;
        //qDebug()<<"Get value, canfd_standard:"<<canfd_standard;
        return true;
    }

    return false;
}

bool ZCAN_GetAbitBaud(DEVICE_HANDLE device_handle, UINT can_index, UINT &canfd_abit_baudrate)
{

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/canfd_abit_baud_rate";
    std::string str = str1 + str2;
    const char* path = str.c_str();


    //char path[50] = {0};
    //sprintf_s(path, "%d/canfd_abit_baud_rate", can_index);

    //UINT canfd_abit_baudrate;

    const char* pRet = (const char*)ZCAN_GetValue(device_handle, path);
    if(pRet)
    {
        canfd_abit_baudrate = *(UINT*)pRet;
        //qDebug()<<"Get value, canfd_abit_baudrate:"<<canfd_abit_baudrate;
        return true;
    }

    return false;
}


bool ZCAN_SetFilterMode(CHANNEL_HANDLE channel_handle, UINT can_index, int filter_mode)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/filter_mode";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = std::to_string(filter_mode).c_str();

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}

bool ZCAN_SetFilterStart(CHANNEL_HANDLE channel_handle, UINT can_index, UINT filterId_start)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/filter_start";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    QString id_hexStr = "0x" + QString::number(filterId_start, 16);

    const char* value = id_hexStr.toStdString().c_str();

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}

bool ZCAN_SetFilterEnd(CHANNEL_HANDLE channel_handle, UINT can_index, UINT filterId_end)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/filter_end";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    QString id_hexStr = "0x" + QString::number(filterId_end, 16);

    const char* value = id_hexStr.toStdString().c_str();

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}

bool ZCAN_SetFilterAck(CHANNEL_HANDLE channel_handle, UINT can_index)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/filter_ack";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = "0";

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}

bool ZCAN_SetFilterClear(CHANNEL_HANDLE channel_handle, UINT can_index)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/filter_clear";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = "0";

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}

bool ZCAN_SetFilter(CHANNEL_HANDLE channel_handle, UINT can_index, int filter_mode, UINT filterId_start, UINT filterId_end)
{
    bool setOk1 = ZCAN_SetFilterMode(channel_handle, can_index, filter_mode);

    bool setOk2 = ZCAN_SetFilterStart(channel_handle, can_index, filterId_start);

    bool setOk3 = ZCAN_SetFilterEnd(channel_handle, can_index, filterId_end);

    //qDebug()<<"set1:"<<setOk1<<", set2:"<<setOk2<<", set2:"<<setOk2;

    return true;
}

UINT ZCAN_SetSendMode(CHANNEL_HANDLE channel_handle, UINT can_index, int sendmode)
{
    UINT status = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/set_send_mode";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    const char* value = std::to_string(sendmode).c_str();

    status = ZCAN_SetValue(channel_handle, path, value);

    return status;
}

UINT ZCAN_AvailableTxCount(CHANNEL_HANDLE channel_handle, UINT can_index)
{
    UINT txBuf = 0;

    std::string str1 = std::to_string(can_index);
    std::string str2 = "/get_device_available_tx_count/1";
    std::string str = str1 + str2;
    const char* path = str.c_str();

    char *ret = (char*)ZCAN_GetValue(channel_handle, path);

    if(ret)
    {
        txBuf = *(UINT*)ret;
    }

    return txBuf;
}
