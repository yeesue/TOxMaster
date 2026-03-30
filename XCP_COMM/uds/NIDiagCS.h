// --------------------------------------------
// (c) Copyright National Instruments 2009-2020
// All rights reserved.
// --------------------------------------------
#ifndef NIDIAGCS_H
#define NIDIAGCS_H

#include "nidiagcs_extcode.h"
#pragma pack(push)
#if defined(_WIN64)
#	pragma pack(8)
#else
#	pragma pack(1)
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	int32_t DiagCANHandle;
	uint16_t TransportProtocol;
	uint8_t ReceiveAddressExtension;
	uint8_t TransmitAddressExtension;
	uint8_t Reserved1;
	uint8_t Reserved2;
	} TD1;

typedef struct {
	int32_t StartByte;
	int32_t ByteLength;
	uint16_t ByteOrder;
	uint16_t DataType;
	double ScaleFactor;
	double ScaleOffset;
	} TD2;

typedef struct {
	int32_t DTCByteLength;
	int32_t StatusByteLength;
	int32_t AddDataByteLength;
	uint16_t ByteOrder;
	} TD3;

typedef struct {
	uint32_t DTC;
	uint32_t Status;
	uint32_t AddData;
	} TD4;

typedef struct {
	uint32_t SPN;
	uint32_t Status;
	uint32_t AddData;
	uint32_t FMI;
	} TD5;

typedef struct {
	uint16_t DTC;
	uint8_t FTB;
	uint32_t Status;
	uint32_t AddData;
	} TD6;


/*!
 * ndCloseDiagnostic
 */
int32_t __stdcall ndCloseDiagnostic(TD1 *diagRef);
/*!
 * ndConvertFromPhys
 */
void __stdcall ndConvertFromPhys(TD2 *typeDescriptor, double value, 
	uint8_t dataOut[], int32_t *len);
/*!
 * ndConvertToPhys
 */
void __stdcall ndConvertToPhys(TD2 *typeDescriptor, uint8_t dataIn[], 
	int32_t len, double *value);
/*!
 * ndDiagnosticService
 */
int32_t __stdcall ndDiagnosticService(TD1 *diagRef, 
	LVBoolean *requireResponse, uint8_t dataIn[], int32_t len, uint8_t dataOut[], 
	int32_t *len2);
/*!
 * ndDisableNormalMessageTransmission
 */
int32_t __stdcall ndDisableNormalMessageTransmission(TD1 *diagRef, 
	LVBoolean *requireResponse, LVBoolean *success);
/*!
 * ndDTCToString
 */
void __stdcall ndDTCToString(uint32_t DTCNum, char DTCString[], int32_t *len);
/*!
 * ndECUReset
 */
int32_t __stdcall ndECUReset(TD1 *diagRef, uint8_t mode, LVBoolean *success);
/*!
 * ndEnableNormalMessageTransmission
 */
int32_t __stdcall ndEnableNormalMessageTransmission(TD1 *diagRef, 
	LVBoolean *requireResponse, LVBoolean *success);
/*!
 * ndInputOutputControlByLocalIdentifier
 */
int32_t __stdcall ndInputOutputControlByLocalIdentifier(TD1 *diagRef, 
	uint8_t localID, uint8_t mode, uint8_t dataIn[], int32_t len, 
	uint8_t dataOut[], int32_t *len2, LVBoolean *success);
/*!
 * ndOBDClearEmissionRelatedDiagnosticInformation
 */
int32_t __stdcall ndOBDClearEmissionRelatedDiagnosticInformation(
	TD1 *diagRef, LVBoolean *success);
/*!
 * ndOBDRequestControlOfOnBoardDevice
 */
int32_t __stdcall ndOBDRequestControlOfOnBoardDevice(TD1 *diagRef, 
	uint8_t TID, uint8_t dataIn[], int32_t len, uint8_t dataOut[], int32_t *len2, 
	LVBoolean *success);
/*!
 * ndOBDRequestCurrentPowertrainDiagnosticData
 */
