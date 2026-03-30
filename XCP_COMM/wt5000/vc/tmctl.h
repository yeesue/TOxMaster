/*STARTM------------------------------------------------------------------
+       Module Name     : tmctl.h                                        +
+       Abstract        : Yokogawa T&M Remote Control functions.         +
+       Revision History:                                                +
+       Rev.    Date.          Notes.                                    +
+       --------------------------------------------------------------   +
+       2.0		20070309	USBTMC2 VXI11 supported. thread safe support +
+       2.1		20080508	RS232C expand                                +
+       2.2		20080704	Add TmcGetInitializeError.                   +
+       2.3		20121121	coped 64 bits                                +
+       3.0		20150820	add VISA USB control                         +
+       4.0		20160523	add Ether Socket control                     +
-ENDM-------------------------------------------------------------------*/

#ifndef	_TMCTL_H
#define	_TMCTL_H	1

#include <windows.h>

/* Control setting */
#define	TM_NO_CONTROL	0
#define	TM_CTL_GPIB		1
#define	TM_CTL_RS232	2
#define	TM_CTL_USB		3
#define	TM_CTL_ETHER	4
#define	TM_CTL_USBTMC	5
#define	TM_CTL_ETHERUDP	6
#define	TM_CTL_USBTMC2	7				// 2007/01/19 add
#define	TM_CTL_VXI11	8				// 2007/01/19 add
#define	TM_CTL_USB2		9
#define	TM_CTL_VISAUSB	10
#define	TM_CTL_SOCKET	11
#define	TM_CTL_USBTMC3	12
#define	TM_CTL_USB3		13
#define TM_CTL_HISLIP	14
#define	TM_MAX_CTL		15				// —\–ñ

/* GPIB */

/* RS232 */
#define	TM_RS_1200		'0'
#define	TM_RS_2400		'1'
#define	TM_RS_4800		'2'
#define	TM_RS_9600		'3'
#define	TM_RS_19200		'4'
#define	TM_RS_38400		'5'
#define	TM_RS_57600		'6'
#define	TM_RS_115200	'7'			// Ver2.1

#define	TM_RS_8N1		'0'
#define	TM_RS_8N		TM_RS_8N1
#define	TM_RS_7E		'1'
#define	TM_RS_7O		'2'
#define	TM_RS_8O		'3'			// Ver2.1 8bit,odd,onestopbit
#define	TM_RS_7N5		'4'			// Ver2.1 7bit,noparity,one5stopbit
#define	TM_RS_8N2		'5'

#define	TM_RS_NO		'0'
#define	TM_RS_XON		'1'
#define	TM_RS_HARD		'2'

/* USB */
#define TM_USB_CHECK_OK				0
#define TM_USB_CHECK_NOTOPEN		1
#define TM_USB_CHECK_NODEVICE		2

#define	TM_USB_READPIPE				0
#define	TM_USB_WRITEPIPE			1
#define	TM_USB_STATUSPIPE			2

/* Error Number */
#define	TMCTL_NO_ERROR				0x00000000		/* No error */
#define	TMCTL_TIMEOUT				0x00000001		/* Timeout */
#define	TMCTL_NO_DEVICE				0x00000002		/* Device Not Found */
#define	TMCTL_FAIL_OPEN				0x00000004		/* Open Port Error */
#define	TMCTL_NOT_OPEN				0x00000008		/* Device Not Open */
#define	TMCTL_DEVICE_ALREADY_OPEN	0x00000010		/* Device Already Open */
#define	TMCTL_NOT_CONTROL			0x00000020		/* Controller Not Found */
#define	TMCTL_ILLEGAL_PARAMETER		0x00000040		/* Parameter is illegal */
#define	TMCTL_SEND_ERROR			0x00000100		/* Send Error */
#define	TMCTL_RECV_ERROR			0x00000200		/* Receive Error */
#define	TMCTL_NOT_BLOCK				0x00000400		/* Data is not Block Data */
#define	TMCTL_SYSTEM_ERROR			0x00001000		/* System Error */
#define	TMCTL_ILLEGAL_ID			0x00002000		/* Device ID is Illegal */
#define	TMCTL_NOT_SUPPORTED			0x00004000		/* this feature not supportred */
#define	TMCTL_INSUFFICIENT_BUFFER	0x00008000		/* unsufficient buffer size */
#define	TMCTL_LIBRARY_ERROR			0x00010000		/* Dll Not Found */

