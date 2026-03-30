#ifndef ZLGCAN_DRIVER_H
#define ZLGCAN_DRIVER_H

#include "zlgcan_x86/zlgcan.h"
#include <QString>

//Set before init can

//设置 FD 模式 0-ISO FD, 1-FD BOSCH
UINT ZCAN_SetCANFDStandard(DEVICE_HANDLE device_handle, UINT can_index, int canfd_standard);

//设置 仲裁域波特率
UINT ZCAN_SetAbitBaud(DEVICE_HANDLE device_handle, UINT can_index, UINT abitbaudrate);

//设置 数据域波特率
UINT ZCAN_SetDbitBaud(DEVICE_HANDLE device_handle, UINT can_index, UINT dbitbaudrate);

//设置 定制波特率 CAN
UINT ZCAN_SetCustomBaudRate(DEVICE_HANDLE device_handle, UINT can_index, char *customBaud);

//Set after init can
//设置 电阻开关 0-Disable, 1-Enable
UINT ZCAN_SetResistanceEnable(DEVICE_HANDLE device_handle, UINT can_index, int enable);

//
UINT ZCAN_SetAutoSend(DEVICE_HANDLE device_handle, UINT can_index, ZCAN_AUTO_TRANSMIT_OBJ *pData);

UINT ZCAN_SetAutoSendFD(DEVICE_HANDLE device_handle, UINT can_index, ZCANFD_AUTO_TRANSMIT_OBJ *pData);

UINT ZCAN_ApplyAutoSend(DEVICE_HANDLE device_handle, UINT can_index);

UINT ZCAN_ClearAutoSend(DEVICE_HANDLE device_handle, UINT can_index);

bool ZCAN_GetCANFDStandard(DEVICE_HANDLE device_handle, UINT can_index, int &canfd_standard);
bool ZCAN_GetAbitBaud(DEVICE_HANDLE device_handle, UINT can_index, UINT &canfd_abit_baudrate);

// set filter
bool ZCAN_SetFilterMode(CHANNEL_HANDLE channel_handle, UINT can_index, int filter_mode);  //0=standard, 1=extented
bool ZCAN_SetFilterStart(CHANNEL_HANDLE channel_handle, UINT can_index, UINT filterId_start);
bool ZCAN_SetFilterEnd(CHANNEL_HANDLE channel_handle, UINT can_index, UINT filterId_end);
bool ZCAN_SetFilterAck(CHANNEL_HANDLE channel_handle, UINT can_index);
bool ZCAN_SetFilterClear(CHANNEL_HANDLE channel_handle, UINT can_index);

bool ZCAN_SetFilter(CHANNEL_HANDLE channel_handle, UINT can_index, int filter_mode, UINT filterId_start, UINT filterId_end);

//设置发送方式
UINT ZCAN_SetSendMode(CHANNEL_HANDLE channel_handle, UINT can_index, int sendmode);
//返回发送队列剩余空间
UINT ZCAN_AvailableTxCount(CHANNEL_HANDLE channel_handle, UINT can_index);

#endif // ZLGCAN_DRIVER_H