int32_t __stdcall ndOBDRequestCurrentPowertrainDiagnosticData(TD1 *diagRef, 
	uint8_t PID, uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndOBDRequestEmissionRelatedDTCs
 */
int32_t __stdcall ndOBDRequestEmissionRelatedDTCs(TD1 *diagRef, 
	TD3 *DTCDescriptor, TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndOBDRequestEmissionRelatedDTCsDuringCurrentDriveCycle
 */
int32_t __stdcall ndOBDRequestEmissionRelatedDTCsDuringCurrentDriveCycle(
	TD1 *diagRef, TD3 *DTCDescriptor, TD4 DTCs[], int32_t *len, 
	LVBoolean *success);
/*!
 * ndOBDRequestOnBoardMonitoringTestResults
 */
int32_t __stdcall ndOBDRequestOnBoardMonitoringTestResults(TD1 *diagRef, 
	uint8_t OBDMID, uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndOBDRequestPermanentFaultCodes
 */
int32_t __stdcall ndOBDRequestPermanentFaultCodes(TD1 *diagRef, 
	TD3 *DTCDescriptor, TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndOBDRequestPowertrainFreezeFrameData
 */
int32_t __stdcall ndOBDRequestPowertrainFreezeFrameData(TD1 *diagRef, 
	uint8_t PID, uint8_t frame, uint8_t dataOut[], int32_t *len, 
	LVBoolean *success);
/*!
 * ndOBDRequestVehicleInformation
 */
int32_t __stdcall ndOBDRequestVehicleInformation(TD1 *diagRef, 
	uint8_t infoType, uint8_t *items, uint8_t dataOut[], int32_t *len, 
	LVBoolean *success);
/*!
 * ndOpenDiagnostic
 */
int32_t __stdcall ndOpenDiagnostic(char CANInterface[], uint32_t baudrate, 
	uint16_t transportProtocol, uint32_t transmitID, uint32_t receiveID, 
	TD1 *diagRefOut);
/*!
 * ndReadDataByLocalIdentifier
 */
int32_t __stdcall ndReadDataByLocalIdentifier(TD1 *diagRef, uint8_t localID, 
	uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndReadDTCByStatus
 */
int32_t __stdcall ndReadDTCByStatus(TD1 *diagRef, uint8_t mode, 
	uint16_t groupOfDTC, TD3 *DTCDescriptor, TD4 DTCs[], int32_t *len, 
	LVBoolean *success);
/*!
 * ndReadECUIdentification
 */
int32_t __stdcall ndReadECUIdentification(TD1 *diagRef, uint8_t mode, 
	uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndReadMemoryByAddress
 */
int32_t __stdcall ndReadMemoryByAddress(TD1 *diagRef, uint32_t address, 
	uint8_t size, uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndReadStatusOfDTC
 */
int32_t __stdcall ndReadStatusOfDTC(TD1 *diagRef, uint16_t groupOfDTC, 
	TD3 *DTCDescriptor, TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndRequestRoutineResultsByLocalIdentifier
 */
int32_t __stdcall ndRequestRoutineResultsByLocalIdentifier(TD1 *diagRef, 
	uint8_t localID, uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndRequestSeed
 */
int32_t __stdcall ndRequestSeed(TD1 *diagRef, uint8_t accessMode, 
	uint8_t seedOut[], int32_t *len, LVBoolean *success);
/*!
 * ndSendKey
 */
int32_t __stdcall ndSendKey(TD1 *diagRef, uint8_t accessMode, 
	uint8_t keyIn[], int32_t len, LVBoolean *success);
/*!
 * ndStartDiagnosticSession
 */
int32_t __stdcall ndStartDiagnosticSession(TD1 *diagRef, uint8_t mode, 
	LVBoolean *success);
/*!
 * ndStartRoutineByLocalIdentifier
 */
int32_t __stdcall ndStartRoutineByLocalIdentifier(TD1 *diagRef, 
	uint8_t localID, uint8_t dataIn[], int32_t len, uint8_t dataOut[], 
	int32_t *len2, LVBoolean *success);
/*!
 * ndStatusToString
 */
void __stdcall ndStatusToString(int32_t errorCode, char message[], 
	int32_t *len);
/*!
 * ndStopDiagnosticSession
 */
int32_t __stdcall ndStopDiagnosticSession(TD1 *diagRef, LVBoolean *success);
/*!
 * ndStopRoutineByLocalIdentifier
 */
int32_t __stdcall ndStopRoutineByLocalIdentifier(TD1 *diagRef, 
	uint8_t localID, uint8_t dataIn[], int32_t len, uint8_t dataOut[], 
	int32_t *len2, LVBoolean *success);
/*!
 * ndTesterPresent
 */
int32_t __stdcall ndTesterPresent(TD1 *diagRef, LVBoolean *responseRequired, 
	LVBoolean *success);
/*!
 * ndUDSCommunicationControl
 */
int32_t __stdcall ndUDSCommunicationControl(TD1 *diagRef, uint8_t type, 
	uint8_t communicationType, LVBoolean *success);
/*!
 * ndUDSControlDTCSetting
 */
int32_t __stdcall ndUDSControlDTCSetting(TD1 *diagRef, uint8_t type, 
	uint8_t dataIn[], int32_t len, LVBoolean *success);
/*!
 * ndUDSDiagnosticSessionControl
 */
int32_t __stdcall ndUDSDiagnosticSessionControl(TD1 *diagRef, uint8_t mode, 
	LVBoolean *success);
/*!
 * ndUDSECUReset
 */
int32_t __stdcall ndUDSECUReset(TD1 *diagRef, uint8_t mode, 
	LVBoolean *success);
/*!
 * ndUDSInputOutputControlByIdentifier
 */
int32_t __stdcall ndUDSInputOutputControlByIdentifier(TD1 *diagRef, 
	uint16_t ID, uint8_t mode, uint8_t dataIn[], int32_t len, uint8_t dataOut[], 
	int32_t *len2, LVBoolean *success);
/*!
 * ndUDSReadDataByIdentifier
 */
int32_t __stdcall ndUDSReadDataByIdentifier(TD1 *diagRef, uint16_t ID, 
	uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndUDSReadMemoryByAddress
 */
int32_t __stdcall ndUDSReadMemoryByAddress(TD1 *diagRef, uint32_t address, 
	uint8_t size, uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndUDSReportDTCBySeverityMaskRecord
 */
int32_t __stdcall ndUDSReportDTCBySeverityMaskRecord(TD1 *diagRef, 
	uint8_t severityMask, uint8_t status, TD3 *DTCDescriptor, 
	uint8_t *statusAvailMask, TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndUDSReportDTCByStatusMask
 */
int32_t __stdcall ndUDSReportDTCByStatusMask(TD1 *diagRef, 
	uint8_t statusMask, TD3 *DTCDescriptor, uint8_t *statusAvailMask, TD4 DTCs[], 
	int32_t *len, LVBoolean *success);
/*!
 * ndUDSReportSeverityInformationOfDTC
 */
int32_t __stdcall ndUDSReportSeverityInformationOfDTC(TD1 *diagRef, 
	uint32_t DTCMaskRecord, TD3 *DTCDescriptor, uint8_t *statusAvailMask, 
	TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndUDSReportSupportedDTCs
 */
int32_t __stdcall ndUDSReportSupportedDTCs(TD1 *diagRef, TD3 *DTCDescriptor, 
	uint8_t *statusAvailMask, TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndUDSRequestSeed
 */
int32_t __stdcall ndUDSRequestSeed(TD1 *diagRef, uint8_t accessMode, 
	uint8_t seedOut[], int32_t *len, LVBoolean *success);
/*!
 * ndUDSRoutineControl
 */
int32_t __stdcall ndUDSRoutineControl(TD1 *diagRef, uint16_t ID, 
	uint8_t mode, uint8_t dataIn[], int32_t len, uint8_t dataOut[], 
	int32_t *len2, LVBoolean *success);
/*!
 * ndUDSSendKey
 */
int32_t __stdcall ndUDSSendKey(TD1 *diagRef, uint8_t accessMode, 
	uint8_t keyIn[], int32_t len, LVBoolean *success);
/*!
 * ndUDSTesterPresent
 */
int32_t __stdcall ndUDSTesterPresent(TD1 *diagRef, 
	LVBoolean *responseRequired, LVBoolean *success);
/*!
 * ndUDSWriteDataByIdentifier
 */
int32_t __stdcall ndUDSWriteDataByIdentifier(TD1 *diagRef, uint16_t ID, 
	uint8_t dataIn[], int32_t len, LVBoolean *success);
/*!
 * ndUDSWriteMemoryByAddress
 */
int32_t __stdcall ndUDSWriteMemoryByAddress(TD1 *diagRef, uint32_t address, 
	uint8_t size, uint8_t dataIn[], int32_t len, LVBoolean *success);
/*!
 * ndWriteDataByLocalIdentifier
 */
int32_t __stdcall ndWriteDataByLocalIdentifier(TD1 *diagRef, uint8_t localID, 
	uint8_t dataIn[], int32_t len, LVBoolean *success);
/*!
 * ndWriteMemoryByAddress
 */
int32_t __stdcall ndWriteMemoryByAddress(TD1 *diagRef, uint32_t address, 
	uint8_t size, uint8_t dataIn[], int32_t len, LVBoolean *success);
/*!
 * ndSetProperty
 */
void __stdcall ndSetProperty(uint16_t propertyID, uint32_t propertyValue);
/*!
 * ndGetProperty
 */
uint32_t __stdcall ndGetProperty(uint16_t propertyID);
/*!
 * ndCreateExtendedCANIds
 */
void __stdcall ndCreateExtendedCANIds(uint16_t addressingMode, 
	uint16_t transportProtocol, uint8_t sourceAddress, uint8_t targetAddress, 
	uint32_t *transmitID, uint32_t *receiveID);
/*!
 * ndVWTPDisconnect
 */
int32_t __stdcall ndVWTPDisconnect(TD1 *diagRef);
/*!
 * ndVWTPConnect
 */
int32_t __stdcall ndVWTPConnect(TD1 *diagRef, uint32_t channelID, 
	uint8_t applicationType);
/*!
 * ndVWTPConnectionTest
 */
int32_t __stdcall ndVWTPConnectionTest(TD1 *diagRef);
/*!
 * ndClearDiagnosticInformation
 */
int32_t __stdcall ndClearDiagnosticInformation(TD1 *diagRef, 
	uint32_t groupOfDTC, TD3 *DTCDescriptor, LVBoolean *success);
/*!
 * ndControlDTCSetting
 */
int32_t __stdcall ndControlDTCSetting(TD1 *diagRef, uint32_t groupOfDTC, 
	uint8_t dataIn[], int32_t len, TD3 *DTCDescriptor, LVBoolean *success);
/*!
 * ndUDSClearDiagnosticInformation
 */
int32_t __stdcall ndUDSClearDiagnosticInformation(TD1 *diagRef, 
	uint32_t groupOfDTC, TD3 *DTCDescriptor, LVBoolean *success);
/*!
 * ndOBDOpen
 */
int32_t __stdcall ndOBDOpen(char CANInterface[], uint32_t baudrate, 
	uint32_t transmitID, uint32_t receiveID, TD1 *diagRefOut);
/*!
 * ndUDSRequestDownload
 */
int32_t __stdcall ndUDSRequestDownload(TD1 *diagRef, uint32_t memoryAddress, 
	uint32_t memorySize, uint8_t memoryAddressLength, uint8_t memorySizeLength, 
	uint8_t dataFormatIdentifier, uint32_t *blockSize, LVBoolean *success);
/*!
 * ndUDSRequestUpload
 */
int32_t __stdcall ndUDSRequestUpload(TD1 *diagRef, uint32_t memoryAddress, 
	uint32_t memorySize, uint8_t memoryAddressLength, uint8_t memorySizeLength, 
	uint8_t dataFormatIdentifier, uint32_t *blockSize, LVBoolean *success);
/*!
 * ndUDSTransferData
 */
int32_t __stdcall ndUDSTransferData(TD1 *diagRef, 
	uint8_t *blockSequenceCounter, uint8_t dataIn[], int32_t len, 
	uint8_t dataOut[], int32_t *len2, LVBoolean *success);
/*!
 * ndUDSRequestTransferExit
 */
int32_t __stdcall ndUDSRequestTransferExit(TD1 *diagRef, uint8_t dataIn[], 
	int32_t len, uint8_t dataOut[], int32_t *len2, LVBoolean *success);
/*!
 * ndUDS06WriteMemoryByAddress
 */
int32_t __stdcall ndUDS06WriteMemoryByAddress(TD1 *diagRef, 
	uint8_t memAddrLen, uint8_t memSizeLen, uint32_t address, uint32_t size, 
	uint8_t dataIn[], int32_t len, LVBoolean *success);
/*!
 * ndUDS06ReadMemoryByAddress
 */
int32_t __stdcall ndUDS06ReadMemoryByAddress(TD1 *diagRef, 
	uint8_t memAddrLen, uint8_t memSizeLen, uint32_t address, uint32_t size, 
	uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndDiagFrameRecv
 */
int32_t __stdcall ndDiagFrameRecv(TD1 *diagRef, uint32_t timeout, 
	uint8_t dataOut[], int32_t *len);
/*!
 * ndDiagFrameSend
 */
int32_t __stdcall ndDiagFrameSend(TD1 *diagRef, uint8_t dataIn[], 
	int32_t len);
/*!
 * ndDoIPActivateRouting
 */
int32_t __stdcall ndDoIPActivateRouting(TD1 *diagRef, uint8_t ActivationType, 
	uint16_t SourceAddress, uint16_t *TargetAddress);
/*!
 * ndDoIPConnect
 */
int32_t __stdcall ndDoIPConnect(TD1 *diagRef, char address[], 
	uint16_t SourceAddress, uint16_t TargetAddress);
/*!
 * ndDoIPDisconnect
 */
int32_t __stdcall ndDoIPDisconnect(TD1 *diagRef);
/*!
 * ndDoIPEntityStatus
 */
int32_t __stdcall ndDoIPEntityStatus(TD1 *diagRef, uint8_t *nodeType, 
	uint8_t *maxSockets, uint8_t *curSockets, LVBoolean *ok);
/*!
 * ndDoIPGetDiagPowerMode
 */
int32_t __stdcall ndDoIPGetDiagPowerMode(TD1 *diagRef, uint8_t *powerMode, 
	LVBoolean *ok);
/*!
 * ndDoIPSendVehicleIdentRequest
 */
int32_t __stdcall ndDoIPSendVehicleIdentRequest(TD1 *diagRef);
/*!
 * ndDoIPSendVehicleIdentReqEID
 */
int32_t __stdcall ndDoIPSendVehicleIdentReqEID(TD1 *diagRef, char EID[]);
/*!
 * ndDoIPSendVehicleIdentReqVIN
 */
int32_t __stdcall ndDoIPSendVehicleIdentReqVIN(TD1 *diagRef, char VIN[]);
/*!
 * ndDoIPGetEntities
 */
int32_t __stdcall ndDoIPGetEntities(TD1 *diagRef, uint16_t DoIPOpenType, 
	char VINOrEID[], uint8_t DoIPEntities[], int32_t *len);
/*!
 * ndOpenDiagnosticOnIP
 */
int32_t __stdcall ndOpenDiagnosticOnIP(LVBoolean *dynamicPort, 
	TD1 *DiagReferenceOut);
/*!
 * ndOpenDiagnosticOnLIN
 */
int32_t __stdcall ndOpenDiagnosticOnLIN(char LINInterface[], 
	uint32_t baudrate, uint8_t NAD, char MasterReqFrame[], char SlaveRespFrame[], 
	TD1 *diagRefOut);
/*!
 * ndWWHOBDClearEmissionRelatedDTCs
 */
int32_t __stdcall ndWWHOBDClearEmissionRelatedDTCs(TD1 *diagRef, 
	LVBoolean *success);
/*!
 * ndWWHOBDConvertDTCsToJ1939
 */
int32_t __stdcall ndWWHOBDConvertDTCsToJ1939(TD4 DTCs[], int32_t lenDTCs, 
	TD5 DTCsJ1939[], int32_t *lenDTCsJ1939);
/*!
 * ndWWHOBDConvertDTCsToJ2012
 */
int32_t __stdcall ndWWHOBDConvertDTCsToJ2012(TD4 DTCs[], int32_t lenDTCs, 
	TD6 DTCsJ2012[], int32_t *lenDTCsJ2012);
/*!
 * ndWWHOBDRequestDID
 */
int32_t __stdcall ndWWHOBDRequestDID(TD1 *diagRef, uint16_t dataIdentifier, 
	uint8_t dataOut[], int32_t *len, LVBoolean *success);
/*!
 * ndWWHOBDRequestDTCExtendedDataRecord
 */
int32_t __stdcall ndWWHOBDRequestDTCExtendedDataRecord(TD1 *diagRef, 
	uint8_t DTCMaskRecord[], int32_t len, uint8_t dataOut[], int32_t *len2, 
	LVBoolean *success);
/*!
 * ndWWHOBDRequestEmissionRelatedDTCs
 */
int32_t __stdcall ndWWHOBDRequestEmissionRelatedDTCs(TD1 *diagRef, 
	uint8_t DTCSeverityMask, uint8_t DTCStatusMask, TD3 *DTCDescriptor, 
	uint8_t *severityAvailabilityMask, uint8_t *statusAvailabilityMask, 
	uint8_t *DTCFormatIdentifier, TD4 DTCs[], int32_t *len, LVBoolean *success);
/*!
 * ndWWHOBDRequestFreezeFrameInformation
 */
int32_t __stdcall ndWWHOBDRequestFreezeFrameInformation(TD1 *diagRef, 
	uint8_t DTCMaskRecord[], int32_t len, uint8_t DTCRecordNumber, 
	uint8_t dataOut[], int32_t *len2, LVBoolean *success);
/*!
 * ndWWHOBDRequestRID
 */
int32_t __stdcall ndWWHOBDRequestRID(TD1 *diagRef, uint8_t mode, 
	uint16_t routineIdentifier, uint8_t dataOut[], int32_t *len, 
	LVBoolean *success);
/*!
 * ndWWHOBDRequestSupportedDIDs
 */
int32_t __stdcall ndWWHOBDRequestSupportedDIDs(TD1 *diagRef, uint16_t DID, 
	uint16_t DIDsOut[], int32_t *len, LVBoolean *success);
/*!
 * ndWWHOBDRequestSupportedRIDs
 */
int32_t __stdcall ndWWHOBDRequestSupportedRIDs(TD1 *diagRef, 
	uint16_t RIDsOut[], int32_t *len, LVBoolean *success);
/*!
 * ndGetTimeStamp
 */
int32_t __stdcall ndGetTimeStamp(TD1 *diagRef, uint64_t *timeStampWriteFirst, 
	uint64_t *timeStampWriteLast, uint64_t *timeStampReadFirst, 
	uint64_t *timeStampReadLast);
/*!
 * ndGetHandleSize
 */
int32_t __stdcall ndGetHandleSize(void);
/*!
 * ndDoIPGetEntitiesEx
 */
int32_t __stdcall ndDoIPGetEntitiesEx(TD1 *diagRef, char BroadcastAddress[], 
	uint16_t DoIPOpenType, char VINOrEID[], uint8_t DoIPEntities[], int32_t *len);
/*!
 * ndDoIPSendVehicleIdentReqEIDEx
 */
int32_t __stdcall ndDoIPSendVehicleIdentReqEIDEx(TD1 *diagRef, 
	char BroadcastAddress[], char EID[]);
/*!
 * ndDoIPSendVehicleIdentRequestEx
 */
int32_t __stdcall ndDoIPSendVehicleIdentRequestEx(TD1 *diagRef, 
	char BroadcastAddress[]);
/*!
 * ndDoIPSendVehicleIdentReqVINEx
 */
int32_t __stdcall ndDoIPSendVehicleIdentReqVINEx(TD1 *diagRef, 
	char BroadcastAddress[], char VIN[]);
/*!
 * ndDiagGetProperty
 */
int32_t __stdcall ndDiagGetProperty(TD1 *diagRef, uint16_t propertyID, 
	uint32_t *propertyValue);
/*!
 * ndDiagSetProperty
 */
int32_t __stdcall ndDiagSetProperty(TD1 *diagRef, uint16_t propertyID, 
	uint32_t propertyValue);
/*!
 * ndOBDOpenOnCANFD
 */
int32_t __stdcall ndOBDOpenOnCANFD(char CANInterface[], uint32_t baudrate, 
	uint64_t FDBaudrate, uint32_t ioMode, uint16_t maxDLC, uint32_t transmitID, 
	uint32_t receiveID, TD1 *diagRefOut);
/*!
 * ndOpenDiagnosticOnCANFD
 */
int32_t __stdcall ndOpenDiagnosticOnCANFD(char CANInterface[], 
	uint64_t baudrate, uint64_t FDBaudrate, uint32_t ioMode, 
	uint16_t transportProtocol, uint16_t maxDLC, uint32_t transmitID, 
	uint32_t receiveID, TD1 *diagRefOut);
/*!
 * ndOpenDiagnosticOnIPVersion
 */
int32_t __stdcall ndOpenDiagnosticOnIPVersion(LVBoolean *dynamicPort, 
	uint8_t DoIPProtocolVersion, TD1 *DiagRefOut);
/*!
 * ndStartPeriodicTesterPresent
 */
int32_t __stdcall ndStartPeriodicTesterPresent(TD1 *diagRef, 
	uint32_t messageType, double periodSec, int32_t responseRequired, 
	uint8_t userMessage[], int32_t len);
/*!
 * ndStopPeriodicTesterPresent
 */
int32_t __stdcall ndStopPeriodicTesterPresent(TD1 *diagRef);
/*!
 * ndDoIPSendVehicleIdentReqEIDAllVersions
 */
int32_t __stdcall ndDoIPSendVehicleIdentReqEIDAllVersions(TD1 *diagRef, 
	char BroadcastAddress[], LVBoolean *AllVersions, char EID[]);
/*!
 * ndDoIPSendVehicleIdentRequestAllVersions
 */
int32_t __stdcall ndDoIPSendVehicleIdentRequestAllVersions(TD1 *diagRef, 
	char BroadcastAddress[], LVBoolean *AllVersions);
/*!
 * ndDoIPSendVehicleIdentReqVINAllVersions
 */
int32_t __stdcall ndDoIPSendVehicleIdentReqVINAllVersions(TD1 *diagRef, 
	char BroadcastAddress[], LVBoolean *AllVersions, char VIN[]);
/*!
 * ndDoIPGetEntitiesAllVersions
 */
int32_t __stdcall ndDoIPGetEntitiesAllVersions(TD1 *diagRef, 
	char BroadcastAddress[], LVBoolean *AllVersions, uint16_t DoIPOpenType, 
	char VINOrEID[], uint8_t DoIPEntities[], int32_t *len);
/*!
 * ndDoIPConnectVersion
 */
int32_t __stdcall ndDoIPConnectVersion(TD1 *diagRef, char address[], 
	uint16_t SourceAddress, uint16_t TargetAddress, uint8_t ProtocolVersion);

MgErr __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

void __cdecl SetExecuteVIsInPrivateExecutionSystem(Bool32 value);

#ifdef __cplusplus
} // extern "C"
#endif

#pragma pack(pop)


#endif // ifndef NIDIAGCS_H