/* L2 Error Number */
#define	TMCTL2_NO_ERROR				(00000)			/* No error */
#define	TMCTL2_TIMEOUT				(10001)			/* Timeout */
#define	TMCTL2_NO_DEVICE			(10002)			/* Device Not Found */
#define	TMCTL2_FAIL_OPEN			(10003)			/* Open Port Error */
#define	TMCTL2_NOT_OPEN				(10004)			/* Device Not Open */
#define	TMCTL2_DEVICE_ALREADY_OPEN	(10005)			/* Device Already Open */
#define	TMCTL2_NOT_CONTROL			(10006)			/* Controller Not Found */
#define	TMCTL2_ILLEGAL_PARAMETER	(10007)			/* Parameter is illegal */
#define	TMCTL2_SEND_ERROR			(10008)			/* Send Error */
#define	TMCTL2_RECV_ERROR			(10009)			/* Receive Error */
#define	TMCTL2_NOT_BLOCK			(10010)			/* Data is not Block Data */
#define	TMCTL2_SYSTEM_ERROR			(10011)			/* System Error */
#define	TMCTL2_ILLEGAL_ID			(10012)			/* Device ID is Illegal */
#define	TMCTL2_NOT_SUPPORTED		(10013)			/* this feature not supportred */
#define	TMCTL2_INSUFFICIENT_BUFFER	(10014)			/* unsufficient buffer size */
#define	TMCTL2_LIBRARY_ERROR		(10015)			/* Dll Not Found */

#define	ADRMAXLEN			(64)

typedef	struct _Devicelist
{
	char	adr[ADRMAXLEN] ;
} DEVICELIST ;

typedef	struct _DevicelistEx
{
	char		adr[ADRMAXLEN] ;
unsigned short	vendorID ;
unsigned short	productID ;
	char		dummy[188] ;
} DEVICELISTEX ;

// type define for "TmcSetCallback"
typedef void( *Hndlr)(int, UCHAR, ULONG, ULONG) ;

/* Functions */
#ifndef	_TMCTL_DEFINES
#ifdef	__cplusplus
extern	"C" {
#endif
extern	int		 TmcInitialize( int wire, char* adr, int* id ) ;
extern	int		 TmcInitializeEx( int wire, char* adr, int* id , int tmo) ;
extern	int		 TmcSetIFC( int id, int tm ) ;
extern	int		 TmcDeviceClear( int id ) ;
extern	int		 TmcDeviceTrigger( int id ) ;
extern	int		 TmcSend( int id, char* msg ) ;
extern	int		 TmcSendByLength( int id, char* msg, int len ) ;
extern	int		 TmcSendSetup( int id ) ;
extern	int		 TmcSendOnly( int id, char* msg, int len, int end ) ;
extern	int		 TmcReceive( int id, char* buff, int blen, int* rlen ) ;
extern	int		 TmcReceiveSetup( int id ) ;
extern	int		 TmcReceiveOnly( int id, char* buff, int blen, int* rlen ) ;
extern	int		 TmcReceiveBlockHeader( int id, int* len ) ;
extern	int		 TmcReceiveBlockData( int id, char* buff, int blen, int* rlen, int* end ) ;
extern	int		 TmcCheckEnd( int id ) ;
extern	int		 TmcSetCmd( int id, char* cmd ) ;
extern	int		 TmcSetRen( int id, int flag ) ;
extern	int		 TmcGetLastError( int id ) ;
extern	int		 TmcGetDetailLastError( int id ) ;
extern	int		 TmcCheckError( int id, int sts, char* msg, char* err ) ;
extern	int		 TmcSetTerm( int id, int eos, int eot ) ;
extern	int		 TmcSetEos( int id, unsigned char eos ) ;
extern	int		 TmcSetTimeout( int id, int tmo ) ;
extern	int		 TmcSetDma( int id, int flg ) ;
extern	int		 TmcGetStatusByte( int id, unsigned char* sts ) ;
extern	int		 TmcFinish( int id ) ;
extern	int		 TmcSearchDevices(int wire, DEVICELIST* list, int max, int* num,char* option) ;
extern	int		 TmcSearchDevicesEx(int wire, DEVICELISTEX* list, int max, int* num,char* option) ;
extern	int		 TmcWaitSRQ(int id, char* stsbyte, int tmo) ;
extern	int		 TmcAbortWaitSRQ(int id) ;
extern	int		 TmcSetCallback(int id,Hndlr func, ULONG p1, ULONG p2) ;
extern	int		 TmcResetCallback(int id) ;
extern	int		 TmcSendTestData(int id, char* msg, int len ) ;
extern	int		 TmcReceiveTestData( int id, char* buff, int blen, int* rlen ) ;
extern	int		 TmcInitiateAbortBulkIn(int id, UCHAR tagNo) ;
extern	int		 TmcInitiateAbortBulkOut(int id, UCHAR tagNo) ;
extern	int		 TmcCheckAbortBulkInStatus(int id) ;
extern	int		 TmcCheckAbortBulkOutStatus(int id) ;
extern	int		 TmcEncodeSerialNumber(char* encode,size_t len,char* src) ;
extern	int		 TmcDecodeSerialNumber(char* decode,size_t len,char* src) ;
extern	int		 TmcGotoLocal( int id ) ;
extern	int		 TmcLocalLockout(int id) ;
extern  int		 TmcAbortPipe(int id,long pipeNo) ;
extern  int		 TmcResetPipe(int id,long pipeNo) ;
extern	int		 TmcWriteHeader(int id, int blen) ;
extern	int		 TmcReceiveWithoutWriteHeader(int id, char* buff, int blen, int* rlen, int* end ) ;
extern	int		 TmcGetTagNo(int id, UCHAR* tag) ;
extern	int		 TmcSendByLength2( int id, char* msg, int msgSize, int len, CHAR eof) ;
extern	int		 TmcDeviceChangeNotify(HWND hWnd, BOOL bStart) ;
extern	int		 TmcCheckUSB(int id) ;
extern	int		 TmcGetPipeNo(int id,int type,int* pipeNo) ;
extern	int		 TmcCheckGUID(void *lParam) ;
extern	ULONG	 TmcGetInitializeError() ;
extern	int		 TmcGetWireInfo( int id, int* wire );

// L2 Functions
extern	int	 xTmcSearchDevices(int wire, DEVICELIST* adrlist, int max, int *num,char* option) ;
extern	int	 xTmcSearchDevicesEx(int wire, DEVICELISTEX* adrlist, int max, int *num,char* option) ;
extern	int	 xTmcInitialize( int wire, char* adr, int* id ) ;
extern	int	 xTmcInitializeEx( int wire, char* adr, int* id, int tmo ) ;
extern	int	 xTmcFinish( int id ) ;
extern	int	 xTmcSend( int id, char* msg ) ;
extern	int	 xTmcSendByLength( int id, char* msg, int len ) ;
extern	int	 xTmcSendSetup( int id ) ;
extern	int	 xTmcSendOnly( int id, char* msg, int len, int end ) ;
extern	int	 xTmcReceive( int id, char* buff, int blen, int* rlen ) ;
extern	int	 xTmcReceiveSetup( int id ) ;
extern	int	 xTmcReceiveOnly( int id, char* buff, int blen, int* rlen ) ;
extern	int	 xTmcReceiveBlockHeader( int id, int* length ) ;
extern	int	 xTmcReceiveBlockData( int id, char* buff, int blen, int* rlen, int* end ) ;
extern	int	 xTmcCheckEnd( int id ) ;
extern	int	 xTmcDeviceClear( int id ) ;
extern	int	 xTmcSetRen( int id, int flag ) ;
extern	int	 xTmcGetLastError( int id ) ;
extern	int	 xTmcSetTerm( int id, int eos, int eot ) ;
extern	int	 xTmcSetTimeout( int id, int tmo ) ;
extern	int	 xTmcCreateSRQHandler( int id, UINT msgId, HWND hWnd) ;
extern	int	 xTmcDleateSRQHandler( int id ) ;
extern	int	 xTmcSetCallback(int id,Hndlr func, ULONG p1, ULONG p2) ;
extern	int	 xTmcResetCallback(int id) ;
extern	int	 xTmcSet( int id, char *msg) ;
extern	int	 xTmcSetBinary( int id, char* msg, char *buf, int size ) ;
extern	int	 xTmcGet( int id, char *msg, char* buf, int blen, int* rlen ) ;
extern	int  xTmcGetBinary( int id, char *msg, char* buf, int blen, int* rlen ) ;
extern	int	 xTmcGetErrorQueue(int id) ;
extern	int	 xTmcSetNE( int id, char *msg) ;
extern	int	 xTmcSetBinaryNE( int id, char* msg, char *buf, int size ) ;
extern	int	 xTmcGetNE( int id, char *msg, char* buf, int blen, int* rlen ) ;
extern	int  xTmcGetBinaryNE( int id, char *msg, char* buf, int blen, int* rlen ) ;
extern	int	 xTmcSetNES( int id, char *msg) ;
extern	int	 xTmcSetBinaryNES( int id, char* msg, char *buf, int size ) ;
extern	int	 xTmcLock( int id ) ;
extern	int	 xTmcUnlock( int id ) ;
extern	int	 xTmcGetWireInfo( int id, int* wire );

#ifdef	__cplusplus
}
#endif
#endif

#endif
