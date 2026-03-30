/*!
   \file nixnet.h
   \brief NI-XNET C API Implementation
*/
/*
   Copyright (c) 2008 National Instruments Corporation.
   All rights reserved.
*/

#ifndef ___nixnet_h___
#define ___nixnet_h___

#if defined(__GNUC__)
#include <stdint.h>   // C99 typedef for GCC
#endif


/***********************************************************************
                     C O M P I L E R   D E F I N E S
***********************************************************************/

   /* Needed for C++ to C (DLL) calls  */
#ifdef __cplusplus
   extern "C" {
#endif


#if defined(_WIN32)
   #define  _NXFUNC __cdecl
#else
   #define  _NXFUNC
#endif

#ifdef _CVI_
   #pragma  EnableLibraryRuntimeChecking
#endif /* _CVI_ */

/***********************************************************************
                              S T A T U S
***********************************************************************/

#define NX_STATUS_QUALIFIER                  (nxStatus_t)(0x3FF63000)
#define NX_STATUS_WARNING                    (nxStatus_t)(0x00000000)
#define NX_STATUS_ERROR                      (nxStatus_t)(0x80000000)

#define NX_WARNING_BASE                      (NX_STATUS_QUALIFIER | NX_STATUS_WARNING)
#define NX_ERROR_BASE                        (NX_STATUS_QUALIFIER | NX_STATUS_ERROR)

#define nxSuccess                            0

   // For a description of each error/warning (including solutions), use nxStatusToString

//! An internal error occurred in the NI-XNET driver. Please contact National
//! Instruments and provide the log files found in
//! %LOCALAPPDATA%\National Instruments\NI-XNET\log. Please note that this
//! location may be hidden on your computer. For LabVIEW RT systems, logs are
//! located in /ni-rt/nixnet/log or /var/local/natinst/nixnet/log.
#define nxErrInternalError                   (NX_ERROR_BASE | 0x001)

//! Board self test failed(code 2). Solution: try reinstalling the driver or
//! switching the slot(s) of the board(s). If the error persists,contact
//! National Instruments.
#define nxErrSelfTestError1                  (NX_ERROR_BASE | 0x002)

//! Board self test failed(code 3). Solution: try reinstalling the driver or
//! switching the slot(s) of the board(s). If the error persists,contact
//! National Instruments.
#define nxErrSelfTestError2                  (NX_ERROR_BASE | 0x003)

//! Board self test failed(code 4). Solution: try reinstalling the driver or
//! switching the slot(s) of the board(s). If the error persists,contact
//! National Instruments.
#define nxErrSelfTestError3                  (NX_ERROR_BASE | 0x004)

//! Board self test failed(code 5). Solution: try reinstalling the driver or
//! switching the slot(s) of the board(s). If the error persists,contact
//! National Instruments.
#define nxErrSelfTestError4                  (NX_ERROR_BASE | 0x005)

//! Board self test failed(code 6). Solution: try reinstalling the driver or
//! switching the slot(s) of the board(s). If the error persists,contact
//! National Instruments.
#define nxErrSelfTestError5                  (NX_ERROR_BASE | 0x006)

//! Computer went to hibernation mode and the board lost power. Solution:
//! prevent the computer from going to hibernation mode in the control panel.
#define nxErrPowerSuspended                  (NX_ERROR_BASE | 0x007)

//! A write queue overflowed. Solution: wait until queue space becomes available
//! and retry.
#define nxErrOutputQueueOverflow             (NX_ERROR_BASE | 0x008)

//! The board's firmware did not answer a command. Solution: Stop your
//! application and execute a self test. Try deactivating/reactivating the
//! driver in the Device Manager. If the problem persists, contact National
//! Instruments.
#define nxErrFirmwareNoResponse              (NX_ERROR_BASE | 0x009)

//! The operation timed out. Solution: specify a timeout long enough to complete
//! the operation, or change the operation in a way that it can get completed in
//! less time (e.g. read less data).
#define nxErrEventTimeout                    (NX_ERROR_BASE | 0x00A)

//! A read queue overflowed. Solution: reduce your data rate or call Read more
//! frequently.
#define nxErrInputQueueOverflow              (NX_ERROR_BASE | 0x00B)

//! The Read buffer is too small to hold a single frame. Solution: provide a
//! buffer large enough.
#define nxErrInputQueueReadSize              (NX_ERROR_BASE | 0x00C)

//! You tried to open the same frame twice. This is not permitted. Solution:
//! open each frame only once.
#define nxErrDuplicateFrameObject            (NX_ERROR_BASE | 0x00D)

//! You tried to open the same stream object twice. This is not permitted.
//! Solution: open each stream object only once.
#define nxErrDuplicateStreamObject           (NX_ERROR_BASE | 0x00E)

//! Self test is not possible since the board is in use by an application.
//! Solution: stop all NI-XNET applications before executing a self test.
#define nxErrSelfTestNotPossible             (NX_ERROR_BASE | 0x00F)

//! Allocation of memory failed. You do not have sufficient memory in the
//! LabVIEW target. Solution: add more RAM or try to use fewer resources in your
//! applications (arrays, XNET sessions, etc).
#define nxErrMemoryFull                      (NX_ERROR_BASE | 0x010)

//! The maximum number of sessions was exceeded. Solution: use fewer sessions.
#define nxErrMaxSessions                     (NX_ERROR_BASE | 0x011)

//! The maximum number of frames has been exceeded. Solution: Use fewer frames
//! in your sessions.
#define nxErrMaxFrames                       (NX_ERROR_BASE | 0x012)

//! The maximum number of devices has been detected. Solution: use fewer
//! devices.
#define nxErrMaxDevices                      (NX_ERROR_BASE | 0x013)

//! A driver support file is missing. Solution: try reinstalling the driver. If
//! the error persists, contact National Instruments.
#define nxErrMissingFile                     (NX_ERROR_BASE | 0x014)

//! This indicates that a NULL pointer or an empty string was passed to a
//! function. The user should verify that the parameters passed in make sense
//! for the given function.
#define nxErrParameterNullOrEmpty            (NX_ERROR_BASE | 0x015)

//! The maximum number of schedules has been detected. Solution: Use fewer
//! schedules.
#define nxErrMaxSchedules                    (NX_ERROR_BASE | 0x016)

//! Board self test failed (code 17). Solution: Try reinstalling the driver or
//! switching the slot(s) of the board(s). If the error persists, contact
//! National Instruments.
#define nxErrSelfTestError6                  (NX_ERROR_BASE | 0x017)

//! You cannot start an NI-XNET application while a self test is in progress.
//! Solution: Complete the self test before starting any NI-XNET applications.
#define nxErrSelfTestInProgress              (NX_ERROR_BASE | 0x018)

//! An invalid reference has been passed to a NI-XNET session function.
//! Solution: Only pass reference retrieved from Create Session, or from an IO
//! name of a session in LabVIEW project.
#define nxErrInvalidSessionHandle            (NX_ERROR_BASE | 0x020)

//! An invalid reference has been passed to a NI-XNET system function. Solution:
//! Only pass a valid system reference.
#define nxErrInvalidSystemHandle             (NX_ERROR_BASE | 0x021)

//! A device reference was expected for a NI-XNET session function. Solution:
//! Only pass a device reference.
#define nxErrDeviceHandleExpected            (NX_ERROR_BASE | 0x022)

//! An interface reference was expected for a NI-XNET session function.
//! Solution: Only pass an interface reference.
#define nxErrIntfHandleExpected              (NX_ERROR_BASE | 0x023)

//! You have configured a property that conflicts with the current mode of the
//! session. For example, you have created a CAN output session with a frame
//! configured with a Timing Type = Cyclic and a Transmit Time of 0.
#define nxErrPropertyModeConflicting         (NX_ERROR_BASE | 0x024)

//! XNET Create Timing Source VI is not supported on Windows. This VI is
//! supported on LabVIEW Real-Time targets only.
#define nxErrTimingSourceNotSupported        (NX_ERROR_BASE | 0x025)

//! You tried to create more than one LabVIEW timing source for a single
//! interface. Only one timing source per interface is supported. The timing
//! source remains until the top-level VI is idle (no longer running). Solution:
//! Call the XNET Create Timing Source VI only once per interface. You can use
//! the timing source with multiple timed structures (e.g. timed loops).
#define nxErrMultipleTimingSource            (NX_ERROR_BASE | 0x026)

//! You invoked two or more VIs simultaneously for the same session, and those
//! VIs do not support overlap. For example, you attempted to invoke two Read
//! VIs at the same time for the same session. Solution: Wire the error cluster
//! from one VI to another, to enforce sequential execution for the session.
#define nxErrOverlappingIO                   (NX_ERROR_BASE | 0x027)

//! You are trying to start an interface that is missing bus power for the
//! transceiver. Some physical layers on NI-XNET hardware are internally
//! powered, but others require external power for the port to operate. This
//! error occurs when starting an interface on hardware that requires external
//! power when no power is detected. Solution: Supply proper voltage to your
//! transceiver. Refer to the NI-XNET Hardware Overview in the NI-XNET Hardware
//! and Software Manual for more information.
#define nxErrMissingBusPower                 (NX_ERROR_BASE | 0x028)

//! The connection with a CompactDAQ chassis was lost, and the host software and
//! modules are out of sync. There is no direct recovery for this problem until
//! the chassis is reset. Solutions: Call DAQmx Reset Device as the first VI or
//! function in your application, prior to creating XNET sessions. Alternately,
//! you could reset the CompactDAQ chassis in Measurement and Automation
//! Explorer (MAX).
#define nxErrCdaqConnectionLost              (NX_ERROR_BASE | 0x029)

//! The transceiver value set is invalid (for this port, e.g. LS on a HS port)
//! or you are trying to perform an operation that requires a different
//! transceiver (e.g., trying to change the state of a disconnected
//! transceiver). Solution: set a valid value.
#define nxErrInvalidTransceiver              (NX_ERROR_BASE | 0x071)

//! The baud rate value set is invalid. Solution: set a valid value.
#define nxErrInvalidBaudRate                 (NX_ERROR_BASE | 0x072)

//! No baud rate value has been set. Solution: set a valid value.
#define nxErrBaudRateNotConfigured           (NX_ERROR_BASE | 0x073)

//! The bit timing values set are invalid. Solution: set valid values.
#define nxErrInvalidBitTimings               (NX_ERROR_BASE | 0x074)

//! The baud rate set does not match the transceiver's allowed range. Solution:
//! change either the baud rate or the transceiver.
#define nxErrBaudRateXcvrMismatch            (NX_ERROR_BASE | 0x075)

//! The configured terminal is not known for this interface. Solution: Make sure
//! that that you pass in a valid value to Connect Terminals or Disconnect
//! Terminals.
#define nxErrUnknownTimingSource             (NX_ERROR_BASE | 0x076)

//! The configured terminal is inappropriate for the hardware. For example,
//! setting a source to FrontPanel0 on XNET hardware that doesn't have
//! front-panel trigger inputs, or selecting PXI_Clk10 for a non-PXI device.
//! Solution: Pick an appropriate terminal for the hardware.
#define nxErrUnknownSynchronizationSource    (NX_ERROR_BASE | 0x077)

//! The source that you connected to the Master Timebase destination is missing.
//! When the start trigger is received, the interface verifies that a signal is
//! present on the configured source. This check has determined that this signal
//! is missing. Solution: Verify that your cables are configured correctly and
//! that your timebase source is generating an appropriate waveform.
#define nxErrMissingTimebaseSource              (NX_ERROR_BASE | 0x078)

//! The source that you connected to the Master Timebase destination is not
//! generating an appropriate signal. When the start trigger is received, the
//! interface verifies that a signal of a known frequency is present on the
//! configured source. This check has determined that this source is generating
//! a signal, but that the signal is not one of the supported frequencies for
//! this hardware. Solution: Verify that your source is generating a signal at a
//! supported frequency.
#define nxErrUnknownTimebaseFrequency           (NX_ERROR_BASE | 0x079)

//! You are trying to disconnect a synchronization terminal that is not
//! currently connected. Solution: Only disconnect synchronization terminals
//! that have previously been connected.
#define nxErrUnconnectedSynchronizationSource   (NX_ERROR_BASE | 0x07A)

//! You are trying to connect a synchronization terminal that is already in use.
//! For example, you are trying to connect a trigger line to the Master Timebase
//! when a different trigger line is already connected to the Master Timebase.
//! Solution: Only connect to synchronization terminals that are not currently
//! in use.
#define nxErrConnectedSynchronizationTerminal   (NX_ERROR_BASE | 0x07B)

//! You are trying to connect an XNET terminal as a source terminal, but the
//! desired XNET terminal is not valid as a source terminal. Solution: Only
//! connect valid source terminals to the source terminal in XNET Connect
//! Terminals.
#define nxErrInvalidSynchronizationSource       (NX_ERROR_BASE | 0x07C)

//! You are trying to connect an XNET terminal as a destination terminal, but
//! the desired XNET terminal is not valid as a destination terminal. Solution:
//! Only connect valid destination terminals to the destination terminal in XNET
//! Connect Terminals.
#define nxErrInvalidSynchronizationDestination  (NX_ERROR_BASE | 0x07D)

//! You are trying to connect two XNET terminals that are incompatible.
//! Solution: Only connect a source and destination terminals that are
//! compatible with each other.
#define nxErrInvalidSynchronizationCombination  (NX_ERROR_BASE | 0x07E)

//! The source that you connected to the Master Timebase destination has
//! disappeared. When the start trigger is received, the interface verifies that
//! a signal is present on the configured source. This check has determined that
//! this signal was present, but while the interface was running, the signal
//! disappeared, so all timebase configuration has reverted to using the onboard
//! (unsynchronized) oscillator. Solution: Verify that your cables are
//! configured correctly and that your timebase source is generating an
//! appropriate waveform the entire time your application is running.
#define nxErrTimebaseDisappeared                (NX_ERROR_BASE | 0x07F)

//! You called Read (State : FlexRay : Cycle Macrotick), and the FlexRay
//! Macrotick is not connected as the master timebase of the interface.
//! Solution: Call Connect Terminals to connect source of FlexRay Macrotick to
//! destination of Master Timebase.
#define nxErrMacrotickDisconnected           (NX_ERROR_BASE | 0x080)

//! The database specified could not be opened. Solution: Check that the alias
//! and/or the file exist and that it is a valid database. If the file exists,
//! the path may contain Unicode characters which cannot be utilized by NI-XNET.
#define nxErrCannotOpenDatabaseFile          (NX_ERROR_BASE | 0x081)

//! The cluster was not found in the database. Solution: Make sure you only
//! initialize a cluster in a session that is defined in the database.
#define nxErrClusterNotFound                 (NX_ERROR_BASE | 0x082)

//! The frame was not found in the database. Solution: Make sure you only
//! initialize frames in a session that are defined in the database.
#define nxErrFrameNotFound                   (NX_ERROR_BASE | 0x083)

//! The signal was not found in the database. Solution: Make sure you only
//! initialize signals in a session that are defined in the database.
#define nxErrSignalNotFound                  (NX_ERROR_BASE | 0x084)

//! A necessary property for a cluster was not found in the database. Solution:
//! Make sure you only initialize a cluster in a session that is completely
//! defined in the database.
#define nxErrUnconfiguredCluster             (NX_ERROR_BASE | 0x085)

//! A necessary property for a frame was not found in the database. Solution:
//! Make sure you only initialize frames in a session that are completely
//! defined in the database.
#define nxErrUnconfiguredFrame               (NX_ERROR_BASE | 0x086)

//! A necessary property for a signal was not found in the database. Solution:
//! Make sure you only initialize signals in a session that are completely
//! defined in the database.
#define nxErrUnconfiguredSignal              (NX_ERROR_BASE | 0x087)

//! Multiple clusters have been specified in one session, either directly
//! (Stream I/O), or through the signals or frames specified. Solution: Make
//! sure that in one session, you open only one cluster, including frames or
//! signals that belong to the same cluster.
#define nxErrMultipleClusters                (NX_ERROR_BASE | 0x088)

//! You specified a database of ':subordinate:' for a session mode other than
//! mode of Frame Input Stream. Solution: either open a Frame Input Stream
//! session, or use a real or in-memory database.
#define nxErrSubordinateNotAllowed           (NX_ERROR_BASE | 0x089)

//! The interface name given does not specify a valid and existing interface.
//! Solution: Use a valid and existing interface. These can be obtained using
//! MAX, XNET system properties, or the LabVIEW XNET Interface IO name. If you
//! are using CompactRIO, refer to the topic "Getting Started with CompactRIO"
//! in the NI-XNET Hardware and Software Help.
#define nxErrInvalidInterface                (NX_ERROR_BASE | 0x08A)

//! The operation is invalid for this interface (e.g. you tried to open a set of
//! FlexRay frames on a CAN interface, or tried to request a CAN property from a
//! FlexRay interface). Solution: run this operation on a suitable interface.
#define nxErrInvalidProtocol                 (NX_ERROR_BASE | 0x08B)

//! You tried to set the AutoStart property to FALSE for an Input session. This
//! is not allowed. Solution: don't set the AutoStart property (TRUE is
//! default).
#define nxErrInputSessionMustAutoStart       (NX_ERROR_BASE | 0x08C)

//! The property ID you specified is not valid (or not valid for the current
//! session mode or form factor).
#define nxErrInvalidPropertyId               (NX_ERROR_BASE | 0x08D)

//! The contents of the property is bigger than the size specified. Use the
//! nxGetPropertySize function to determine the size of the buffer needed.
#define nxErrInvalidPropertySize             (NX_ERROR_BASE | 0x08E)

//! The function you called is not defined for the session mode (e.g. you called
//! a frame I/O function on a signal I/O session).
#define nxErrIncorrectMode                   (NX_ERROR_BASE | 0x08F)

//! The data that you passed to the XNET Write is too small to hold all the data
//! specified for the session. Solution: determine the number of elements
//! (frames or signals) that you configured for the session, and pass that
//! number of elements to XNET Write.
#define nxErrBufferTooSmall                  (NX_ERROR_BASE | 0x090)

//! For Signal Output sessions, the multiplexer signals used in the session must
//! be specified explicitly in the signal list.
#define nxErrMustSpecifyMultiplexers         (NX_ERROR_BASE | 0x091)

//! You used an XNET Session IO name, and that session was not found in your
//! LabVIEW project. Solution: Within LabVIEW project, right-click the target
//! (RT or My Computer), and select New > NI-XNET Session. Add the VI that uses
//! the session under the target. If you are using the session with a built
//! application (.EXE), ensure that you copy the built configuration file
//! nixnetSession.txt such that it resides in the same folder as the executable.
#define nxErrSessionNotFound                 (NX_ERROR_BASE | 0x092)

//! You used the same XNET session name in multiple top-level VIs, which is not
//! supported. Solution: Use each session in only one top-level VI (application)
//! at a time.
#define nxErrMultipleUseOfSession            (NX_ERROR_BASE | 0x093)

//! To execute this function properly, the session's list must contain only one
//! frame. Solution: break your session up into multiple, each of which contains
//! only one frame.
#define nxErrOnlyOneFrame                    (NX_ERROR_BASE | 0x094)

//! You used the same alias for different database files which is not allowed.
//! Solution: Use each alias only for a single database file.
#define nxErrDuplicateAlias                  (NX_ERROR_BASE | 0x095)

//! You try to deploy a database file while another deployment is in progress.
//! Solution: wait until the other deployment has finished and try again.
#define nxErrDeploymentInProgress            (NX_ERROR_BASE | 0x096)

//! A signal or frame session has been opened, but it doesn’t contain signals or
//! frames. Solution: specify at least one signal or frame.
#define nxErrNoFramesOrSignals               (NX_ERROR_BASE | 0x097)

//! An invalid value has been specified for the ‘mode?parameter. Solution:
//! specify a valid value.
#define nxErrInvalidMode                     (NX_ERROR_BASE | 0x098)

//! A session was created by references, but no database references have been
//! specified. Solution: specify at least one appropriate database reference
//! (i.e. signal or frame or cluster ref depending on the session mode).
#define nxErrNeedReference                   (NX_ERROR_BASE | 0x099)

//! The interface has already been opened with different cluster settings than
//! the ones specified for this session. Solution: make sure that the cluster
//! settings agree for the interface, or use a different interface.
#define nxErrDifferentClusterOpen            (NX_ERROR_BASE | 0x09A)

//! The cycle repetition of a frame in the database for the FlexRay protocol is
//! invalid. Solution: Make sure that the cycle repetition is a power of 2
//! between 1 and 64.
#define nxErrFlexRayInvalidCycleRep          (NX_ERROR_BASE | 0x09B)

//! You called XNET Clear for the session, then tried to perform another
//! operation. Solution: Defer clear (session close) until you are done using
//! it. This error can also occur if you branch a wire after creating the
//! session. Solution: Do not branch a session to multiple flows in the diagram.
#define nxErrSessionCleared                  (NX_ERROR_BASE | 0x09C)

//! You called Create Session VI with a list of items that does not match the
//! mode. This includes using: 1) signal items for a Frame I/O mode 2) frame
//! items for a Signal I/O mode 3) cluster item for a mode other than Frame
//! Input Stream or Frame Output Stream
#define nxErrWrongModeForCreateSelection     (NX_ERROR_BASE | 0x09D)

//! You tried to create a new session while the interface is already running.
//! Solution: Create all sessions before starting any of them.
#define nxErrInterfaceRunning                (NX_ERROR_BASE | 0x09E)

//! You wrote a frame whose payload length is larger than the maximum payload
//! allowed by the database (e.g. wrote 10 bytes for CAN frame, max 8 bytes).
//! Solution: Never write more payload bytes than the Payload Length Maximum
//! property of the session.
#define nxErrFrameWriteTooLarge              (NX_ERROR_BASE | 0x09F)

//! You called a Read function with a nonzero timeout, and you used a negative
//! numberToRead. Negative value for numberToRead requests all available data
//! from the Read, which is ambiguous when used with a timeout. Solutions: 1)
//! Pass timeout of and numberToRead of -1, to request all available data. 2)
//! Pass timeout > 0, and numberToRead > 0, to wait for a specific number of
//! data elements.
#define nxErrTimeoutWithoutNumToRead         (NX_ERROR_BASE | 0x0A0)

//! Timestamps are not (yet) supported for Write Signal XY. Solution: Do not
//! provide a timestamp array for Write Signal XY.
#define nxErrTimestampsNotSupported          (NX_ERROR_BASE | 0x0A1)

// \REVIEW: Rename to WaitCondition
//! The condition parameter passed to Wait is not known. Solution: Pass a valid
//! parameter.
#define nxErrUnknownCondition                (NX_ERROR_BASE | 0x0A2)

//! You attempted an I/O operation, but the session is not yet started (and the
//! AutoStart property is set to FALSE). Solution: call Start before you use
//! this IO operation.
#define nxErrSessionNotStarted               (NX_ERROR_BASE | 0x0A3)

//! The maximum number of Wait operations has been exceeded. Solution: If you
//! are waiting for multiple events on the interface, use fewer Wait operations
//! on this interface (even for multiple sessions). If you are waiting for
//! multiple events for a frame (e.g. transmit complete), use only one Wait at a
//! time for that frame.
#define nxErrMaxWaitsExceeded                (NX_ERROR_BASE | 0x0A4)

//! You used an invalid name for an XNET Device. Solution: Get valid XNET Device
//! names from the XNET System properties (only).
#define nxErrInvalidDevice                   (NX_ERROR_BASE | 0x0A5)

//! A terminal name passed to ConnectTerminals or DisconnectTerminals is
//! unknown. Solution: only pass valid names.
#define nxErrInvalidTerminalName             (NX_ERROR_BASE | 0x0A6)

//! You tried to blink the port LEDs but these are currently busy. Solution:
//! stop all applications running on that port; do not access it from MAX or LV
//! Project.
#define nxErrPortLEDsBusy                    (NX_ERROR_BASE | 0x0A7)

//! You tried to set a FlexRay keyslot ID that is not listed as valid in the
//! database. Solution: only pass slot IDs of frames that have the startup or
//! sync property set in the database.
#define nxErrInvalidKeyslot                  (NX_ERROR_BASE | 0x0A8)

//! You tried to set a queue size that is bigger than the maximum allowed.
//! Solution: Specify an in-range queue size.
#define nxErrMaxQueueSizeExceeded            (NX_ERROR_BASE | 0x0A9)

//! You wrote a frame whose payload length is different than the payload length
//! configured by the database. Solution: Never write a different payload length
//! for a frame that is different than the configured payload length.
#define nxErrFrameSizeMismatch               (NX_ERROR_BASE | 0x0AA)

//! The index to indicate an session list element is too large. Solution:
//! Specify an index in the range ... NumInList-1.
#define nxErrIndexTooBig                     (NX_ERROR_BASE | 0x0AB)

//! You have tried to create a session that is invalid for the mode of the
//! driver/firmware. For example, you are using the Replay Exclusive mode for
//! Stream Output and you have an output session open.
#define nxErrSessionModeIncompatibility      (NX_ERROR_BASE | 0x0AC)

//! The trigger signal for a frame is allowed only in Single Point Signal
//! sessions (Input or Output). For Output Single Point Signal sessions, only
//! one trigger signal is allowed per frame. Solution: Do not use the trigger
//! signal, or change to a single point I/O session.
#define nxErrTriggerSignalNotAllowed         (NX_ERROR_BASE | 0x0AD)

//! To execute this function properly, the session's list must contain only one
//! cluster. Solution: Use only one cluster in the session.
#define nxErrOnlyOneCluster                  (NX_ERROR_BASE | 0x0AE)

//! You attempted to convert a CAN or LIN frame with a payload length greater
//! than 8. For example, you may be converting a frame that uses a higher layer
//! transport protocol, such as SAE-J1939. NI-XNET currently supports conversion
//! of CAN/LIN frames only (layer 2). Solutions: 1) Implement higher layer
//! protocols (including signal conversion) within your code. 2) Contact
//! National Instruments to request this feature in a future version.
#define nxErrConvertInvalidPayload           (NX_ERROR_BASE | 0x0AF)

//! Allocation of memory failed for the data returned from LabVIEW XNET Read.
//! Solutions: 1) Wire a smaller "number to read" to XNET Read (default -1 uses
//! queue size). 2) For Signal Input Waveform, use a smaller resample rate. 3)
//! Set smaller value for session's queue size property (default is large to
//! avoid loss of data).
#define nxErrMemoryFullReadData              (NX_ERROR_BASE | 0x0B0)

//! Allocation of memory failed in the firmware. Solutions: 1) Create less
//! firmware objects 2) Set smaller value for output session's queue size
//! property (default is large to avoid loss of data).
#define nxErrMemoryFullFirmware              (NX_ERROR_BASE | 0x0B1)

//! The NI-XNET driver no longer can communicate with the device. Solution: Make
//! sure the device has not been removed from the computer.
#define nxErrCommunicationLost               (NX_ERROR_BASE | 0x0B2)

//! A LIN schedule has an invalid priority. Solution: Use a valid priority (0 =
//! NULL schedule, 1..254 = Run once schedule, 255 = Continuous schedule).
#define nxErrInvalidPriority                 (NX_ERROR_BASE | 0x0B3)

//! (Dis)ConnectTerminals is not allowed for XNET C Series modules. Solution: To
//! connect the module start trigger, use the Session property Interface Source
//! Terminal Start Trigger.
#define nxErrSynchronizationNotAllowed       (NX_ERROR_BASE | 0x0B4)

//! You requested a time (like Start or Communication Time) before the event has
//! happened. Solution: Request the time only after it occurred.
#define nxErrTimeNotReached                  (NX_ERROR_BASE | 0x0B5)

//! An internal input queue overflowed. Solution: Attempt to pull data from the
//! hardware faster. If you are connected by an external bus (for example, USB
//! or Ethernet), you can try to use a faster connection.
#define nxErrInternalInputQueueOverflow      (NX_ERROR_BASE | 0x0B6)

//! A bad firmware image file can not be loaded to the hardware. Solution:
//! Uninstall and reinstall the NI-XNET software as the default firmware file
//! may be corrupt. If you are using a custom firmware file, try rebuilding it.
#define nxErrBadImageFile                    (NX_ERROR_BASE | 0x0B7)

//! The encoding of embedded network data (CAN, FlexRay, LIN, etc.) within the
//! TDMS file is invalid. Solutions: 1) In the application that wrote (created)
//! the logfile, and the application in which you are reading it, confirm that
//! both use the same major version for frame data encoding
//! (NI_network_frame_version property of the TDMS channel). 2) Ensure that your
//! file was not corrupted.
#define nxErrInvalidLogfile                  (NX_ERROR_BASE | 0x0B8)

//! The NI-XNET hardware no longer can communicate with the transceiver cable.
//! This may be due to the cable being removed, a power loss event, an over
//! voltage condition on the power input, or a general communication error.
//! Solution: Make sure the dongle is properly latched and, for some hardware,
//! external power is properly applied. To detect other errors, stop your
//! application and execute a self test.
#define nxErrDongleCommunicationLost         (NX_ERROR_BASE | 0xB9)

//! A property value was out of range or incorrect. Solution: specify a correct
//! value.
#define nxErrInvalidPropertyValue            (NX_ERROR_BASE | 0x0C0)

//! Integration of the interface into the FlexRay cluster failed, so
//! communication did not start for the interface. Solution: check the cluster
//! and/or interface parameters and verify that there are startup frames
//! defined.
#define nxErrFlexRayIntegrationFailed        (NX_ERROR_BASE | 0x0C1)

//! The PDU was not found in the database. Solution: Make sure you initialize
//! only PDUs in a session that are defined in the database.
#define nxErrPduNotFound                     (NX_ERROR_BASE | 0x0D0)

//! A necessary property for a PDU was not found in the database. Solution: Make
//! sure you initialize only PDUs in a session that are completely defined in
//! the database.
#define nxErrUnconfiguredPdu                 (NX_ERROR_BASE | 0x0D1)

//! You tried to open the same PDU twice. This is not permitted. Solution: Open
//! each PDU only once.
#define nxErrDuplicatePduObject              (NX_ERROR_BASE | 0x0D2)

//! You can access this database object only by PDU, not by frame. Solution: For
//! CAN and LIN, this is not supported by the current version of NI-XNET; for
//! FlexRay, make sure the database is set to use PDUs.
#define nxErrNeedPdu                         (NX_ERROR_BASE | 0x0D3)

//! Remote communication with the LabVIEW RT target failed. Solution: check if
//! NI-XNET has been installed on the RT target and check if the NI-XNET RPC
//! server has been started.
#define nxErrRPCCommunication                (NX_ERROR_BASE | 0x100)

//! File transfer communication with the LabVIEW Real-Time (RT) target failed.
//! Solution: Check if the RT target has been powered on, the RT target has been
//! connected to the network, and if the IP address settings are correct. If the RT
//! target is password-protected, make sure you provided the correct user credentials.
#define nxErrFileTransferCommunication       (NX_ERROR_BASE | 0x101)
#define nxErrFTPCommunication                (nxErrFileTransferCommunication)
//! File transfer to the LabVIEW Real-Time (RT) target failed, because the
//! required files could not be accessed. Solution: You may have executed a VI
//! that opened the database, but did not close. If that is the case, you should
//! change the VI to call Database Close, then reboot the RT controller to
//! continue.
#define nxErrFileTransferAccess              (NX_ERROR_BASE | 0x102)
#define nxErrFTPFileAccess                   (nxErrFileTransferAccess)

//! The database file you want to use is already assigned to another alias.
//! Solution: Each database file can only be assigned to a single alias. Use the
//! alias that is already assigned to the database instead.
#define nxErrDatabaseAlreadyInUse            (NX_ERROR_BASE | 0x103)

//! An internal file used by NI-XNET could not be accessed. Solution: Make sure
//! that the internal NI-XNET files are not write protected and that the
//! directories for these files exist.
#define nxErrInternalFileAccess              (NX_ERROR_BASE | 0x104)

//! The file cannot be deployed because another file deployment is already
//! active. Solution: wait until the other file deployment has finished and try
//! again.
#define nxErrFileTransferActive              (NX_ERROR_BASE | 0x105)

//! The nixnet.dll or one of its components could not be loaded. Solution: try
//! reinstalling NI-XNET. If the error persists,contact National Instruments.
#define nxErrDllLoad                         (NX_ERROR_BASE | 0x117)

//! You attempted to perform an action on a session or interface that is
//! started, and the action that requires the session/interface to be stopped.
//! Solution: Stop the object before performing this action.
#define nxErrObjectStarted                   (NX_ERROR_BASE | 0x11E)

//! You have passed a default payload to the firmware where the number of bytes
//! in the payload is larger than the number of bytes that this frame can
//! transmit. Solution: Decrease the number of bytes in your default payload.
#define  nxErrDefaultPayloadNumBytes         (NX_ERROR_BASE | 0x11F)

//! You attempted to set a CAN arbitration ID with an invalid value. For
//! example, a CAN standard arbitration ID supports only 11 bits. If you attempt
//! to set a standard arbitration ID that uses more than 11 bits, this error is
//! returned. Solution: Use a valid arbitration ID.
#define nxErrInvalidArbitrationId            (NX_ERROR_BASE | 0x123)

//! You attempted to set a LIN ID with an invalid value. For example, a LIN ID
//! supports only 6 bits. If you attempt to set an ID that uses more than 6
//! bits, this error is returned. Solution: Use a valid LIN ID.
#define nxErrInvalidLinId                    (NX_ERROR_BASE | 0x124)

//! Too many open files. NI-XNET allows up to 7 database files to be opened
//! simultaneously. Solution: Open fewer files.
#define  nxErrTooManyOpenFiles               (NX_ERROR_BASE | 0x130)

//! Bad reference has been passed to a database function, e.g. a session
//! reference, or frame reference to retrieve properties from a signal.
#define  nxErrDatabaseBadReference           (NX_ERROR_BASE | 0x131)

//! Creating a database file failed. Solution: Verify access rights to the
//! destination directory or check if overwritten file has read only permission.
#define  nxErrCreateDatabaseFile             (NX_ERROR_BASE | 0x132)

//! A cluster with the same name already exists in the database. Solution: Use
//! another name for this cluster.
#define  nxErrDuplicateClusterName           (NX_ERROR_BASE | 0x133)

//! A frame with the same name already exists in the cluster. Solution: Use
//! another name for this frame.
#define  nxErrDuplicateFrameName             (NX_ERROR_BASE | 0x134)

//! A signal with the same name already exists in the frame. Solution: Use
//! another name for this signal.
#define  nxErrDuplicateSignalName            (NX_ERROR_BASE | 0x135)

//! An ECU with the same name already exists in the cluster. Solution: Use
//! another name for this ECU.
#define  nxErrDuplicateECUName               (NX_ERROR_BASE | 0x136)

//! A subframe with the same name already exists in the frame. Solution: Use
//! another name for this subframe.
#define  nxErrDuplicateSubframeName          (NX_ERROR_BASE | 0x137)

//! The operation is improper for the protocol in use, e.g. you cannot assign
//! FlexRay channels to a CAN frame.
#define  nxErrImproperProtocol               (NX_ERROR_BASE | 0x138)

//! Wrong parent relationship for a child that you are creating with XNET
//! Database Create.
#define  nxErrObjectRelation                 (NX_ERROR_BASE | 0x139)

//! The retrieved required property is not defined on the specified object.
//! Solution: Make sure that your database file has this property defined or
//! that you set it in the objects created in memory.
#define  nxErrUnconfiguredRequiredProperty   (NX_ERROR_BASE | 0x13B)

//! The feature is not supported under LabVIEW RT, e.g.Save Database
#define  nxErrNotSupportedOnRT               (NX_ERROR_BASE | 0x13C)

//! The object name contains unsupported characters. The name must contain just
//! alphanumeric characters and the underscore, but cannot begin with a digit.
//! The maximum size is 128.
#define  nxErrNameSyntax                     (NX_ERROR_BASE | 0x13D)

//! Unsupported database format. For reading a database, the extension must be
//! .xml, .dbc, .ncd, or .ldf. For saving, the extension must be .xml or .ldf
#define  nxErrFileExtension                  (NX_ERROR_BASE | 0x13E)

//! Database object not found, e.g. an object with given name doesn't exist.
#define  nxErrDatabaseObjectNotFound         (NX_ERROR_BASE | 0x13F)

//! Database cache file cannot be removed or replaced on the disc, e.g. it is
//! write-protected.
#define  nxErrRemoveDatabaseCacheFile        (NX_ERROR_BASE | 0x140)

//! You are trying to write a read-only property, e.g. the mux value on a signal
//! is a read only property (can be changed on the subframe).
#define  nxErrReadOnlyProperty               (NX_ERROR_BASE | 0x141)

//! You are trying to change a signal to be a mux signal, but a mux is already
//! defined in this frame
#define  nxErrFrameMuxExists                 (NX_ERROR_BASE | 0x142)

//! You are trying to define FlexRay in-cycle-repetition slots before defining
//! the first slot. Define the first slot (frame ID) before defining
//! in-cycle-repetition slots.
#define  nxErrUndefinedFirstSlot             (NX_ERROR_BASE | 0x144)

//! You are trying to define FlexRay in-cycle-repetition channels before
//! defining the first channels. Define the Channel Assignment on a frame before
//! defining in-cycle-repetition channels.
#define  nxErrUndefinedFirstChannels         (NX_ERROR_BASE | 0x145)

//! You must define the protocol before setting this property, e.g. the frame ID
//! has a different meaning in a CAN or FlexRay cluster.
#define  nxErrUndefinedProtocol              (NX_ERROR_BASE | 0x146)

//! The database information on the real-time system has been created with an
//! older NI-XNET version. This version is no longer supported. To correct this
//! error, re-deploy your database to the real-time system.
#define  nxErrOldDatabaseCacheFile           (NX_ERROR_BASE | 0x147)

//! Frame ConfigStatus: A signal within the frame exceeds the frame boundaries
//! (Payload Length).
#define  nxErrDbConfigSigOutOfFrame          (NX_ERROR_BASE | 0x148)

//! Frame ConfigStatus: A signal within the frame overlaps another signal.
#define  nxErrDbConfigSigOverlapped          (NX_ERROR_BASE | 0x149)

//! Frame ConfigStatus: A integer signal within the frame is defined with more
//! than 52 bits. Not supported.
#define  nxErrDbConfigSig52BitInteger        (NX_ERROR_BASE | 0x14A)

//! Frame ConfigStatus: Frame is defined with wrong number of bytes Allowed
//! values: - CAN: 0-8, - Flexray: 0-254 and even number.
#define  nxErrDbConfigFrameNumBytes          (NX_ERROR_BASE | 0x14B)

//! You are trying to add transmitted FlexRay frames to an ECU, with at least
//! two of them having Startup or Sync property on. Only one Sync or Startup
//! frame is allowed to be sent by an ECU.
#define  nxErrMultSyncStartup                (NX_ERROR_BASE | 0x14C)

//! You are trying to add TX/RX frames to an ECU which are defined in a
//! different cluster than the ECU.
#define  nxErrInvalidCluster                 (NX_ERROR_BASE | 0x14D)

//! Database name parameter is incorrect. Solution: Use a valid name for the
//! database, e.g. ":memory:" for in-memory database.
#define  nxErrDatabaseName                   (NX_ERROR_BASE | 0x14E)

//! Database object is locked because it is used in a session. Solution:
//! Configure the database before using it in a session.
#define  nxErrDatabaseObjectLocked           (NX_ERROR_BASE | 0x14F)

//! Alias name passed to a function is not defined. Solution: Define the alias
//! before calling the function.
#define nxErrAliasNotFound                   (NX_ERROR_BASE | 0x150)

//! Database file cannot be saved because frames are assigned to FlexRay
//! channels not defined in the cluster. Solution: Verify that all frames in the
//! FlexRay cluster are assigned to an existing cluster channel.
#define nxErrClusterFrameChannelRelation     (NX_ERROR_BASE | 0x151)

//! Frame ConfigStatus: This FlexRay frame transmitted in a dynamic segment uses
//! both channels A and B. This is not allowed. Solution: Use either channel A
//! or B.
#define nxErrDynFlexRayFrameChanAandB        (NX_ERROR_BASE | 0x152)

//! Database is locked because it is being modified by an another instance of
//! the same application. Solution: Close the database in the other application
//! instance.
#define  nxErrDatabaseLockedInUse            (NX_ERROR_BASE | 0x153)

//! A frame name is ambiguous, e.g. a frame with the same name exists in another
//! cluster. Solution: Specify the cluster name for the frame using the required
//! syntax.
#define  nxErrAmbiguousFrameName             (NX_ERROR_BASE | 0x154)

//! A signal name is ambiguous, e.g. a signal with the same name exists in
//! another frame. Solution: Use [frame].[signal] syntax for the signal.
#define  nxErrAmbiguousSignalName            (NX_ERROR_BASE | 0x155)

//! An ECU name is ambiguous, e.g. an ECU with the same name exists in another
//! cluster. Solution: Specify the cluster name for the ECU using the required
//! syntax.
#define  nxErrAmbiguousECUName               (NX_ERROR_BASE | 0x156)

//! A subframe name is ambiguous, e.g. a subframe with the same name exists in
//! another cluster. Solution: Specify the cluster name for the subframe using
//! the required syntax.
#define  nxErrAmbiguousSubframeName          (NX_ERROR_BASE | 0x157)

//! A LIN schedule name is ambiguous, e.g. a schedule with the same name exists
//! in another cluster. Solution: Specify the cluster name for the schedule
//! using the required syntax.
#define  nxErrAmbiguousScheduleName          (NX_ERROR_BASE | 0x158)

//! A LIN schedule with the same name already exists in the database. Solution:
//! Use another name for this schedule.
#define  nxErrDuplicateScheduleName          (NX_ERROR_BASE | 0x159)

//! Multiplexers (mode-dependent signals) are not supported when the given
//! protocol is used. Solution: Contact National Instruments to see whether
//! there is a newer NI-XNET version that supports multiplexers for the given
//! protocol.
#define  nxErrProtocolMuxNotSupported        (NX_ERROR_BASE | 0x15A)

//! Saving a FIBEX file containing a LIN cluster is not supported in this
//! NI-XNET version. Solution: Contact National Instruments to see whether there
//! is a newer NI-XNET version that supports saving a FIBEX file that contains a
//! LIN cluster.
#define  nxErrSaveLINnotSupported            (NX_ERROR_BASE | 0x15B)

//! This property requires an ECU configured as LIN master to be present in this
//! cluster. Solution: Create a LIN master ECU in this cluster.
#define  nxErrLINmasterNotDefined            (NX_ERROR_BASE | 0x15C)

//! You have tried to create a session using a frame that is incompatible with
//! the selected session type. For example, you are using a LIN diagnostic frame
//! with a single point output session.
#define nxErrSessionTypeFrameIncompatibility (NX_ERROR_BASE | 0x15D)

//! You cannot mix open of NI-XNET database objects as both manual and
//! automatic. You open manually by calling the Database Open VI. You open
//! automatically when you 1) wire the IO name directly to a property node or
//! VI, 2) branch a wire to multiple data flows on the diagram, 3) use the IO
//! name with a VI or property node after closing it with the Database Close VI.
//! Solution: Change your diagram to use the manual technique in all locations
//! (always call Open and Close VIs), or to use the automatic technique in all
//! locations (never call Open or Close VIs).
#define nxErrMixAutoManualOpen               (NX_ERROR_BASE | 0x15E)

//! Due to problems in LabVIEW versions 8.5 through 8.6.1, automatic open of
//! NI-XNET database objects is not supported. You open automatically when you
//! 1) wire the IO name directly to a property node or VI, 2) branch a wire to
//! multiple data flows on the diagram, 3) use the IO name with a VI or property
//! node after closing it with the Database Close VI. Solution: Change your
//! diagram to call the Database Open VI prior to any use (VI or property node)
//! in a data flow (including a new wire branch). Change your diagram to call
//! the Database Close VI when you are finished using the database in your
//! application.
#define nxErrAutoOpenNotSupported            (NX_ERROR_BASE | 0x15F)

//! You called a Write function with the number of array elements (frames or
//! signals) different than the number of elements configured in the session
//! (such as the "list" parameter of the Create Session function). Solution:
//! Write the same number of elements as configured in the session.
#define nxErrWrongNumSignalsWritten          (NX_ERROR_BASE | 0x160)

//! You used XNET session from multiple LabVIEW projects (or multiple
//! executables), which NI-XNET does not support. Solution: Run XNET sessions in
//! only one LabVIEW project at a time.
#define nxErrMultipleLvProject               (NX_ERROR_BASE | 0x161)

//! When an XNET session is used at runtime, all sessions in the same scope are
//! created on the interface. The same scope is defined as all sessions within
//! the same LabVIEW project which use the same cluster and interface (same
//! physical cable configuration). If you attempt to use a session in the same
//! scope after running the VI, this error occurs. The most likely cause is that
//! you added a new session, and tried to use that new session in a running VI.
//! Solution: Configure all session in LabVIEW project, then run the VI(s) that
//! use those sessions.
#define nxErrSessionConflictLvProject        (NX_ERROR_BASE | 0x162)

//! You used an empty name for an XNET database object (database, cluster, ECU,
//! frame, or signal). Empty name is not supported. Solution: Refer to NI-XNET
//! help for IO names to review the required syntax for the name, and change
//! your code to use that syntax.
#define nxErrDbObjectNameEmpty               (NX_ERROR_BASE | 0x163)

//! You used a name for an XNET database object (such as frame or signal) that
//! did not include a valid cluster selection. Solution: Refer to the NI-XNET
//! help for the IO name that you are using, and use the syntax specified for
//! that class, which includes the cluster selection.
#define nxErrMissingAliasInDbObjectName      (NX_ERROR_BASE | 0x164)

//! Unsupported database file version.
//! Solution: Use only database versions that are supported by this version of
//! NI-XNET. Please see the NI-XNET documentation for information on which
//! database versions are currently supported.
#define nxErrDatabaseImportVersion           (NX_ERROR_BASE | 0x165)
#define nxErrFibexImportVersion              (nxErrDatabaseImportVersion)

//! You used an empty name for the XNET Session. Empty name is not supported.
//! Solution: Use a valid XNET session name from your LabVIEW project.
#define nxErrEmptySessionName                (NX_ERROR_BASE | 0x166)

//! There is not enough message RAM on the FlexRay hardware to configure the
//! data partition for the object(s). Solution: Please refer to the manual for
//! limitations on the number of objects that can be created at any given time
//! based on the payload length.
#define nxErrNotEnoughMessageRAMForObject     (NX_ERROR_BASE | 0x167)

//! The FlexRay keyslot ID has been configured and a startup session has been
//! created. Either the keyslot ID needs to be configured OR the startup session
//! needs to be created. Both cannot exist at the same time. Solution: Choose a
//! single method to configure startup sessions in your application.
#define nxErrKeySlotIDConfig                 (NX_ERROR_BASE | 0x168)

//! An unsupported session was created. For example, stream output is not
//! supported on FlexRay hardware. Solution: Only use supported sessions in your
//! application.
#define nxErrUnsupportedSession         (NX_ERROR_BASE | 0x169)


//! An XNET session was created after starting the Interface. Only the Stream
//! Input session in the subordinate mode can be created after the Interface has
//! started. Solution: Create sessions prior to starting the XNET Interface in
//! your application.
#define nxErrObjectCreatedAfterStart         (NX_ERROR_BASE | 0x170)

//! The Single Slot property was enabled on the XNET FlexRay Interface after the
//! interface had started. Solution: Enable the Single Slot property prior to
//! starting the XNET FlexRay Interface.
#define nxErrSingleSlotEnabledAfterStart      (NX_ERROR_BASE | 0x171)

//! The FlexRay macrotick offset specified for XNET Create Timing Source is
//! unsupported. Example: Specifying a macrotick offset greater than
//! MacroPerCycle will result in this error. Solution: Specify a macrotick
//! offset within the supported range for the cluster.
#define nxErrUnsupportedNumMacroticks         (NX_ERROR_BASE | 0x172)

//! You used invalid syntax in the name of a database object (signal, frame, or
//! ECU). For example, you may have specified a frame's name as
//! [cluster].[frame], which is allowed in NI-XNET for C/C++, but not NI-XNET
//! for LabVIEW. Solution: Use the string syntax specified in the help topic for
//! the XNET I/O name class you are using.
#define nxErrBadSyntaxInDatabaseObjectName    (NX_ERROR_BASE | 0x173)

//! A LIN schedule entry name is ambiguous, e.g. a schedule entry with the same
//! name exists in another schedule. Solution: Specify the schedule name for the
//! schedule entry using the required syntax.
#define  nxErrAmbiguousScheduleEntryName      (NX_ERROR_BASE | 0x174)

//! A LIN schedule entry with the same name already exists in the schedule.
//! Solution: Use another name for this schedule entry.
#define  nxErrDuplicateScheduleEntryName      (NX_ERROR_BASE | 0x175)

//! At least one of the frames in the session has an undefined identifier.
//! Solution: Set the frame's "Identifier (Slot)" property before creating the
//! session.
#define  nxErrUndefinedFrameId                (NX_ERROR_BASE | 0x176)

//! At least one of the frames in the session has an undefined payload length.
//! Solution: Set the frame's "Payload Length (in bytes)" property before
//! creating the session.
#define  nxErrUndefinedFramePayloadLength     (NX_ERROR_BASE | 0x177)

//! At least one of the signals in the session has an undefined start bit.
//! Solution: Set the "Start Bit" property of the signal before creating the
//! session.
#define  nxErrUndefinedSignalStartBit         (NX_ERROR_BASE | 0x178)

//! At least one of the signals in the session has an undefined number of bits.
//! Solution: Set the "Number of Bits" property of the signal before creating
//! the session.
#define  nxErrUndefinedSignalNumBits          (NX_ERROR_BASE | 0x179)

//! At least one of the signals in the session has an undefined byte order.
//! Solution: Set the "Byte Order" property of the signal before creating the
//! session.
#define  nxErrUndefinedSignalByteOrder        (NX_ERROR_BASE | 0x17A)

//! At least one of the signals in the session has an undefined data type.
//! Solution: Set the "Data Type" property of the signal before creating the
//! session.
#define  nxErrUndefinedSignalDataType         (NX_ERROR_BASE | 0x17B)

//! At least one of the subframes in the session has an undefined multiplexer
//! value. Solution: Set the "Multiplexer Value" property of the subframe before
//! creating the session.
#define  nxErrUndefinedSubfMuxValue           (NX_ERROR_BASE | 0x17C)

//! You provided an invalid index to Write (State LIN Schedule Change).
//! Solution: Use a number from to N-1, where N is the number of LIN schedules
//! returned from the cluster property LIN Schedules. If you are using LabVIEW,
//! the string for the number must be decimal (not hexadecimal).
#define  nxErrInvalidLinSchedIndex            (NX_ERROR_BASE | 0x17D)

//! You provided an invalid name to Write (State LIN Schedule Change). Solution:
//! Use a valid LIN schedule name returned from the cluster property LIN
//! Schedules, or the session property Interface LIN Schedules. You can use the
//! short name (schedule only) or long name (schedule plus database and
//! cluster).
#define  nxErrInvalidLinSchedName             (NX_ERROR_BASE | 0x17E)

//! You provided an invalid active index for the session property.
#define  nxErrInvalidActiveFrameIndex         (NX_ERROR_BASE | 0x17F)

//! You provided an invalid name for Frame:Active of the session property node.
//! Solution: Use a valid item name from the session's List property. You can
//! use the short name (frame or signal only) or long name (frame/signal plus
//! database and cluster).
#define  nxErrInvalidActiveFrameName          (NX_ERROR_BASE | 0x180)

//! The database you are using requires using PDUs, and the operation is
//! ambiguous with respect to PDUs. Example: You are trying to get the frame
//! parent of the signal, but the PDU in which the signal is contained is
//! referenced in multiple frames.
#define  nxErrAmbiguousPDU                    (NX_ERROR_BASE | 0x181)

//! A PDU with the same name already exists in the cluster. Solution: Use
//! another name for this PDU.
#define  nxErrDuplicatePDU                    (NX_ERROR_BASE | 0x182)

//! You are trying to assign start bits or update bits to PDUs referenced in a
//! frame, but the number of elements in this array is different than the number
//! of referenced PDUs. Solution: Use the same number of elements in the array
//! as in the PDU references array.
#define  nxErrNumberOfPDUs                    (NX_ERROR_BASE | 0x183)

//! The configuration of this object requires using advanced PDUs, which the
//! given protocol does not support. Solution: You cannot use this object in the
//! given protocol.
#define  nxErrPDUsRequired                    (NX_ERROR_BASE | 0x184)

//! The maximum number of PDUs has been exceeded. Solution: Use fewer PDUs in
//! your sessions.
#define nxErrMaxPDUs                          (NX_ERROR_BASE | 0x185)

//! This mode value is not currently supported. Solution: Use a valid value.
#define nxErrUnsupportedMode                  (NX_ERROR_BASE | 0x186)

//! The firmware image on your XNET hardware is corrupted. Solution: Update the
//! firmware of this XNET hardware in MAX.
#define nxErrBadFpgaSignature                 (NX_ERROR_BASE | 0x187)
#define nxErrBadcSeriesFpgaSignature          (nxErrBadFpgaSignature)

//! The firmware version of your XNET hardware is not in sync with your host
//! computer. Solution: Update the firmware of this XNET hardware in MAX.
#define nxErrBadFpgaRevision                  (NX_ERROR_BASE | 0x188)
#define nxErrBadcSeriesFpgaRevision           (nxErrBadFpgaRevision)

//! The firmware version of your XNET C Series module is not in sync with the
//! NI-XNET software on your remote target. Solution: Update the NI-XNET
//! software on the remote target.
#define nxErrBadFpgaRevisionOnTarget          (NX_ERROR_BASE | 0x189)

//! The terminal you are trying to use is already in use. Only one connection
//! per terminal is allowed. Solution: disconnect the terminal that is already
//! in use.
#define nxErrRouteInUse                       (NX_ERROR_BASE | 0x18A)

//! You need to install a supported version of NI-DAQmx for your XNET C Series
//! module to work correctly with your Compact DAQ system. Solution: Check the
//! NI-XNET readme file for supported versions of the NI-DAQmx driver software.
#define nxErrDAQmxIncorrectVersion            (NX_ERROR_BASE | 0x18B)

//! Unable to create the requested route. This may be caused by a routing
//! conflict or an invalid terminal name. Solution: Fix invalid terminal names,
//! such as a blank string. Since NI-XNET relies on the NI-DAQmx driver software
//! to create routes on Compact DAQ chassis, use DAQmx to resolve routing
//! conflicts.
#define nxErrAddRoute                         (NX_ERROR_BASE | 0x18C)

//! You attempted to transmit a go to sleep frame (by setting the LIN Sleep mode
//! to Remote Sleep) on a LIN interface configured as slave. In conformance with
//! the LIN protocol standard, only an interface configured as master may
//! transmit a go to sleep frame.
#define nxErrRemoteSleepOnLinSlave            (NX_ERROR_BASE | 0x18D)

//! You attempted to set properties related to Sleep and Wakeup when the FlexRay
//! cluster defined in the Fibex file does not support it. Solution: Edit the
//! Fibex file used in your application to include all relevant cluster wakeup
//! attributes.
#define nxErrSleepWakeupNotSupported          (NX_ERROR_BASE | 0x18E)

//! A LIN diagnostic schedule change requires the diagnostic schedule to be
//! defined in the database. Solution: Define the diagnostic schedule in the
//! database.
#define  nxErrDiagnosticScheduleNotDefined   (NX_ERROR_BASE | 0x18F)

//! The data payload written for a diagnostic frame for transmit does not
//! conform to the LIN transport layer specification. Solution: Ensure the data
//! payload for a diagnostic frame conforms to the transport layer
//! specification.
#define nxErrLINTransportLayer                (NX_ERROR_BASE | 0x192)

//! An error occurred within the NI-XNET example code for logfile access (TDMS).
//! Solution: For LabVIEW, the subVI with the error is shown as the source, and
//! you can open that subVI to determine the cause of the problem. For other
//! programming languages, review the source code for the logfile example to
//! determine the cause of the problem.
#define nxErrLogfile                          (NX_ERROR_BASE | 0x193)

//! You attempted to write a LIN schedule and use a stream output replay timing
//! mode concurrently. You can only use the stream output immediate timing mode
//! cuncurrently with the LIN scheduler.
#define nxErrStrmOutTmgLinSchedulerConflict   (NX_ERROR_BASE | 0x200)

//! You attempted to create a session that is incompatible with the LIN
//! interface personality (master or slave), or set the LIN interface
//! personality to one that is incompatible with a session already created for
//! it. For example, setting the LIN interface to slave after creating a stream
//! output session will report this error, because only LIN interface as master
//! supports stream output.
#define nxErrSessnTypeLinIntfPrsIncompatible (NX_ERROR_BASE | 0x201)

//! You attempted to save an LDF or DBC database, but the passed reference is
//! not a database cluster. Solution: A cluster reference must be used to
//! specify the cluster you want to export.
#define nxErrSaveClusterOnly                 (NX_ERROR_BASE | 0x202)
//! Need to define for compatibility with older versions
#define nxErrSaveLdfClusterOnly              nxErrSaveClusterOnly

//! You tried to assign the same interface name twice. This is not permitted.
//! Solution: Assign a unique name to an interface.
#define nxErrDuplicateInterfaceName           (NX_ERROR_BASE | 0x203)

//! Transceiver cable hardware revision is too new. The current driver does not
//! support this transceiver cable. Solution: Upgrade the NI-XNET driver.
#define  nxErrIncompatiableTransceiverRevision    (NX_ERROR_BASE | 0x204)

//! Transceiver cable image revision is too new. The current driver does not
//! support this transceiver cable. Solution: Upgrade the NI-XNET driver or
//! downgrade the image on the transceiver cable.
#define  nxErrIncompatiableTransceiverImage     (NX_ERROR_BASE | 0x205)

//! The property does not apply to this type of hardware. For example, attempting
//! to access dongle revision properties on XNET devices that do not support
//! Transceiver Cables will result in this error. Solution: Do not apply
//! the property to this type of hardware.
#define  nxErrPropertyNotsupported              (NX_ERROR_BASE | 0x206)

//! Exporting cluster into the specified database type failed. Solution: Ensure
//! the database configuration is complete. Refer to the standard documentation
//! for the related file format.
#define nxErrExport_Semantic                 (NX_ERROR_BASE | 0x207)

//! A J1939 input queue overflowed. Reading large J1939 frames can make the
//! queue overflow, and the Read function delivers fewer frames then specified.
//! Solution: Call the Read function again to read the remaining frames.
#define nxErrJ1939QueueOverflow                 (NX_ERROR_BASE | 0x0208)

//! You are trying to transmit a non-J1939 frame with more than 8 bytes. Only
//! J1939 frames can use the J1939 transport protocol. Solution: Verify the
//! transport protocol property on the frame in the database.
#define nxErrNonJ1939FrameSize                  (NX_ERROR_BASE | 0x0209)

//! You are trying to transmit a J1939 frame, but no J1939 address is assigned
//! to the session. Solution: Set the address using the J1939 address property.
#define nxErrJ1939MissingAddress                (NX_ERROR_BASE | 0x020A)

//! The received J1939 TP.CM_CTS message has the wrong total size.
#define nxErrJ1939AddressLost                   (NX_ERROR_BASE | 0x020B)

//! The next packet value of the received J1939 TP.CM_CTS message is larger than
//! the total number of packets.
#define nxErrJ1939CtsNextPckLargerTotalPckNum   (NX_ERROR_BASE | 0x020C)

//! The received J1939 TP.CM_CTS message has a number of packets of 0, but the
//! next packet number is not 255.
#define nxErrJ1939CtsNextPck                    (NX_ERROR_BASE | 0x020D)

//! The received J1939 TP.CM_CTS message has a next packet value of 0
#define nxErrJ1939CtsNextPckNull                (NX_ERROR_BASE | 0x020E)

//! The received J1939 TP.CM_CTS message does not have the same PGN as in the
//! TP.CM_RTS message.
#define nxErrJ1939CtsPgn                        (NX_ERROR_BASE | 0x020F)

//! Received unexpected sequence number in the J1939 TP.DT message.
#define nxErrJ1939UnexpectedSeqNum              (NX_ERROR_BASE | 0x0210)

//! More Packets are requested than allowed in the J1939 TP.CM_CTS message.
#define nxErrJ1939MorePckReqThanAllowed         (NX_ERROR_BASE | 0x0211)

//! J1939 Timeout T1 while waiting for data.
#define nxErrJ1939TimeoutT1                     (NX_ERROR_BASE | 0x0212)

//! J1939 Timeout T2 while waiting for data.
#define nxErrJ1939TimeoutT2                     (NX_ERROR_BASE | 0x0213)

//! J1939 Timeout T3 while waiting for TP.CM_CTS or TP.CM_EndOfMsgAck.
#define nxErrJ1939TimeoutT3                     (NX_ERROR_BASE | 0x0214)

//! J1939 Timeout T4 while waiting for next CTS MSG.
#define nxErrJ1939TimeoutT4                     (NX_ERROR_BASE | 0x0215)

//! Received wrong DLC in the J1939 TP.CM_RTS message. DLC must be 8.
#define nxErrJ1939RtsDlc                        (NX_ERROR_BASE | 0x0216)

//! Received wrong DLC in the J1939 TP.CM_CTS message. DLC must be 8.
#define nxErrJ1939CtsDlc                        (NX_ERROR_BASE | 0x0217)

//! Received wrong DLC in the J1939 TP.CM_BAM message. DLC must be 8.
#define nxErrJ1939BamDlc                        (NX_ERROR_BASE | 0x0218)

//! Received wrong DLC in the J1939 TP.DT message. DLC must be 8.
#define nxErrJ1939DtDlc                         (NX_ERROR_BASE | 0x0219)

//! Received wrong DLC in the J1939 TP.CM_Abort message. DLC must be 8.
#define nxErrJ1939AbortDlc                      (NX_ERROR_BASE | 0x021A)

//! Received wrong DLC in the J1939 TP.CM_EndOfMsgAck message. DLC must be 8.
#define nxErrJ1939EomaDlc                       (NX_ERROR_BASE | 0x021B)

//! Received wrong PGN in the J1939 TP.CM_Abort message.
#define nxErrJ1939AbortPgn                      (NX_ERROR_BASE | 0x021C)

//! Internal error occurred for send TP.CM_CTS Hold Message.
#define nxErrJ1939CtsHoldMsg                    (NX_ERROR_BASE | 0x021D)

//! Invalid total message size in J1939 TP.CM_RTS message. Expect 9..1785.
#define nxErrJ1939InvalidTotalSize              (NX_ERROR_BASE | 0x021E)

//! Total number of packets in received J1939 TP.CM_RTS message must be greater
//! than 1.
#define nxErrJ1939TotalPckNum                   (NX_ERROR_BASE | 0x021F)

//! Reserved data bytes in J1939 received message are not BFF63FF.
#define nxErrJ1939ReservedData                  (NX_ERROR_BASE | 0x0220)

//! Not enough system resources for the J1939 Transport Protocol.
#define nxErrJ1939NotEnoughSysRes               (NX_ERROR_BASE | 0x0221)

//! Received J1939 TP.CM_Abort message with reason ActiveConnection: Already in
//! one or more connection managed sessions and cannot support another.
#define nxErrJ1939AbortMsgActiveConnection      (NX_ERROR_BASE | 0x0222)

//! Received J1939 TP.CM_Abort message with reason NotEnoughSystemResources:
//! System resources were needed for another task, so this connection managed
//! session was terminated.
#define nxErrJ1939AbortMsgNotEnoughSysRes       (NX_ERROR_BASE | 0x0223)

//! Received J1939 TP.CM_Abort message with reason Timeout: A timeout occurred,
//! and this is the connection abort to close the session.
#define nxErrJ1939AbortMsgTimeout               (NX_ERROR_BASE | 0x0224)

//! Received J1939 TP.CM_Abort message with reason CtsReceived: CTS messages
//! received when data transfer is in progress.
#define nxErrJ1939AbortMsgCtsRec                (NX_ERROR_BASE | 0x0225)

//! Received J1939 TP.CM_Abort message with reason MaxRetransmit: Maximum
//! retransmit request limit reached.
#define nxErrJ1939AbortMsgMaxRetransmit         (NX_ERROR_BASE | 0x0226)

//! Remote communication with the LabVIEW RT target failed because the host and
//! target versions of NI-XNET are different. Solution: On the target, install
//! the same NI-XNET version that is installed on the host.
#define nxErrRPCVersion                         (NX_ERROR_BASE | 0x0227)

//! The CAN frame I/O mode is higher than the CAN cluster I/O mode. This frame
//! cannot be transmitted on the network. Solution: Change the frame or cluster
//! I/O mode.
#define nxErrFrameCanIoMode                     (NX_ERROR_BASE | 0x0228)

//! The current driver cannot update the firmware on your hardware. Solution:
//! Ask National Instruments for compatible driver software.
#define nxErrIncompatibleFlash                  (NX_ERROR_BASE | 0x0229)

//! You are trying to use the CAN Transmit I/O Mode (TxIoMode) property in an
//! unsupported interface mode. Solution: You can use this property in only
//! non-ISO or ISO Legacy mode.
#define nxErrTxIoMode                           (NX_ERROR_BASE | 0x022A)

//! You are trying to use the XS Transceiver Cable on unsupported hardware. This
//! currently requires a PXIe-8510 board.
#define nxErrXsDongleUnsupportedBoard           (NX_ERROR_BASE | 0x022B)

//! You are trying to use a database alias name that contains an invalid
//! character (for example, a comma).
#define nxErrInvalidCharInDatabaseAlias         (NX_ERROR_BASE | 0x022C)

//! You are trying to use a database filepath that contains an invalid character
//! (for example, a comma).
#define nxErrInvalidCharInDatabaseFilepath      (NX_ERROR_BASE | 0x022D)

//! You are trying to use CAN FD with a non-HS/FD port. CAN FD is supported with
//! High Speed CAN only.
#define nxErrInvalidCanFdPortType               (NX_ERROR_BASE | 0x022E)

//! An unconditional LIN schedule entry is incorrectly configured. To fix this
//! issue, reference only one frame in this entry.
#define nxErrInvUnconditionalEntry              (NX_ERROR_BASE | 0x022F)

//! An event LIN schedule entry has no collision resolving schedule assigned. To
//! fix this issue, assign a schedule to the schedule entry.
#define nxErrEventEntryNoSchedule               (NX_ERROR_BASE | 0x0230)

//! You have connected your USB device to a port that supports only Full Speed
//! (USB 1.1). NI-XNET USB devices require at least High Speed (USB 2.0+)
//! support for correct operation.
#define nxErrUnsupportedUsbSpeed                (NX_ERROR_BASE | 0x0231)

//! You added two or more frames with different payload sizes to
//! the same LIN event-triggered schedule entry.
//! Solution: Use an equal payload length for all event-triggered frames in an event-triggered
//! schedule entry.
#define nxErrEventUnequalPayloadLength          (NX_ERROR_BASE | 0x0232)

//! You added two or more frames with different checksum types to the same event-triggered schedule entry.
//! The LIN checksum type is determined by the LIN protocol version of the ECU transmitting the frame.
//! Solution: Use the same checksum model for all event-triggered frames in an event-triggered
//! schedule entry.
#define nxErrEventUnequalCheckSumType           (NX_ERROR_BASE | 0x0233)

//! The versions of the NI-XNET device driver and Compact RIO driver are
//! incompatible. Solution: Install drivers that are listed in the Recommended
//! Software Set.
#define nxErrCrioBadDriverVersions              (NX_ERROR_BASE | 0x0234)

//! The module support for this slot is missing features required for the
//! NI-XNET driver to be able to communicate with the module. Solution:
//! Load a bitfile to the chassis that contains support for NI-XNET modules for
//! the occupied slots.
#define nxErrCrioMissingSlotSupport             (NX_ERROR_BASE | 0x0235)

//! The interface you have attempted to use with NI-XNET Compatibility Library is already in use by NI-XNET.
//! Solution: Stop all applications running on the interface, or select one not in use by NI-XNET sessions.
#define nxErrIntfAlreadyInUseByNiXnet           (NX_ERROR_BASE | 0x0236)

//! The interface you have attempted to use with NI-XNET is already in use by NI-XNET Compatibility Library.
//! Solution: Stop all applications running on the interface, or select one not in use by the NI-XNET Compatibility
//! Library.
#define nxErrIntfAlreadyInUseByNiXcl            (NX_ERROR_BASE | 0x0237)

//! Byte Array signals are not allowed for this session type or function.
//! Solution: Select only numeric signals for the session.
#define nxErrByteArrayNotAllowed                (NX_ERROR_BASE | 0x0238)

//! Mixing of Byte Array signals and numeric signals is not allowed for this session type.
//! Solution: Select only numeric signals or Byte Array signals for the session.
#define nxErrNoByteArrayMix                     (NX_ERROR_BASE | 0x0239)

//! Only one Byte Array signal is allowed for this session type or function.
//! Solution: Select only one Byte Array signal for the session.
#define nxErrOnlyOneByteArray                   (NX_ERROR_BASE | 0x023A)

   // Warning Section

//! The CAN FD baud rate you supplied exceeds the capabilities the transceiver
//! manufacturer specified. In our internal testing, we have found this baud
//! rate to run, but bus errors may be detected or generated during
//! communication. Refer to the NI-XNET CAN Hardware Overview section in the
//! NI-XNET Hardware and Software Manual for more information.
#define nxWarnFdBaudExceedsCapability        (NX_WARNING_BASE | 0x040)

//! There is a warning from importing the database file. For details, refer to
//! the log files found in %LOCALAPPDATA%\National Instruments\NI-XNET\log.
//! Please note that this location may be hidden on your computer.
#define nxWarnDatabaseImport                 (NX_WARNING_BASE | 0x085)

//! The database file has been imported, but it was not created by the XNET
//! Editor or using the XNET API. Saving the database file with the XNET API or
//! XNET Editor may lose information from the original file.
#define nxWarnDatabaseImportFIBEXNoXNETFile  (NX_WARNING_BASE | 0x086)

//! The database file was not created by the XNET Editor or using the XNET API.
//! Additionally, there is another warning. For details, refer to the import log
//! file nixnetfx-log.txt under %LOCALAPPDATA%\National Instruments\NI-XNET\log.
//! Please note that this location may be hidden on your computer.
#define nxWarnDatabaseImportFIBEXNoXNETFilePlusWarning \
                                             (NX_WARNING_BASE | 0x087)

//! Close Database returns a warning instead of an error when an invalid
//! reference is passed to the function.
#define  nxWarnDatabaseBadReference          (NX_WARNING_BASE | 0x131)

//! Your are retrieving signals from a frame that uses advanced PDU
//! configuration. The signal start bit is given relative to the PDU, and it may
//! be different than the start bit relative to the frame.
#define  nxWarnAdvancedPDU                   (NX_WARNING_BASE | 0x132)

//! The multiplexer size exceeds 16 bit. This is not supported for Single Point
//! sessions.
#define  nxWarnMuxExceeds16Bit               (NX_WARNING_BASE | 0x133)


/***********************************************************************
                          P R O P E R T Y   I D S
***********************************************************************/

   // Class IDs used for encoding of property IDs (nxProp*)
   // Also class parameter of function nxdbCreateObject, nxdbDeleteObject, and nxdbFindObject
#define nxClass_Database                  (u32)0x00000000   //Database
#define nxClass_Cluster                   (u32)0x00010000   //Cluster
#define nxClass_Frame                     (u32)0x00020000   //Frame
#define nxClass_Signal                    (u32)0x00030000   //Signal
#define nxClass_Subframe                  (u32)0x00040000   //Subframe
#define nxClass_ECU                       (u32)0x00050000   //ECU
#define nxClass_LINSched                  (u32)0x00060000   //LIN Schedule
#define nxClass_LINSchedEntry             (u32)0x00070000   //LIN Schedule Entry
#define nxClass_PDU                       (u32)0x00080000   //PDU
#define nxClass_Session                   (u32)0x00100000   //Session
#define nxClass_System                    (u32)0x00110000   //System
#define nxClass_Device                    (u32)0x00120000   //Device
#define nxClass_Interface                 (u32)0x00130000   //Interface
#define nxClass_Alias                     (u32)0x00140000   //Alias

#define nxClass_Mask                      (u32)0x00FF0000   //mask for object class

   // Datatype IDs used in encoding of property IDs (nxProp*)
#define nxPrptype_u32                     (u32)0x00000000
#define nxPrptype_f64                     (u32)0x01000000
#define nxPrptype_bool                    (u32)0x02000000   // use u8 as datatype (semantic only)
#define nxPrptype_string                  (u32)0x03000000
#define nxPrptype_1Dstring                (u32)0x04000000   // comma-separated list
#define nxPrptype_ref                     (u32)0x05000000   // u32 reference (handle)
#define nxPrptype_1Dref                   (u32)0x06000000   // array of u32 reference
#define nxPrptype_time                    (u32)0x07000000   // nxTimestamp_t
#define nxPrptype_1Du32                   (u32)0x08000000   // array of u32 values
#define nxPrptype_u64                     (u32)0x09000000
#define nxPrptype_1Du8                    (u32)0x0A000000   // array of u8 values

#define nxPrptype_Mask                    (u32)0xFF000000   // mask for proptype
    /* PropertyId parameter of nxGetProperty, nxGetPropertySize, nxSetProperty functions. */

      // Session:ApplicationProtocol
#define nxPropSession_ApplicationProtocol       ((u32)0x00000091 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Auto Start?
#define nxPropSession_AutoStart                 ((u32)0x00000001 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:ClusterName
#define nxPropSession_ClusterName               ((u32)0x0000000A | nxClass_Session | nxPrptype_string)   //--r
      // Session:Database
#define nxPropSession_DatabaseName              ((u32)0x00000002 | nxClass_Session | nxPrptype_string)   //--r
      // Session:List of Signals / List of Frames
#define nxPropSession_List                      ((u32)0x00000003 | nxClass_Session | nxPrptype_1Dstring) //--r
      // Session:Mode
#define nxPropSession_Mode                      ((u32)0x00000004 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Number of Frames
#define nxPropSession_NumFrames                 ((u32)0x0000000D | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Number in List
#define nxPropSession_NumInList                 ((u32)0x00000005 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Number of Values Pending
#define nxPropSession_NumPend                   ((u32)0x00000006 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Number of Values Unused
#define nxPropSession_NumUnused                 ((u32)0x0000000B | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Payload Length Maximum
#define nxPropSession_PayldLenMax               ((u32)0x00000009 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Protocol
#define nxPropSession_Protocol                  ((u32)0x00000008 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Queue Size
#define nxPropSession_QueueSize                 ((u32)0x0000000C | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Resample Rate
#define nxPropSession_ResampRate                ((u32)0x00000007 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:Interface:Baud Rate (32-bit) Superseded by nxPropSession_IntfBaudRate64
#define nxPropSession_IntfBaudRate              ((u32)0x00000016 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:Baud Rate (64-bit)
#define nxPropSession_IntfBaudRate64            ((u32)0x00000016 | nxClass_Session | nxPrptype_u64)      //--rw
      // Session:Interface:Bus Error Frames to Input Stream?
#define nxPropSession_IntfBusErrToInStrm        ((u32)0x00000015 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:Echo Transmit?
#define nxPropSession_IntfEchoTx                ((u32)0x00000010 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:Name
#define nxPropSession_IntfName                  ((u32)0x00000013 | nxClass_Session | nxPrptype_string)   //--r
      // Session:Interface:Output Stream List
#define nxPropSession_IntfOutStrmList           ((u32)0x00000011 | nxClass_Session | nxPrptype_1Dref)    //--rw
      // Session:Interface:Output Stream Timing
#define nxPropSession_IntfOutStrmTimng          ((u32)0x00000012 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:Start Trigger Frames to Input Stream?
#define nxPropSession_IntfStartTrigToInStrm     ((u32)0x00000014 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:CAN:External Transceiver Configuration
#define nxPropSession_IntfCANExtTcvrConfig      ((u32)0x00000023 | nxClass_Session | nxPrptype_u32)      //--w
      // Session:Interface:CAN:Listen Only?
#define nxPropSession_IntfCANLstnOnly           ((u32)0x00000022 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:CAN:Pending Transmit Order
#define nxPropSession_IntfCANPendTxOrder        ((u32)0x00000020 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:CAN:Single Shot Transmit?
#define nxPropSession_IntfCANSingShot           ((u32)0x00000024 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:CAN:Termination
#define nxPropSession_IntfCANTerm               ((u32)0x00000025 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:CAN:Transceiver State
#define nxPropSession_IntfCANTcvrState          ((u32)0x00000028 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:CAN:Transceiver Type
#define nxPropSession_IntfCANTcvrType           ((u32)0x00000029 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:CAN:Output Stream List By ID
#define nxPropSession_IntfCANOutStrmListById    ((u32)0x00000021 | nxClass_Session | nxPrptype_1Du32)    //--rw
      // Session:Interface:CAN:CAN I/O Mode
#define nxPropSession_IntfCanIoMode             ((u32)0x00000026 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Interface:CAN:CAN FD Baudrate (32-bit) Superseded by nxPropSession_IntfCanFdBaudRate64
#define nxPropSession_IntfCanFdBaudRate         ((u32)0x00000027 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:CAN:CAN FD Baudrate (64-bit)
#define nxPropSession_IntfCanFdBaudRate64       ((u32)0x00000027 | nxClass_Session | nxPrptype_u64)      //--rw
      // Session:Interface:CAN:CAN TX I/O Mode
#define nxPropSession_IntfCanTxIoMode           ((u32)0x00000039 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:CAN:FD ISO Mode
#define nxPropSession_IntfCanFdIsoMode          ((u32)0x0000003E | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Accepted Startup Range
#define nxPropSession_IntfFlexRayAccStartRng    ((u32)0x00000030 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Allow Halt Due To Clock?
#define nxPropSession_IntfFlexRayAlwHltClk      ((u32)0x00000031 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:FlexRay:Allow Passive to Active
#define nxPropSession_IntfFlexRayAlwPassAct     ((u32)0x00000032 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Auto Asleep When Stopped
#define nxPropSession_IntfFlexRayAutoAslpWhnStp ((u32)0x0000003A | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:FlexRay:Cluster Drift Damping
#define nxPropSession_IntfFlexRayClstDriftDmp   ((u32)0x00000033 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Coldstart?
#define nxPropSession_IntfFlexRayColdstart      ((u32)0x00000034 | nxClass_Session | nxPrptype_bool)     //--r
      // Session:Interface:FlexRay:Decoding Correction
#define nxPropSession_IntfFlexRayDecCorr        ((u32)0x00000035 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Delay Compensation Ch A
#define nxPropSession_IntfFlexRayDelayCompA     ((u32)0x00000036 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Delay Compensation Ch B
#define nxPropSession_IntfFlexRayDelayCompB     ((u32)0x00000037 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Key Slot Identifier
#define nxPropSession_IntfFlexRayKeySlotID      ((u32)0x00000038 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Latest Tx
#define nxPropSession_IntfFlexRayLatestTx       ((u32)0x00000041 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Interface:FlexRay:Listen Timeout
#define nxPropSession_IntfFlexRayListTimo       ((u32)0x00000042 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Macro Initial Offset Ch A
#define nxPropSession_IntfFlexRayMacInitOffA    ((u32)0x00000043 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Macro Initial Offset Ch B
#define nxPropSession_IntfFlexRayMacInitOffB    ((u32)0x00000044 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Micro Initial Offset Ch A
#define nxPropSession_IntfFlexRayMicInitOffA    ((u32)0x00000045 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Micro Initial Offset Ch B
#define nxPropSession_IntfFlexRayMicInitOffB    ((u32)0x00000046 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Max Drift
#define nxPropSession_IntfFlexRayMaxDrift       ((u32)0x00000047 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Microtick
#define nxPropSession_IntfFlexRayMicrotick      ((u32)0x00000048 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Interface:FlexRay:Null Frames To Input Stream?
#define nxPropSession_IntfFlexRayNullToInStrm   ((u32)0x00000049 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:FlexRay:Offset Correction
#define nxPropSession_IntfFlexRayOffCorr        ((u32)0x00000058 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Interface:FlexRay:Offset Correction Out
#define nxPropSession_IntfFlexRayOffCorrOut     ((u32)0x00000050 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Rate Correction
#define nxPropSession_IntfFlexRayRateCorr       ((u32)0x00000059 | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Interface:FlexRay:Rate Correction Out
#define nxPropSession_IntfFlexRayRateCorrOut    ((u32)0x00000052 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Samples Per Microtick
#define nxPropSession_IntfFlexRaySampPerMicro   ((u32)0x00000053 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Single Slot Enabled?
#define nxPropSession_IntfFlexRaySingSlotEn     ((u32)0x00000054 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:FlexRay:Statistics Enabled?
#define nxPropSession_IntfFlexRayStatisticsEn   ((u32)0x0000005A | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:FlexRay:Symbol Frames To Input Stream?
#define nxPropSession_IntfFlexRaySymToInStrm    ((u32)0x0000003D | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:FlexRay:Sync on Ch A Even Cycle
#define nxPropSession_IntfFlexRaySyncChAEven    ((u32)0x0000005B | nxClass_Session | nxPrptype_1Du32)    //--r
      // Session:Interface:FlexRay:Sync on Ch A Odd Cycle
#define nxPropSession_IntfFlexRaySyncChAOdd     ((u32)0x0000005C | nxClass_Session | nxPrptype_1Du32)    //--r
      // Session:Interface:FlexRay:Sync on Ch B Even Cycle
#define nxPropSession_IntfFlexRaySyncChBEven    ((u32)0x0000005D | nxClass_Session | nxPrptype_1Du32)    //--r
      // Session:Interface:FlexRay:Sync on Ch B Odd Cycle
#define nxPropSession_IntfFlexRaySyncChBOdd     ((u32)0x0000005E | nxClass_Session | nxPrptype_1Du32)    //--r
      // Session:Interface:FlexRay:Sync Frame Status
#define nxPropSession_IntfFlexRaySyncStatus     ((u32)0x0000005F | nxClass_Session | nxPrptype_u32)      //--r
      // Session:Interface:FlexRay:Termination
#define nxPropSession_IntfFlexRayTerm           ((u32)0x00000057 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Wakeup Channel
#define nxPropSession_IntfFlexRayWakeupCh       ((u32)0x00000055 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Wakeup Pattern
#define nxPropSession_IntfFlexRayWakeupPtrn     ((u32)0x00000056 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:FlexRay:Sleep
#define nxPropSession_IntfFlexRaySleep          ((u32)0x0000003B | nxClass_Session | nxPrptype_u32)      //--w
      // Session:Interface:FlexRay:Connected Channels
#define nxPropSession_IntfFlexRayConnectedChs   ((u32)0x0000003C | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:LIN:Break Length
#define nxPropSession_IntfLINBreakLength        ((u32)0x00000070 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:LIN:Master?
#define nxPropSession_IntfLINMaster             ((u32)0x00000072 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:LIN:Schedule Names
#define nxPropSession_IntfLINSchedNames         ((u32)0x00000075 | nxClass_Session | nxPrptype_1Dstring) //--r
      // Session:Interface:LIN:Sleep
#define nxPropSession_IntfLINSleep              ((u32)0x00000073 | nxClass_Session | nxPrptype_u32)      //--w
      // Session:Interface:LIN:Termination
#define nxPropSession_IntfLINTerm               ((u32)0x00000074 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:Interface:LIN:Diagnostics P2min
#define nxPropSession_IntfLINDiagP2min          ((u32)0x00000077 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:Interface:LIN:Diagnostics STmin
#define nxPropSession_IntfLINDiagSTmin          ((u32)0x00000076 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:Interface:LIN:Allow Start Without Bus Power?
#define nxPropSession_IntfLINAlwStartWoBusPwr   ((u32)0x00000078 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:LIN:Output Stream Slave Response List By NAD
#define nxPropSession_IntfLINOStrSlvRspLstByNAD ((u32)0x00000079 | nxClass_Session | nxPrptype_1Du32)    //--rw
      // Session:Interface:LIN:No Response Frames to Input Stream?      
#define nxPropSession_IntfLINNoResponseToInStrm ((u32)0x00000080 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:LIN:Checksum to Input Stream?
#define nxPropSession_IntfLINChecksumToInStrm   ((u32)0x00000081 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:Source Terminal:Start Trigger
#define nxPropSession_IntfSrcTermStartTrigger   ((u32)0x00000090 | nxClass_Session | nxPrptype_string)   //--rw
      // Session:SAE J1939:Node Address
#define nxPropSession_J1939Address              ((u32)0x00000092 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:SAE J1939:Node Name
#define nxPropSession_J1939Name                 ((u32)0x00000094 | nxClass_Session | nxPrptype_u64)      //--rw
      // Session:SAE J1939:ECU
#define nxPropSession_J1939ECU                  ((u32)0x00000093 | nxClass_Session | nxPrptype_ref)      //--w
      // Session:SAE J1939:Timeout T1
#define nxPropSession_J1939TimeoutT1            ((u32)0x00000095 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Timeout T2
#define nxPropSession_J1939TimeoutT2            ((u32)0x00000096 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Timeout T3
#define nxPropSession_J1939TimeoutT3            ((u32)0x00000097 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Timeout T4
#define nxPropSession_J1939TimeoutT4            ((u32)0x00000098 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Response Time TrSD
#define nxPropSession_J1939ResponseTimeTrSD     ((u32)0x00000099 | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Response Time TrGD
#define nxPropSession_J1939ResponseTimeTrGD     ((u32)0x0000009A | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Hold Time Th
#define nxPropSession_J1939HoldTimeTh           ((u32)0x0000009B | nxClass_Session | nxPrptype_f64)      //--rw
      // Session:SAE J1939:Num Packets Received
#define nxPropSession_J1939NumPacketsRecv       ((u32)0x0000009C | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:SAE J1939:Num Packets Response
#define nxPropSession_J1939NumPacketsResp       ((u32)0x0000009D | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:SAE J1939:Max Repeat CTS
#define nxPropSession_J1939MaxRepeatCTS         ((u32)0x0000009E | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:SAE J1939:Fill Byte
#define nxPropSession_J1939FillByte             ((u32)0x0000009F | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:SAE J1939:Write Queue Size
#define nxPropSession_J1939WriteQueueSize       ((u32)0x000000A0 | nxClass_Session | nxPrptype_u32)      //--rw
      // Session:SAE J1939:ECU Busy
#define nxPropSession_J1939ECUBusy              ((u32)0x000000A1 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:CAN:Enable Edge Filter
#define nxPropSession_IntfCanEdgeFilter         ((u32)0x000000A2 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:CAN:Enable Transmit Pause
#define nxPropSession_IntfCanTransmitPause      ((u32)0x000000A3 | nxClass_Session | nxPrptype_bool)     //--rw
      // Session:Interface:CAN:Disable Protocol Exception Handling
#define nxPropSession_IntfCanDisableProtExceptionHandling   ((u32)0x000000A4 | nxClass_Session | nxPrptype_bool)     //--rw
      // System:Devices
#define nxPropSys_DevRefs                       ((u32)0x00000002 | nxClass_System | nxPrptype_1Dref)     //--r
      // System:Interfaces (All the Interface with known protocol)
#define nxPropSys_IntfRefs                      ((u32)0x00000003 | nxClass_System | nxPrptype_1Dref)     //--r
      // System:Interfaces (CAN)
#define nxPropSys_IntfRefsCAN                   ((u32)0x00000004 | nxClass_System | nxPrptype_1Dref)     //--r
      // System:Interfaces (FlexRay)
#define nxPropSys_IntfRefsFlexRay               ((u32)0x00000005 | nxClass_System | nxPrptype_1Dref)     //--r
      // System:Interfaces (LIN)
#define nxPropSys_IntfRefsLIN                   ((u32)0x00000007 | nxClass_System | nxPrptype_1Dref)     //--r
      // System:Version:Build
#define nxPropSys_VerBuild                      ((u32)0x00000006 | nxClass_System | nxPrptype_u32)       //--r
      // System:Version:Major
#define nxPropSys_VerMajor                      ((u32)0x00000008 | nxClass_System | nxPrptype_u32)       //--r
      // System:Version:Minor
#define nxPropSys_VerMinor                      ((u32)0x00000009 | nxClass_System | nxPrptype_u32)       //--r
      // System:Version:Phase
#define nxPropSys_VerPhase                      ((u32)0x0000000A | nxClass_System | nxPrptype_u32)       //--r
      // System:Version:Update
#define nxPropSys_VerUpdate                     ((u32)0x0000000B | nxClass_System | nxPrptype_u32)       //--r
      // System:CompactDAQ Packet Time
#define nxPropSys_CDAQPktTime                   ((u32)0x0000000C | nxClass_System | nxPrptype_f64)       //--rw
      // System:All the interfaces
#define nxPropSys_IntfRefsAll                   ((u32)0x0000000D | nxClass_System | nxPrptype_1Dref)     //--r
// Device:Form Factor
#define nxPropDev_FormFac                       ((u32)0x00000001 | nxClass_Device | nxPrptype_u32)       //--r
      // Device:Interfaces with known protocol
#define nxPropDev_IntfRefs                      ((u32)0x00000002 | nxClass_Device | nxPrptype_1Dref)     //--r
      // Device:Name
#define nxPropDev_Name                          ((u32)0x00000003 | nxClass_Device | nxPrptype_string)    //--r
      // Device:Number of Ports with known protocol
#define nxPropDev_NumPorts                      ((u32)0x00000004 | nxClass_Device | nxPrptype_u32)       //--r
      // Device:Product Number
#define nxPropDev_ProductNum                    ((u32)0x00000008 | nxClass_Device | nxPrptype_u32)       //--r
      // Device:Serial Number
#define nxPropDev_SerNum                        ((u32)0x00000005 | nxClass_Device | nxPrptype_u32)       //--r
      // Device:Slot Number
#define nxPropDev_SlotNum                       ((u32)0x00000006 | nxClass_Device | nxPrptype_u32)       //--r
      // Device:Count of All the Ports on the device
#define nxPropDev_NumPortsAll                   ((u32)0x00000007 | nxClass_Device | nxPrptype_u32)       //--r
      // Device:All the interfaces
#define nxPropDev_IntfRefsAll                   ((u32)0x00000008 | nxClass_Device | nxPrptype_1Dref)     //--r
// Interface:Device
#define nxPropIntf_DevRef                       ((u32)0x00000001 | nxClass_Interface | nxPrptype_ref)    //--r
      // Interface:Name
#define nxPropIntf_Name                         ((u32)0x00000002 | nxClass_Interface | nxPrptype_string) //--r
      // Interface:Number
#define nxPropIntf_Num                          ((u32)0x00000003 | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:Port Number
#define nxPropIntf_PortNum                      ((u32)0x00000004 | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:Protocol
#define nxPropIntf_Protocol                     ((u32)0x00000005 | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:CAN:Termination Capability
#define nxPropIntf_CANTermCap                   ((u32)0x00000008 | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:CAN:Transceiver Capability
#define nxPropIntf_CANTcvrCap                   ((u32)0x00000007 | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:Dongle State
#define nxPropIntf_DongleState                  ((u32)0x00000009 | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:DongleID
#define nxPropIntf_DongleID                     ((u32)0x0000000A | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:DongleRevision
#define nxPropIntf_DongleRevision               ((u32)0x0000000C | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:DongleFirmwareVersion
#define nxPropIntf_DongleFirmwareVersion        ((u32)0x0000000D | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:DongleCompatibleRevision
#define nxPropIntf_DongleCompatibleRevision        ((u32)0x0000000E | nxClass_Interface | nxPrptype_u32)    //--r
      // Interface:DongleCompatibleFirmwareVersion
#define nxPropIntf_DongleCompatibleFirmwareVersion ((u32)0x0000000F | nxClass_Interface | nxPrptype_u32)    //--r



    /* PropertyId parameter of nxGetSubProperty, nxGetSubPropertySize, nxSetSubProperty functions. */

      // Session:Frame:CAN:Start Time Offset
#define nxPropSessionSub_CANStartTimeOff        ((u32)0x00000081 | nxClass_Session | nxPrptype_f64)      //--w
      // Session:Frame:CAN:Transmit Time
#define nxPropSessionSub_CANTxTime              ((u32)0x00000082 | nxClass_Session | nxPrptype_f64)      //--w
      // Session:Frame:Skip N Cyclic Frames
#define nxPropSessionSub_SkipNCyclicFrames      ((u32)0x00000083 | nxClass_Session | nxPrptype_u32)      //--w
      // Session:Frame:OutQueueUpdateFreq
#define nxPropSessionSub_OutputQueueUpdateFreq  ((u32)0x00000084 | nxClass_Session | nxPrptype_u32)      //--w
      // Session:Frame:LIN:Transmit N Corrupted Checksums
#define nxPropSessionSub_LINTxNCorruptedChksums ((u32)0x00000085 | nxClass_Session | nxPrptype_u32)      //--w
      // Session:Frame:CAN:J1939_AddrFilter
#define nxPropSessionSub_J1939_AddrFilter       ((u32)0x00000086 | nxClass_Session | nxPrptype_string)   //--w

    /* PropertyId parameter of nxdbGetProperty, nxdbGetPropertySize, nxdbSetProperty functions. */

      // Database:Name
#define nxPropDatabase_Name                     ((u32)0x00000001 | nxClass_Database | nxPrptype_string)  //--r
      // Database:Clusters
#define nxPropDatabase_ClstRefs                 ((u32)0x00000002 | nxClass_Database | nxPrptype_1Dref)   //--r
      // Database:Show Invalid Frames and Signals
#define nxPropDatabase_ShowInvalidFromOpen      ((u32)0x00000003 | nxClass_Database | nxPrptype_bool)    //--rw
      // Cluster:Baud Rate (32-bit) Superseded by nxPropClst_BaudRate64
#define nxPropClst_BaudRate                     ((u32)0x00000001 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:Baud Rate (64-bit)
#define nxPropClst_BaudRate64                   ((u32)0x00000001 | nxClass_Cluster | nxPrptype_u64)      //--rw
      // Cluster:Comment
#define nxPropClst_Comment                      ((u32)0x00000008 | nxClass_Cluster | nxPrptype_string)   //--rw
      // Cluster:Configuration Status
#define nxPropClst_ConfigStatus                 ((u32)0x00000009 | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:Database
#define nxPropClst_DatabaseRef                  ((u32)0x00000002 | nxClass_Cluster | nxPrptype_ref)      //--r
      // Cluster:ECUs
#define nxPropClst_ECURefs                      ((u32)0x00000003 | nxClass_Cluster | nxPrptype_1Dref)    //--r
      // Cluster:Frames
#define nxPropClst_FrmRefs                      ((u32)0x00000004 | nxClass_Cluster | nxPrptype_1Dref)    //--r
      // Cluster:Name (Short)
#define nxPropClst_Name                         ((u32)0x00000005 | nxClass_Cluster | nxPrptype_string)   //--rw
      // Cluster:PDUs
#define nxPropClst_PDURefs                      ((u32)0x00000008 | nxClass_Cluster | nxPrptype_1Dref)    //--r
      // Cluster:PDUs Required?
#define nxPropClst_PDUsReqd                     ((u32)0x0000000A | nxClass_Cluster | nxPrptype_bool)     //--r
      // Cluster:Protocol
#define nxPropClst_Protocol                     ((u32)0x00000006 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:Signals
#define nxPropClst_SigRefs                      ((u32)0x00000007 | nxClass_Cluster | nxPrptype_1Dref)    //--r
      // Cluster:CAN I/O Mode
#define nxPropClst_CanIoMode                    ((u32)0x00000010 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:CAN FD Baudrate (32-bit) Superseded by nxPropClst_CanFdBaudRate64
#define nxPropClst_CanFdBaudRate                ((u32)0x00000011 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:CAN FD Baudrate (64-bit)
#define nxPropClst_CanFdBaudRate64              ((u32)0x00000011 | nxClass_Cluster | nxPrptype_u64)      //--rw
      // Cluster:FlexRay:Action Point Offset
#define nxPropClst_FlexRayActPtOff              ((u32)0x00000020 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:CAS Rx Low Max
#define nxPropClst_FlexRayCASRxLMax             ((u32)0x00000021 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Channels
#define nxPropClst_FlexRayChannels              ((u32)0x00000022 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Cluster Drift Damping
#define nxPropClst_FlexRayClstDriftDmp          ((u32)0x00000023 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Cold Start Attempts
#define nxPropClst_FlexRayColdStAts             ((u32)0x00000024 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Cycle
#define nxPropClst_FlexRayCycle                 ((u32)0x00000025 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Dynamic Segment Start
#define nxPropClst_FlexRayDynSegStart           ((u32)0x00000026 | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:FlexRay:Dynamic Slot Idle Phase
#define nxPropClst_FlexRayDynSlotIdlPh          ((u32)0x00000027 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Latest Usable Dynamic Slot
#define nxPropClst_FlexRayLatestUsableDyn       ((u32)0x0000002A | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:FlexRay:Latest Guaranteed Dynamic Slot
#define nxPropClst_FlexRayLatestGuarDyn         ((u32)0x0000002B | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:FlexRay:Listen Noise
#define nxPropClst_FlexRayLisNoise              ((u32)0x00000028 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Macro Per Cycle
#define nxPropClst_FlexRayMacroPerCycle         ((u32)0x00000029 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Macrotick
#define nxPropClst_FlexRayMacrotick             ((u32)0x00000030 | nxClass_Cluster | nxPrptype_f64)      //--r
      // Cluster:FlexRay:Max Without Clock Correction Fatal
#define nxPropClst_FlexRayMaxWoClkCorFat        ((u32)0x00000031 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Max Without Clock Correction Passive
#define nxPropClst_FlexRayMaxWoClkCorPas        ((u32)0x00000032 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Minislot Action Point Offset
#define nxPropClst_FlexRayMinislotActPt         ((u32)0x00000033 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Minislot
#define nxPropClst_FlexRayMinislot              ((u32)0x00000034 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Network Management Vector Length
#define nxPropClst_FlexRayNMVecLen              ((u32)0x00000035 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:NIT
#define nxPropClst_FlexRayNIT                   ((u32)0x00000036 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:NIT Start
#define nxPropClst_FlexRayNITStart              ((u32)0x00000037 | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:FlexRay:Number Of Minislots
#define nxPropClst_FlexRayNumMinislt            ((u32)0x00000038 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Number Of Static Slots
#define nxPropClst_FlexRayNumStatSlt            ((u32)0x00000039 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Offset Correction Start
#define nxPropClst_FlexRayOffCorSt              ((u32)0x00000040 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Payload Length Dynamic Maximum
#define nxPropClst_FlexRayPayldLenDynMax        ((u32)0x00000041 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Payload Length Maximum
#define nxPropClst_FlexRayPayldLenMax           ((u32)0x00000042 | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:FlexRay:Payload Length Static
#define nxPropClst_FlexRayPayldLenSt            ((u32)0x00000043 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Static Slot
#define nxPropClst_FlexRayStatSlot              ((u32)0x00000045 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Symbol Window
#define nxPropClst_FlexRaySymWin                ((u32)0x00000046 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Symbol Window Start
#define nxPropClst_FlexRaySymWinStart           ((u32)0x00000047 | nxClass_Cluster | nxPrptype_u32)      //--r
      // Cluster:FlexRay:Sync Node Max
#define nxPropClst_FlexRaySyncNodeMax           ((u32)0x00000048 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:TSS Transmitter
#define nxPropClst_FlexRayTSSTx                 ((u32)0x00000049 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Wakeup Symbol Rx Idle
#define nxPropClst_FlexRayWakeSymRxIdl          ((u32)0x00000050 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Wakeup Symbol Rx Low
#define nxPropClst_FlexRayWakeSymRxLow          ((u32)0x00000051 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Wakeup Symbol Rx Window
#define nxPropClst_FlexRayWakeSymRxWin          ((u32)0x00000052 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Wakeup Symbol Tx Idle
#define nxPropClst_FlexRayWakeSymTxIdl          ((u32)0x00000053 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Wakeup Symbol Tx Low
#define nxPropClst_FlexRayWakeSymTxLow          ((u32)0x00000054 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:FlexRay:Use Wakeup
#define nxPropClst_FlexRayUseWakeup             ((u32)0x00000055 | nxClass_Cluster | nxPrptype_bool)     //--rw
      // Cluster:LIN:Schedules
#define nxPropClst_LINSchedules                 ((u32)0x00000070 | nxClass_Cluster | nxPrptype_1Dref)    //--r
      // Cluster:LIN:Tick
#define nxPropClst_LINTick                      ((u32)0x00000071 | nxClass_Cluster | nxPrptype_f64)      //--rw
      // Cluster:FlexRay:Allow Passive to Active
#define nxPropClst_FlexRayAlwPassAct            ((u32)0x00000072 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:Application Protocol
#define nxPropClst_ApplicationProtocol          ((u32)0x00000073 | nxClass_Cluster | nxPrptype_u32)      //--rw
      // Cluster:CAN FD ISO Mode
#define nxPropClst_CanFdIsoMode                 ((u32)0x00000074 | nxClass_Cluster  | nxPrptype_u32)     //--r
      // Frame: Application Protocol
#define nxPropFrm_ApplicationProtocol           ((u32)0x00000064 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:Cluster
#define nxPropFrm_ClusterRef                    ((u32)0x00000001 | nxClass_Frame | nxPrptype_ref)        //--r
      // Frame:Comment
#define nxPropFrm_Comment                       ((u32)0x00000002 | nxClass_Frame | nxPrptype_string)     //--rw
      // Frame:Configuration Status
#define nxPropFrm_ConfigStatus                  ((u32)0x00000009 | nxClass_Frame | nxPrptype_u32)        //--r
      // Frame:Default Payload
#define nxPropFrm_DefaultPayload                ((u32)0x00000005 | nxClass_Frame | nxPrptype_1Du8)       //--rw
      // Frame:Identifier (Slot)
#define nxPropFrm_ID                            ((u32)0x00000003 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:Name (Short)
#define nxPropFrm_Name                          ((u32)0x00000004 | nxClass_Frame | nxPrptype_string)     //--rw
      // Frame:Payload Length (in bytes)
#define nxPropFrm_PayloadLen                    ((u32)0x00000007 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:Signals
#define nxPropFrm_SigRefs                       ((u32)0x00000008 | nxClass_Frame | nxPrptype_1Dref)      //--r
      // Frame:CAN:Extended Identifier?
#define nxPropFrm_CANExtID                      ((u32)0x00000010 | nxClass_Frame | nxPrptype_bool)       //--rw
      // Frame:CAN:Timing Type
#define nxPropFrm_CANTimingType                 ((u32)0x00000011 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:CAN:Transmit Time
#define nxPropFrm_CANTxTime                     ((u32)0x00000012 | nxClass_Frame | nxPrptype_f64)        //--rw
      // Frame:FlexRay:Base Cycle
#define nxPropFrm_FlexRayBaseCycle              ((u32)0x00000020 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:FlexRay:Channel Assignment
#define nxPropFrm_FlexRayChAssign               ((u32)0x00000021 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:FlexRay:Cycle Repetition
#define nxPropFrm_FlexRayCycleRep               ((u32)0x00000022 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:FlexRay:Payload Preamble?
#define nxPropFrm_FlexRayPreamble               ((u32)0x00000023 | nxClass_Frame | nxPrptype_bool)       //--rw
      // Frame:FlexRay:Startup?
#define nxPropFrm_FlexRayStartup                ((u32)0x00000024 | nxClass_Frame | nxPrptype_bool)       //--rw
      // Frame:FlexRay:Sync?
#define nxPropFrm_FlexRaySync                   ((u32)0x00000025 | nxClass_Frame | nxPrptype_bool)       //--rw
      // Frame:FlexRay:Timing Type
#define nxPropFrm_FlexRayTimingType             ((u32)0x00000026 | nxClass_Frame | nxPrptype_u32)        //--rw
      // Frame:FlexRay:In Cycle Repetitions:Enabled?
#define nxPropFrm_FlexRayInCycRepEnabled        ((u32)0x00000030 | nxClass_Frame | nxPrptype_bool)       //--r
      // Frame:FlexRay:In Cycle Repetitions:Identifiers
#define nxPropFrm_FlexRayInCycRepIDs            ((u32)0x00000031 | nxClass_Frame | nxPrptype_1Du32)      //--rw
      // Frame:FlexRay:In Cycle Repetitions:Channel Assignments
#define nxPropFrm_FlexRayInCycRepChAssigns      ((u32)0x00000032 | nxClass_Frame | nxPrptype_1Du32)      //--rw
      // Frame:LIN:Checksum
#define nxPropFrm_LINChecksum                   ((u32)0x00000050 | nxClass_Frame | nxPrptype_u32)        //--r
      // Frame:Mux:Is Data Multiplexed?
#define nxPropFrm_MuxIsMuxed                    ((u32)0x00000040 | nxClass_Frame | nxPrptype_bool)       //--r
      // Frame:Mux:Data Multiplexer Signal
#define nxPropFrm_MuxDataMuxSigRef              ((u32)0x00000041 | nxClass_Frame | nxPrptype_ref)        //--r
      // Frame:Mux:Static Signals
#define nxPropFrm_MuxStaticSigRefs              ((u32)0x00000042 | nxClass_Frame | nxPrptype_1Dref)      //--r
      // Frame:Mux:Subframes
#define nxPropFrm_MuxSubframeRefs               ((u32)0x00000043 | nxClass_Frame | nxPrptype_1Dref)      //--r
      // Frame: PDUs
#define nxPropFrm_PDURefs                       ((u32)0x00000060 | nxClass_Frame | nxPrptype_1Dref)      //--rw
      // Frame: PDU Start Bits
#define nxPropFrm_PDUStartBits                  ((u32)0x00000061 | nxClass_Frame | nxPrptype_1Du32)      //--rw
      // Frame: PDU Update Bits
#define nxPropFrm_PDUUpdateBits                 ((u32)0x00000063 | nxClass_Frame | nxPrptype_1Du32)      //--rw
      // Frame: Variable Payload
#define nxPropFrm_VariablePayload               ((u32)0x00000065 | nxClass_Frame | nxPrptype_bool)       //--rw
      // Frame: CANioMode
#define nxPropFrm_CANioMode                     ((u32)0x00000066 | nxClass_Frame | nxPrptype_u32)        //--rw
      // PDU: Cluster
#define nxPropPDU_ClusterRef                    ((u32)0x00000004 | nxClass_PDU | nxPrptype_ref)          //--r
      // PDU: Default Payload
#define nxPropPDU_DefaultPayload                ((u32)0x00000005 | nxClass_PDU | nxPrptype_1Du8)         //--rw
      // PDU: Comment
#define nxPropPDU_Comment                       ((u32)0x00000002 | nxClass_PDU | nxPrptype_string)       //--rw
      // PDU:Configuration Status
#define nxPropPDU_ConfigStatus                  ((u32)0x00000007 | nxClass_PDU | nxPrptype_u32)          //--r
      // PDU:Frames
#define nxPropPDU_FrmRefs                       ((u32)0x00000006 | nxClass_PDU | nxPrptype_1Dref)        //--r
      // PDU: Name (Short)
#define nxPropPDU_Name                          ((u32)0x00000001 | nxClass_PDU | nxPrptype_string)       //--rw
      // PDU: Payload Length (in bytes)
#define nxPropPDU_PayloadLen                    ((u32)0x00000003 | nxClass_PDU | nxPrptype_u32)          //--rw
      // PDU:Signals
#define nxPropPDU_SigRefs                       ((u32)0x00000005 | nxClass_PDU | nxPrptype_1Dref)        //--r
      // PDU:Mux:Is Data Multiplexed?
#define nxPropPDU_MuxIsMuxed                    ((u32)0x00000008 | nxClass_PDU | nxPrptype_bool)         //--r
      // PDU:Mux:Data Multiplexer Signal
#define nxPropPDU_MuxDataMuxSigRef              ((u32)0x00000009 | nxClass_PDU | nxPrptype_ref)          //--r
      // PDU:Mux:Static Signals
#define nxPropPDU_MuxStaticSigRefs              ((u32)0x0000000A | nxClass_PDU | nxPrptype_1Dref)        //--r
      // PDU:Mux:Subframes
#define nxPropPDU_MuxSubframeRefs               ((u32)0x0000000B | nxClass_PDU | nxPrptype_1Dref)        //--r
      // Signal:Byte Order
#define nxPropSig_ByteOrdr                      ((u32)0x00000001 | nxClass_Signal | nxPrptype_u32)       //--rw
      // Signal:Comment
#define nxPropSig_Comment                       ((u32)0x00000002 | nxClass_Signal | nxPrptype_string)    //--rw
      // Signal:Configuration Status
#define nxPropSig_ConfigStatus                  ((u32)0x00000009 | nxClass_Signal | nxPrptype_u32)       //--r
      // Signal:Data Type
#define nxPropSig_DataType                      ((u32)0x00000003 | nxClass_Signal | nxPrptype_u32)       //--rw
      // Signal:Default Value
#define nxPropSig_Default                       ((u32)0x00000004 | nxClass_Signal | nxPrptype_f64)       //--rw
      // Signal:Frame
#define nxPropSig_FrameRef                      ((u32)0x00000005 | nxClass_Signal | nxPrptype_ref)       //--r
      // Signal:Maximum Value
#define nxPropSig_Max                           ((u32)0x00000006 | nxClass_Signal | nxPrptype_f64)       //--rw
      // Signal:Minimum Value
#define nxPropSig_Min                           ((u32)0x00000007 | nxClass_Signal | nxPrptype_f64)       //--rw
      // Signal:Name (Short)
#define nxPropSig_Name                          ((u32)0x00000008 | nxClass_Signal | nxPrptype_string)    //--rw
      // Signal:Name (Unique to Cluster)
#define nxPropSig_NameUniqueToCluster           ((u32)0x00000010 | nxClass_Signal | nxPrptype_string)    //--r
      // Signal:Number of Bits
#define nxPropSig_NumBits                       ((u32)0x00000012 | nxClass_Signal | nxPrptype_u32)       //--rw
      // Signal:PDU
#define nxPropSig_PDURef                        ((u32)0x00000011 | nxClass_Signal | nxPrptype_ref)       //--r
      // Signal:Scaling Factor
#define nxPropSig_ScaleFac                      ((u32)0x00000013 | nxClass_Signal | nxPrptype_f64)       //--rw
      // Signal:Scaling Offset
#define nxPropSig_ScaleOff                      ((u32)0x00000014 | nxClass_Signal | nxPrptype_f64)       //--rw
      // Signal:Start Bit
#define nxPropSig_StartBit                      ((u32)0x00000015 | nxClass_Signal | nxPrptype_u32)       //--rw
      // Signal:Unit
#define nxPropSig_Unit                          ((u32)0x00000016 | nxClass_Signal | nxPrptype_string)    //--rw
      // Signal:Mux:Data Multiplexer?
#define nxPropSig_MuxIsDataMux                  ((u32)0x00000030 | nxClass_Signal | nxPrptype_bool)      //--rw
      // Signal:Mux:Dynamic?
#define nxPropSig_MuxIsDynamic                  ((u32)0x00000031 | nxClass_Signal | nxPrptype_bool)      //--r
      // Signal:Mux:Multiplexer Value
#define nxPropSig_MuxValue                      ((u32)0x00000032 | nxClass_Signal | nxPrptype_u32)       //--r
      // Signal:Mux:Subframe
#define nxPropSig_MuxSubfrmRef                  ((u32)0x00000033 | nxClass_Signal | nxPrptype_ref)       //--r
      // Subframe:Configuration Status
#define nxPropSubfrm_ConfigStatus               ((u32)0x00000009 | nxClass_Subframe | nxPrptype_u32)     //--r
      // Subframe:Dynamic Signals
#define nxPropSubfrm_DynSigRefs                 ((u32)0x00000001 | nxClass_Subframe | nxPrptype_1Dref)   //--r
      // Subframe:"Frame"
#define nxPropSubfrm_FrmRef                     ((u32)0x00000002 | nxClass_Subframe | nxPrptype_ref)     //--r
      // Subframe:Multiplexer Value
#define nxPropSubfrm_MuxValue                   ((u32)0x00000003 | nxClass_Subframe | nxPrptype_u32)     //--rw
      // Subframe:Name (Short)
#define nxPropSubfrm_Name                       ((u32)0x00000004 | nxClass_Subframe | nxPrptype_string)  //--rw
      // Subframe:PDU
#define nxPropSubfrm_PDURef                     ((u32)0x00000005 | nxClass_Subframe | nxPrptype_ref)     //--r
      // Subframe:Name (Unique to Cluster)
#define nxPropSubfrm_NameUniqueToCluster        ((u32)0x00000007 | nxClass_Subframe | nxPrptype_string)  //--r
      // ECU:Cluster
#define nxPropECU_ClstRef                       ((u32)0x00000001 | nxClass_ECU | nxPrptype_ref)          //--r
      // ECU:Comment
#define nxPropECU_Comment                       ((u32)0x00000005 | nxClass_ECU | nxPrptype_string)       //--rw
      // ECU:Configuration Status
#define nxPropECU_ConfigStatus                  ((u32)0x00000009 | nxClass_ECU | nxPrptype_u32)          //--r
      // ECU:Name (Short)
#define nxPropECU_Name                          ((u32)0x00000002 | nxClass_ECU | nxPrptype_string)       //--rw
      // ECU:Frames Received
#define nxPropECU_RxFrmRefs                     ((u32)0x00000003 | nxClass_ECU | nxPrptype_1Dref)        //--rw
      // ECU:Frames Transmitted
#define nxPropECU_TxFrmRefs                     ((u32)0x00000004 | nxClass_ECU | nxPrptype_1Dref)        //--rw
      // ECU:FlexRay:Coldstart?
#define nxPropECU_FlexRayIsColdstart            ((u32)0x00000010 | nxClass_ECU | nxPrptype_bool)         //--r
      // ECU:FlexRay:Startup Frame
#define nxPropECU_FlexRayStartupFrameRef        ((u32)0x00000011 | nxClass_ECU | nxPrptype_ref)          //--r
      // ECU:FlexRay:Wakeup Pattern
#define nxPropECU_FlexRayWakeupPtrn             ((u32)0x00000012 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:FlexRay:Wakeup Channels
#define nxPropECU_FlexRayWakeupChs              ((u32)0x00000013 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:FlexRay:Connected Channels
#define nxPropECU_FlexRayConnectedChs           ((u32)0x00000014 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:LIN:Master?
#define nxPropECU_LINMaster                     ((u32)0x00000020 | nxClass_ECU | nxPrptype_bool)         //--rw
      // ECU:LIN:Protocol Version
#define nxPropECU_LINProtocolVer                ((u32)0x00000021 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:LIN:Initial NAD
#define nxPropECU_LINInitialNAD                 ((u32)0x00000022 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:LIN:Configured NAD
#define nxPropECU_LINConfigNAD                  ((u32)0x00000023 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:LIN:Supplier ID
#define nxPropECU_LINSupplierID                 ((u32)0x00000024 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:LIN:Function ID
#define nxPropECU_LINFunctionID                 ((u32)0x00000025 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:LIN:P2 Min
#define nxPropECU_LINP2min                      ((u32)0x00000026 | nxClass_ECU | nxPrptype_f64)          //--rw
      // ECU:LIN:ST Min
#define nxPropECU_LINSTmin                      ((u32)0x00000027 | nxClass_ECU | nxPrptype_f64)          //--rw
      // ECU:SAE J1939:PreferredAddress
#define nxPropECU_J1939PreferredAddress         ((u32)0x00000028 | nxClass_ECU | nxPrptype_u32)          //--rw
      // ECU:SAE J1939:NodeName
#define nxPropECU_J1939NodeName                 ((u32)0x00000029 | nxClass_ECU | nxPrptype_u64)          //--rw
      // LIN Schedule:Cluster
#define nxPropLINSched_ClstRef                  ((u32)0x00000005 | nxClass_LINSched | nxPrptype_ref)     //--r
      // LIN Schedule:Comment
#define nxPropLINSched_Comment                  ((u32)0x00000006 | nxClass_LINSched | nxPrptype_string)  //--rw
      // LIN Schedule:Configuration Status
#define nxPropLINSched_ConfigStatus             ((u32)0x00000007 | nxClass_LINSched | nxPrptype_u32)     //--r
      // LIN Schedule:Entries
#define nxPropLINSched_Entries                  ((u32)0x00000001 | nxClass_LINSched | nxPrptype_1Dref)   //--r
      // LIN Schedule:Name
#define nxPropLINSched_Name                     ((u32)0x00000002 | nxClass_LINSched | nxPrptype_string)  //--rw
      // LIN Schedule:Priority
#define nxPropLINSched_Priority                 ((u32)0x00000003 | nxClass_LINSched | nxPrptype_u32)     //--rw
      // LIN Schedule:Run Mode
#define nxPropLINSched_RunMode                  ((u32)0x00000004 | nxClass_LINSched | nxPrptype_u32)     //--rw
      // LIN Schedule Entry:Collision Resolving Schedule
#define nxPropLINSchedEntry_CollisionResSched   ((u32)0x00000001 | nxClass_LINSchedEntry | nxPrptype_ref) //--rw
      // LIN Schedule Entry:Delay
#define nxPropLINSchedEntry_Delay               ((u32)0x00000002 | nxClass_LINSchedEntry | nxPrptype_f64) //--rw
      // LIN Schedule Entry:Event Identifier
#define nxPropLINSchedEntry_EventID             ((u32)0x00000003 | nxClass_LINSchedEntry | nxPrptype_u32) //--rw
      // LIN Schedule Entry:Frames
#define nxPropLINSchedEntry_Frames              ((u32)0x00000004 | nxClass_LINSchedEntry | nxPrptype_1Dref) //--rw
      // LIN Schedule Entry:Name
#define nxPropLINSchedEntry_Name                ((u32)0x00000006 | nxClass_LINSchedEntry | nxPrptype_string)  //--rw
      // LIN Schedule Entry:Name (Unique to Cluster)
#define nxPropLINSchedEntry_NameUniqueToCluster ((u32)0x00000008 | nxClass_LINSchedEntry | nxPrptype_string)  //--r
      // LIN Schedule Entry:Schedule
#define nxPropLINSchedEntry_Sched               ((u32)0x00000007 | nxClass_LINSchedEntry | nxPrptype_ref)     //--r
      // LIN Schedule Entry:Type
#define nxPropLINSchedEntry_Type                ((u32)0x00000005 | nxClass_LINSchedEntry | nxPrptype_u32)     //--rw
      // LIN Schedule Entry:Node Configuration:Free Format:Data Bytes
#define nxPropLINSchedEntry_NC_FF_DataBytes     ((u32)0x00000009 | nxClass_LINSchedEntry | nxPrptype_1Du8)    //--rw

/***********************************************************************
   C O N S T A N T S   F O R   F U N C T I O N   P A R A M E T E R S
***********************************************************************/

   // Parameter Mode of function nxCreateSession
#define nxMode_SignalInSinglePoint           0  // SignalInSinglePoint
#define nxMode_SignalInWaveform              1  // SignalInWaveform
#define nxMode_SignalInXY                    2  // SignalInXY
#define nxMode_SignalOutSinglePoint          3  // SignalOutSinglePoint
#define nxMode_SignalOutWaveform             4  // SignalOutWaveform
#define nxMode_SignalOutXY                   5  // SignalOutXY
#define nxMode_FrameInStream                 6  // FrameInStream
#define nxMode_FrameInQueued                 7  // FrameInQueued
#define nxMode_FrameInSinglePoint            8  // FrameInSinglePoint
#define nxMode_FrameOutStream                9  // FrameOutStream
#define nxMode_FrameOutQueued                10 // FrameOutQueued
#define nxMode_FrameOutSinglePoint           11 // FrameOutSinglePoint
#define nxMode_SignalConversionSinglePoint   12 // SignalConversionSinglePoint

   // Parameter Scope of functions nxStart, nxStop
#define nxStartStop_Normal                   0  // StartStop_Normal
#define nxStartStop_SessionOnly              1  // StartStop_SessionOnly
#define nxStartStop_InterfaceOnly            2  // StartStop_InterfaceOnly
#define nxStartStop_SessionOnlyBlocking      3  // StartStop_SessionOnlyBlocking

   // Parameter Modifier of nxBlink
#define nxBlink_Disable                      0  // Blink_Disable
#define nxBlink_Enable                       1  // Blink_Enable

   // Terminal names for nxConnectTerminals and nxDisconnectTerminals (source or destination)
#define nxTerm_PXI_Trig0                     "PXI_Trig0"             // PXI_Trig0 same as RTSI0
#define nxTerm_PXI_Trig1                     "PXI_Trig1"
#define nxTerm_PXI_Trig2                     "PXI_Trig2"
#define nxTerm_PXI_Trig3                     "PXI_Trig3"
#define nxTerm_PXI_Trig4                     "PXI_Trig4"
#define nxTerm_PXI_Trig5                     "PXI_Trig5"
#define nxTerm_PXI_Trig6                     "PXI_Trig6"
#define nxTerm_PXI_Trig7                     "PXI_Trig7"
#define nxTerm_FrontPanel0                   "FrontPanel0"
#define nxTerm_FrontPanel1                   "FrontPanel1"
#define nxTerm_PXI_Star                      "PXI_Star"
#define nxTerm_PXI_Clk10                     "PXI_Clk10"
#define nxTerm_10MHzTimebase                 "10MHzTimebase"
#define nxTerm_1MHzTimebase                  "1MHzTimebase"
#define nxTerm_MasterTimebase                "MasterTimebase"
#define nxTerm_CommTrigger                   "CommTrigger"
#define nxTerm_StartTrigger                  "StartTrigger"
#define nxTerm_FlexRayStartCycle             "FlexRayStartCycle"
#define nxTerm_FlexRayMacrotick              "FlexRayMacrotick"
#define nxTerm_LogTrigger                    "LogTrigger"

   /* StateID for nxReadState
   These constants use an encoding similar to property ID (nxProp_ prefix).
   */
      // Current time of the interface (using nxTimestamp_t)
#define nxState_TimeCurrent                  ((u32)0x00000001 | nxClass_Interface | nxPrptype_time)   // TimeCurrent
      // Time when communication began on the interface (protocol operational / integrated)
#define nxState_TimeCommunicating            ((u32)0x00000002 | nxClass_Interface | nxPrptype_time)   // TimeCommunicating
      // Start time of the interface, when the attempt to communicate began (startup protocol)
#define nxState_TimeStart                    ((u32)0x00000003 | nxClass_Interface | nxPrptype_time)   // TimeStart
      // Session information: Use macros with prefix nxSessionInfo_Get_ to get fields of the u32
#define nxState_SessionInfo                  ((u32)0x00000004 | nxClass_Interface | nxPrptype_u32)    // SessionInfo
      // CAN communication: Use macros with prefix nxCANComm_Get_ to get fields of the u32
#define nxState_CANComm                      ((u32)0x00000010 | nxClass_Interface | nxPrptype_u32)    // CANComm
      // FlexRay communication: Use macros with prefix nxFlexRayComm_Get_ to get fields of the u32
#define nxState_FlexRayComm                  ((u32)0x00000020 | nxClass_Interface | nxPrptype_u32)    // FlexRayComm
      // FlexRay statistics: Use typedef nxFlexRayStats_t to read these statistics using a struct of multiple u32
#define nxState_FlexRayStats                 ((u32)0x00000021 | nxClass_Interface | nxPrptype_1Du32)  // FlexRayStats
      // LIN communication: Use macros with prefix nxLINComm_Get_ to get fields of 2 u32's
#define nxState_LINComm                      ((u32)0x00000030 | nxClass_Interface | nxPrptype_u32)    // LINComm
      // J1939 communication:
#define nxState_J1939Comm                    ((u32)0x00000040 | nxClass_Interface | nxPrptype_u32)    // J1939Comm

   /* StateID for nxWriteState
   These constants use an encoding similar to property ID (nxProp_ prefix).
   */
#define nxState_LINScheduleChange            ((u32)0x00000081 | nxClass_Interface | nxPrptype_u32)    // LINScheduleChange
#define nxState_LINDiagnosticScheduleChange  ((u32)0x00000083 | nxClass_Interface | nxPrptype_u32)    // LINDiagnosticScheduleChange
#define nxState_FlexRaySymbol                ((u32)0x00000082 | nxClass_Interface | nxPrptype_u32)    // FlexRaySymbol

#define  nxCanFdMode_ISO         0  // ISO mode, default
#define  nxCanFdMode_NonISO      1  // Non-ISO mode (first CAN FD definition)
#define  nxCanFdMode_ISO_Legacy  2  // ISO mode, but behaviour like non-ISO

   // Macros to get fields of u32 returned by nxReadState of nxState_SessionInfo
      // Get state of frames in the session; uses constants with prefix nxSessionInfoState_
#define nxSessionInfo_Get_State(StateValue)  ((u8)( (u32)StateValue         & 0x00000003))

   // State of frames in the session, from nxState_SessionInfo (nxSessionInfo_Get_State)
#define nxSessionInfoState_Stopped           0     // all frames stopped
#define nxSessionInfoState_Started           1     // all frames started
#define nxSessionInfoState_Mix               2     // one or more frames started, and one or more frames stopped

   // Macros to get fields of u32 returned by nxReadState of nxState_CANComm
      // Get CAN communication state; uses constants with prefix nxCANCommState_
#define nxCANComm_Get_CommState(StateValue)  ((u8)( (u32)StateValue         & 0x0000000F))
      // Get CAN transceiver error (!NERR); 1 = error, 0 = no error
#define nxCANComm_Get_TcvrErr(StateValue)    ((u8)( ((u32)StateValue >> 4)  & 0x00000001))
      // Get indication of CAN controller/transceiver sleep; 1 = asleep, 0 = awake
#define nxCANComm_Get_Sleep(StateValue)      ((u8)( ((u32)StateValue >> 5)  & 0x00000001))
      // Get last bus error that incremented counters; uses constants with prefix nxCANLastErr_
#define nxCANComm_Get_LastErr(StateValue)    ((u8)( ((u32)StateValue >> 8)  & 0x0000000F))
      // Get Transmit Error Counter as defined by the CAN protocol specification
#define nxCANComm_Get_TxErrCount(StateValue) ((u8)( ((u32)StateValue >> 16) & 0x000000FF))
      // Get Receive Error Counter as defined by the CAN protocol specification
#define nxCANComm_Get_RxErrCount(StateValue) ((u8)( ((u32)StateValue >> 24) & 0x000000FF))

   // Communication state from nxState_CANComm (nxCANComm_Get_CommState)
#define nxCANCommState_ErrorActive           0
#define nxCANCommState_ErrorPassive          1
#define nxCANCommState_BusOff                2
#define nxCANCommState_Init                  3

   // Last bus error from nxState_CANComm (nxCANComm_Get_LastErr)
#define nxCANLastErr_None                    0
#define nxCANLastErr_Stuff                   1
#define nxCANLastErr_Form                    2
#define nxCANLastErr_Ack                     3
#define nxCANLastErr_Bit1                    4
#define nxCANLastErr_Bit0                    5
#define nxCANLastErr_CRC                     6

   // constants for nxPropClst_CanIoMode, nxPropSession_IntfCanIoMode, nxPropSession_IntfCanTxIoMode, nxPropAlias_CanIoMode
#define nxCANioMode_CAN                      0
#define nxCANioMode_CAN_FD                   1
#define nxCANioMode_CAN_FD_BRS               2

   // Macros to get fields of u32 returned by nxReadState of nxState_FlexRayComm
      /* Get FlexRay Protocol Operation Control (POC) state,
      which uses constants with prefix nxFlexRayPOCState_ */
#define nxFlexRayComm_Get_POCState(StateValue)        ((u8)((u32)StateValue & 0x0000000F))
      /* From FlexRay spec 9.3.1.3.4: "the number of consecutive even/odd cycle pairs
      (vClockCorrectionFailed) that have passed without clock synchronization having performed an offset or a rate
      correction due to lack of synchronization frames (as maintained by the POC process)."
      This value is used for comparison to the cluster thresholds MaxWithoutClockCorrectFatal and
      MaxWithoutClockCorrectionPassive (XNET properties nxPropClst_FlexRayMaxWoClkCorFat
      and nxPropClst_FlexRayMaxWoClkCorPas). */
#define nxFlexRayComm_Get_ClockCorrFailed(StateValue) ((u8)( ((u32)StateValue >> 4) & 0x0000000F))
      /* From FlexRay spec 9.3.1.3.1: "the number of consecutive even/odd cycle pairs (vAllowPassiveToActive)
      that have passed with valid rate and offset correction terms, but the node still in POC:normal passive
      state due to a host configured delay to POC:normal active state (as maintained by the POC process).
      This value is used for comparison to the interface threshold AllowPassiveToActive
      (XNET property nxPropSession_IntfFlexRayAlwPassAct). */
#define nxFlexRayComm_Get_PassiveToActiveCount(StateValue)  \
                                                      ((u8)( ((u32)StateValue >> 8) & 0x0000001F))
#define nxFlexRayComm_Get_ChannelASleep(StateValue)  \
                                                      ((u8)( ((u32)StateValue >> 13) & 0x00000001))
#define nxFlexRayComm_Get_ChannelBSleep(StateValue)  \
                                                      ((u8)( ((u32)StateValue >> 14) & 0x00000001))

   // POC state (Protocol Operation Control state) from nxFlexRayPOC_Get_State
#define nxFlexRayPOCState_DefaultConfig      0
#define nxFlexRayPOCState_Ready              1
#define nxFlexRayPOCState_NormalActive       2
#define nxFlexRayPOCState_NormalPassive      3
#define nxFlexRayPOCState_Halt               4
#define nxFlexRayPOCState_Monitor            5
#define nxFlexRayPOCState_Config             15

   // Macros to get fields of 1st u32 returned by nxReadState of nxState_LINComm
      // Get indication of LIN interface sleep state; 1 = asleep, 0 = awake
#define nxLINComm_Get_Sleep(StateValue)      ((u8)( ((u32)StateValue >> 1) & 0x00000001))
      // Get LIN communication state; uses constants with prefix nxLINCommState_
#define nxLINComm_Get_CommState(StateValue)  ((u8)( ((u32)StateValue >> 2) & 0x00000003))
      // Get last bus error; this code uses constants with prefix nxLINLastErrCode_
#define nxLINComm_Get_LastErrCode(StateValue) \
                                             ((u8)( ((u32)StateValue >> 4) & 0x0000000F))
      // Get received data for last bus error; this value applies only to specific codes
#define nxLINComm_Get_LastErrReceived(StateValue) \
                                             ((u8)( ((u32)StateValue >> 8) & 0x000000FF))
      // Get expected data for last bus error; this value applies only to specific codes
#define nxLINComm_Get_LastErrExpected(StateValue) \
                                             ((u8)( ((u32)StateValue >> 16) & 0x000000FF))
      // Get ID of last bus error; this value applies only to specific codes
#define nxLINComm_Get_LastErrID(StateValue) \
                                             ((u8)( ((u32)StateValue >> 24) & 0x0000003F))
      // Get indication of LIN transceiver ready (powered); 1 = powered, 0 = not powered
#define nxLINComm_Get_TcvrRdy(StateValue)    ((u8)( ((u32)StateValue >> 31) & 0x00000001))

   // Macros to get fields of 2nd u32 returned by nxReadState of nxState_LINComm
      // Get index of the currently running schedule (0xFF if Null-schedule).
#define nxLINComm_Get2_ScheduleIndex(State2Value) \
                                             ((u8)( ((u32)State2Value) & 0x000000FF))

   // Communication state from nxState_LINComm (nxLINComm_Get_CommState macro)
#define nxLINCommState_Idle                  0
#define nxLINCommState_Active                1
#define nxLINCommState_Inactive              2

   // Diagnostic schedule state for nxState_LINDiagnosticScheduleChange
#define nxLINDiagnosticSchedule_NULL         0x0000
#define nxLINDiagnosticSchedule_MasterReq    0x0001
#define nxLINDiagnosticSchedule_SlaveResp    0x0002

   // Last error code from nxState_LINComm (nxLINComm_Get_LastErrCode macro)
#define nxLINLastErrCode_None                0
#define nxLINLastErrCode_UnknownId           1
#define nxLINLastErrCode_Form                2
#define nxLINLastErrCode_Framing             3
#define nxLINLastErrCode_Readback            4
#define nxLINLastErrCode_Timeout             5
#define nxLINLastErrCode_CRC                 6

   // Condition of nxWait
#define nxCondition_TransmitComplete         0x8001  // TransmitComplete
#define nxCondition_IntfCommunicating        0x8002  // IntfCommunicating
#define nxCondition_IntfRemoteWakeup         0x8003  // IntfRemoteWakeup

      // Constants for use with Timeout parameter of read and write functions
#define nxTimeout_None                       (0)
#define nxTimeout_Infinite                   (-1)

   // Parameter Mode of function nxdbGetDBCAttribute and nxdbGetDBCAttributeSize
#define nxGetDBCMode_Attribute               0  // Attribute
#define nxGetDBCMode_EnumerationList         1  // Enumeration List
#define nxGetDBCMode_AttributeList           2  // List of available attributes of given type
#define nxGetDBCMode_ValueTableList          3  // Value table for a signal

   // Copy mode of function nxdbMerge
#define nxdbMerge_CopyUseSource     0  // CopyUseSource
#define nxdbMerge_CopyUseTarget     1  // CopyUseTarget
#define nxdbMerge_MergeUseSource    2  // MergeUseSource
#define nxdbMerge_MergeUseTarget    3  // MergeUseTarget

   // State of the dongle (nxPropIntf_DongleState)
#define nxDongleState_NoDongle_NoExtPower    1
#define nxDongleState_NoDongle_ExtPower      2
#define nxDongleState_Dongle_NoExtPower      3
#define nxDongleState_Ready                  4
#define nxDongleState_Busy                   5
#define nxDongleState_CommError              13
#define nxDongleState_OverCurrent            14

   // Dongle ID (nxPropIntf_DongleID)
#define nxDongleID_LS_CAN           1
#define nxDongleID_HS_CAN           2
#define nxDongleID_SW_CAN           3
#define nxDongleID_XS_CAN           4
#define nxDongleID_LIN              6
#define nxDongleID_DongleLess       13
#define nxDongleID_Unknown          14

/***********************************************************************
   C O N S T A N T S   F O R   H A R D W A R E   P R O P E R T I E S
***********************************************************************/

// System/Device/Interface properties (hardware info)

   // Property ID nxPropSys_VerPhase
#define nxPhase_Development                  0
#define nxPhase_Alpha                        1
#define nxPhase_Beta                         2
#define nxPhase_Release                      3

   // Property ID nxPropDev_FormFac
#define nxDevForm_PXI                        0
#define nxDevForm_PCI                        1
#define nxDevForm_cSeries                    2
#define nxDevForm_PXIe                       3
#define nxDevForm_USB                        4
#define nxDevForm_PCIe                       5

   // Property ID nxPropIntf_CANTermCap
#define nxCANTermCap_No                      0
#define nxCANTermCap_Yes                     1

   // Property ID nxPropIntf_CANTcvrCap
#define nxCANTcvrCap_HS                      0
#define nxCANTcvrCap_LS                      1
#define nxCANTcvrCap_XS                      3
#define nxCANTcvrCap_XS_HS_LS                4
#define nxCANTcvrCap_Unknown                 0xFFFFFFFF

   // Property ID nxPropIntf_Protocol and nxPropClst_Protocol
#define nxProtocol_Unknown                   0xFFFFFFFE   //The value will be reported on a port with unplugged transceiver cable
#define nxProtocol_CAN                       0
#define nxProtocol_FlexRay                   1
#define nxProtocol_LIN                       2

   // Property ID nxPropClst_ApplicationProtocol and nxPropSession_ApplicationProtocol
#define nxAppProtocol_None                   0
#define nxAppProtocol_J1939                  1

/***********************************************************************
   C O N S T A N T S   F O R   S E S S I O N   P R O P E R T I E S
***********************************************************************/

// Session properties (including runtime interface properties)

   // Macro to set nxPropSession_IntfBaudRate for an advanced CAN baud rate (bit timings)
   // If you pass a basic baud rate like 125000 or 500000, NI-XNET calculates bit timings for you
#define nxAdvCANBaudRate_Set(TimeQuantum, TimeSeg0, TimeSeg1, SyncJumpWidth) ( \
            (((u32)TimeQuantum) & 0x0000FFFF) | \
            (((u32)TimeSeg0 << 16) & 0x000F0000) | \
            (((u32)TimeSeg1 << 20) & 0x00700000) | \
            (((u32)SyncJumpWidth << 24) & 0x03000000) | \
            ((u32)0x80000000) )

   // Macros to get fields of nxPropSession_IntfBaudRate for an advanced CAN baud rate
#define nxAdvCANBaudRate_Get_TimeQuantum(AdvBdRt)     ((u16)( ((u32)AdvBdRt) & 0x0000FFFF))
#define nxAdvCANBaudRate_Get_TimeSeg0(AdvBdRt)        ((u8)( ((u32)AdvBdRt >> 16) & 0x0000000F))
#define nxAdvCANBaudRate_Get_TimeSeg1(AdvBdRt)        ((u8)( ((u32)AdvBdRt >> 20) & 0x00000007))
#define nxAdvCANBaudRate_Get_SyncJumpWidth(AdvBdRt)   ((u8)( ((u32)AdvBdRt >> 24) & 0x00000003))
#define nxAdvCANBaudRate_Get_NumSamples(AdvBdRt)      ((u8)( ((u32)AdvBdRt >> 26) & 0x00000001))

   // Property ID nxPropSession_IntfCANTerm
#define nxCANTerm_Off                        0
#define nxCANTerm_On                         1

    // Property ID nxPropSession_IntfCANTcvrState
#define nxCANTcvrState_Normal                0
#define nxCANTcvrState_Sleep                 1
#define nxCANTcvrState_SWWakeup              2
#define nxCANTcvrState_SWHighSpeed           3

   // Property ID nxPropSession_IntfCANTcvrType
#define nxCANTcvrType_HS                     0
#define nxCANTcvrType_LS                     1
#define nxCANTcvrType_SW                     2
#define nxCANTcvrType_Ext                    3
#define nxCANTcvrType_Disc                   4

   // Property ID nxPropSession_IntfFlexRaySampPerMicro
#define nxFlexRaySampPerMicro_1              0
#define nxFlexRaySampPerMicro_2              1
#define nxFlexRaySampPerMicro_4              2

   // Property ID nxPropSession_IntfFlexRayTerm
#define nxFlexRayTerm_Off                    0
#define nxFlexRayTerm_On                     1

   // Property ID nxPropSession_IntfLINSleep
#define nxLINSleep_RemoteSleep               0
#define nxLINSleep_RemoteWake                1
#define nxLINSleep_LocalSleep                2
#define nxLINSleep_LocalWake                 3

   // Property ID nxPropSession_IntfLINTerm
#define nxLINTerm_Off                        0
#define nxLINTerm_On                         1

   // Property ID nxPropSession_IntfOutStrmTimng
#define nxOutStrmTimng_Immediate             0
#define nxOutStrmTimng_ReplayExclusive       1
#define nxOutStrmTimng_ReplayInclusive       2

   // Property ID nxPropSession_IntfCANPendTxOrder
#define nxCANPendTxOrder_AsSubmitted         0
#define nxCANPendTxOrder_ByIdentifier        1

   // Property ID nxPropSession_IntfFlexRaySleep
#define nxFlexRaySleep_LocalSleep            0
#define nxFlexRaySleep_LocalWake             1
#define nxFlexRaySleep_RemoteWake            2

   // Property ID nxPropSession_IntfCANExtTcvrConfig
   // These bits can be combined to define the capabilities of the connected
   // external transceiver.
#define nxCANExtTcvrConfig_NormalSupported              (1 << 2)
#define nxCANExtTcvrConfig_SleepSupported               (1 << 5)
#define nxCANExtTcvrConfig_SWWakeupSupported            (1 << 8)
#define nxCANExtTcvrConfig_SWHighSpeedSupported         (1 << 11)
#define nxCANExtTcvrConfig_PowerOnSupported             (1 << 14)
#define nxCANExtTcvrConfig_NormalOutput0Set             (1 << 0)
#define nxCANExtTcvrConfig_SleepOutput0Set              (1 << 3)
#define nxCANExtTcvrConfig_SWWakeupOutput0Set           (1 << 6)
#define nxCANExtTcvrConfig_SWHighSpeedOutput0Set        (1 << 9)
#define nxCANExtTcvrConfig_PowerOnOutput0Set            (1 << 12)
#define nxCANExtTcvrConfig_NormalOutput1Set             (1 << 1)
#define nxCANExtTcvrConfig_SleepOutput1Set              (1 << 4)
#define nxCANExtTcvrConfig_SWWakeupOutput1Set           (1 << 7)
#define nxCANExtTcvrConfig_SWHighSpeedOutput1Set        (1 << 10)
#define nxCANExtTcvrConfig_PowerOnOutput1Set            (1 << 13)
#define nxCANExtTcvrConfig_nErrConnected                (1 << 31)


/***********************************************************************
   C O N S T A N T S   F O R   D A T A B A S E   P R O P E R T I E S
***********************************************************************/

// Database properties (Database/Cluster/ECU/Frame/Subframe/Signal)

   // Property ID nxPropClst_FlexRayChannels, nxPropFrm_FlexRayChAssign
   // nxPropClst_FlexRayConnectedChannels and nxPropClst_FlexRayWakeChannels
#define nxFrmFlexRayChAssign_A                  1
#define nxFrmFlexRayChAssign_B                  2
#define nxFrmFlexRayChAssign_AandB              3
#define nxFrmFlexRayChAssign_None               4

   // Property ID nxPropClst_FlexRaySampClkPer
#define nxClstFlexRaySampClkPer_p0125us         0
#define nxClstFlexRaySampClkPer_p025us          1
#define nxClstFlexRaySampClkPer_p05us           2

   // Property ID nxPropClst_Protocol uses prefix nxProtocol_

   // Property ID nxPropFrm_FlexRayTimingType
#define nxFrmFlexRayTiming_Cyclic               0
#define nxFrmFlexRayTiming_Event                1

   // Property ID nxPropFrm_CANTimingType
#define nxFrmCANTiming_CyclicData               0
#define nxFrmCANTiming_EventData                1
#define nxFrmCANTiming_CyclicRemote             2
#define nxFrmCANTiming_EventRemote              3
#define nxFrmCANTiming_CyclicEvent              4

   // Property ID nxPropSig_ByteOrdr
#define nxSigByteOrdr_LittleEndian              0  // Intel
#define nxSigByteOrdr_BigEndian                 1  // Motorola

   // Property ID nxPropSig_DataType
#define nxSigDataType_Signed                    0
#define nxSigDataType_Unsigned                  1
#define nxSigDataType_IEEEFloat                 2
#define nxSigDataType_ByteArray                 3

   // Property ID nxPropECU_LINProtocolVer
#define nxLINProtocolVer_1_2                    2
#define nxLINProtocolVer_1_3                    3
#define nxLINProtocolVer_2_0                    4
#define nxLINProtocolVer_2_1                    5
#define nxLINProtocolVer_2_2                    6

   // Property ID nxPropLINSched_RunMode
#define nxLINSchedRunMode_Continuous            0
#define nxLINSchedRunMode_Once                  1
#define nxLINSchedRunMode_Null                  2

   // Property ID nxPropLINSchedEntry_Type
#define nxLINSchedEntryType_Unconditional       0
#define nxLINSchedEntryType_Sporadic            1
#define nxLINSchedEntryType_EventTriggered      2
#define nxLINSchedEntryType_NodeConfigService   3

   // Property ID nxPropFrm_LINChecksum
#define nxFrmLINChecksum_Classic                0
#define nxFrmLINChecksum_Enhanced               1

/***********************************************************************
                            D A T A   T Y P E S
***********************************************************************/

   /* The ANSI C99 standard defines simple numeric types of a specific size,
   such as int32_t for a signed 32-bit integer.
   Many C/C++ compilers are not ANSI C99 by default, such as Microsoft Visual C/C++.
   Therefore, NI-XNET does not require use of ANSI C99.
   Since NI-XNET does not attempt to override ANSI C99 types (as defined in stdint.h),
   it uses legacy National Instruments numeric types such as i32. If desired, you can use
   ANSI C99 numeric types instead of the analogous NI-XNET numeric type
   (i.e. int32_t instead of i32). */

#ifndef _NIDAQ_Header_      // Traditional NI-DAQ header defines numeric types same as below.
#ifndef nNISS100_kCPP       // Same for NI SS layer.

#ifndef _NI_i8_DEFINED_
#define _NI_i8_DEFINED_
typedef signed char        i8;
#endif
#ifndef _NI_i16_DEFINED_
#define _NI_i16_DEFINED_
typedef signed short       i16;
#endif

   /*Define portalbe i32 type
   Since  NI-XNET does not require use of ANSI C99, and there is no standard that consistantly define 32bit type for 32bit and 64 bit OS
   For example, long could be 4 bytes on 32bit OS , but 8 bytes on 64 bit OS for certain compiler
   However Both MSVC and CVI  have  defined int as 4 byte value.
   For MSVC see http://msdn.microsoft.com/en-us/library/s3f49ktz.aspx
   For CVI see  http://zone.ni.com/reference/en-XX/help/370051Y-01/cvi/programmerref/data_types/

   GCC always support C99 type
   */
#ifndef _NI_i32_DEFINED_
#define _NI_i32_DEFINED_
#if (defined(_MSC_VER) || defined(_CVI_) || defined(__BORLANDC__))
   typedef signed int     i32;
#elif defined(__GNUC__)
   typedef int32_t i32;
#endif
#endif

#ifndef _NI_i64_DEFINED_
#define _NI_i64_DEFINED_
#if (defined(_MSC_VER) || defined(_CVI_) || defined(__BORLANDC__))
   typedef __int64     i64;
#elif defined(__GNUC__)
   typedef long long int   i64;
#endif
#endif

#ifndef _NI_u8_DEFINED_
#define _NI_u8_DEFINED_
typedef unsigned char      u8;
#endif

#ifndef _NI_u16_DEFINED_
#define _NI_u16_DEFINED_
typedef unsigned short     u16;
#endif

#ifndef _NI_u32_DEFINED_
#define _NI_u32_DEFINED_
#if (defined(_MSC_VER) || defined(_CVI_) || defined(__BORLANDC__))
   typedef unsigned int    u32;
#elif defined(__GNUC__)
   typedef uint32_t u32;
#endif
#endif

#ifndef _NI_u64_DEFINED_
#define _NI_u64_DEFINED_
#if (defined(_MSC_VER) || defined(_CVI_) || defined(__BORLANDC__))
   typedef unsigned __int64     u64;
#elif defined(__GNUC__)
   typedef unsigned long long int   u64;
#endif
#endif

#ifndef _NI_f32_DEFINED_
#define _NI_f32_DEFINED_
typedef float              f32;
#endif

#ifndef _NI_f64_DEFINED_
#define _NI_f64_DEFINED_
typedef double             f64;
#endif

typedef void*              nxVoidPtr;
typedef u32*               nxU32Ptr;
#endif // nNISS100_kCPP
#endif // _NIDAQ_Header_

   // Session Reference (handle).
typedef u32 nxSessionRef_t;

   // Database Reference (handle).
typedef u32 nxDatabaseRef_t;

typedef i32 nxStatus_t;       // Return value

   // Absolute timestamp.
typedef u64 nxTimestamp_t;

typedef struct _nxFlexRayStats_t {
      u32 NumSyntaxErrorChA;
      u32 NumSyntaxErrorChB;
      u32 NumContentErrorChA;
      u32 NumContentErrorChB;
      u32 NumSlotBoundaryViolationChA;
      u32 NumSlotBoundaryViolationChB;
   } nxFlexRayStats_t;

   // nxReadState nxState_J1939Comm StateValue
typedef struct _nxJ1939CommState_t {
      u32 PGN;
      u8  SourceAddress;
      u8  DestinationAddress;
      u8  TransmitError;
      u8  ReceiveError;
      u32 Reserved1;
      u32 Reserved2;
   } nxJ1939CommState_t;

/***********************************************************************
                                F R A M E
***********************************************************************/

   // Type
#define nxFrameType_CAN_Data                 0x00
#define nxFrameType_CAN_Remote               0x01
#define nxFrameType_CAN_BusError             0x02
#define nxFrameType_CAN20_Data               0x08
#define nxFrameType_CANFD_Data               0x10
#define nxFrameType_CANFDBRS_Data            0x18
#define nxFrameType_FlexRay_Data             0x20
#define nxFrameType_FlexRay_Null             0x21
#define nxFrameType_FlexRay_Symbol           0x22
#define nxFrameType_LIN_Data                 0x40
#define nxFrameType_LIN_BusError             0x41
#define nxFrameType_LIN_NoResponse           0x42
#define nxFrameType_J1939_Data               0xC0
#define nxFrameType_Special_Delay            0xE0
#define nxFrameType_Special_LogTrigger       0xE1
#define nxFrameType_Special_StartTrigger     0xE2


   /* For Data frames, your application may not be concerned with specifics for
   CAN, FlexRay, or LIN. For example, you can use fields of the frame to determine
   the contents of Payload, and write general-purpose code to map signal
   values in/out of the Payload data bytes.
   This macro can be used with the frame's Type to determine if the frame is a
   data frame. The macro is used in boolean conditionals. */
#define nxFrameType_IsData(frametype) \
            ((u8)(frametype) & 0x7) == 0)

#define nxFrameId_CAN_IsExtended             0x20000000

   // Macros to get fields of frame Identifier for FlexRay input
#define nxFrameId_FlexRay_Get_Slot(FrameId)        (u16)( ((u32)FrameId) & 0x0000FFFF)

   /* When Type is nxFrameType_FlexRay_Data,
   the following bitmasks are used with the Flags field.
   */
#define nxFrameFlags_FlexRay_Startup         0x01     // Startup frame
#define nxFrameFlags_FlexRay_Sync            0x02     // Sync frame
#define nxFrameFlags_FlexRay_Preamble        0x04     // Preamble bit
#define nxFrameFlags_FlexRay_ChA             0x10     // Transfer on Channel A
#define nxFrameFlags_FlexRay_ChB             0x20     // Transfer on Channel B

   /* When Type is nxFrameType_LIN_Data,
   the following bitmasks are used with the Flags field.
   */
#define nxFrameFlags_LIN_EventSlot           0x01     // Unconditional frame in event-triggered slot

   /* When Type is nxFrameType_CAN_Data, nxFrameType_CAN_Remote,
   nxFrameType_FlexRay_Data, or nxFrameType_LIN_Data,
   the following bitmasks are used with the Flags field.
   */
#define nxFrameFlags_TransmitEcho            0x80

   /* When Type is nxFrameType_FlexRay_Symbol,
   the following values are used with the first Payload byte (offset 0).
   */
#define nxFlexRaySymbol_MTS                  0x00
#define nxFlexRaySymbol_Wakeup               0x01

#define  nxInternal_PadPayload(paylod) \
            ( (u16)(paylod) ? (( (u16)(paylod) + 7) & 0x07F8) : 8)

#define  nxFrameFixed_t(payld) \
            struct { \
               nxTimestamp_t       Timestamp; \
               u32                 Identifier; \
               u8                  Type; \
               u8                  Flags; \
               u8                  Info; \
               u8                  PayloadLength; \
               u8                  Payload[ nxInternal_PadPayload(payld) ]; \
            }

// -----------------------------------------------------------------------------
// If you are using CVI version 2009 or earlier, you may see a compile error for
// this line. Upgrade to CVI version 2010 or later for the fix, disable the
// "Build with C99 extensions" compiler option in the "Build Options" dialog of
// CVI or edit your copy of the header file to resolve the error.
// -----------------------------------------------------------------------------
typedef nxFrameFixed_t(8) nxFrameCAN_t;
typedef nxFrameFixed_t(8) nxFrameLIN_t;
typedef nxFrameFixed_t(1) nxFrameVar_t;

#define nxFramePayldLenHigh_Mask_J1939       (7)

#define nxSizeofFrameHeader                  (16)

#define nxFrameSize(payload) \
            (  nxSizeofFrameHeader + nxInternal_PadPayload(payload) )

   /* Get Payload Length. For J1939, the lowest three bits of the Info field
   are the high bits of the PayloadLength (valid up to 1785). */
#define nxFrameGetPayloadLength(frameptr) \
            (  (u32) (frameptr)->PayloadLength | \
               ((frameptr)->Type == nxFrameType_J1939_Data ? \
                  (u32) ((frameptr)->Info & nxFramePayldLenHigh_Mask_J1939) << 8 : \
                  0) \
            )



   /* Set Payload Length. For J1939, the lowest three bits of the Info field
      are the high bits of the PayloadLength (valid up to 1785 bytes). Before using this
      macro with J1939, the frame type should be set to nxFrameType_J1939_Data */
#define nxFrameSetPayloadLength(frameptr,PayloadLen) \
   (frameptr)->PayloadLength = (u8)((PayloadLen)&0xFF); \
   if (nxFrameType_J1939_Data == (frameptr)->Type ) { \
         (frameptr)->Info = (u8)( ( (frameptr)->Info & ( ~ nxFramePayldLenHigh_Mask_J1939 ) ) | \
         (( (PayloadLen) >> 8 ) & nxFramePayldLenHigh_Mask_J1939 ));}


   /* Use this macro to iterate through variable-length frames.
   You call this macro as a function, as if it used the following prototype:
      nxFrameVar_t * nxFrameIterate(nxFrameVar_t * frameptr);
   The input parameter must be initialized to point to the header of a valid frame.
   The macro returns a pointer to the header of the next frame in the buffer.
   In other words, the macro will iterate from one variable-length frame to
   the next variable-length frame.
   */
#define nxFrameIterate(frameptr) \
            (nxFrameVar_t *) ( (u8 *)(frameptr) + nxFrameSize( nxFrameGetPayloadLength(frameptr) ) )


#ifndef _NX_FUNCTION_EXPORT

/***********************************************************************
       F U N C T I O N   P R O T O T Y P E S  :  S E S S I O N
***********************************************************************/

nxStatus_t _NXFUNC nxCreateSession (
                           const char * DatabaseName,
                           const char * ClusterName,
                           const char * List,
                           const char * Interface,
                           u32 Mode,
                           nxSessionRef_t * SessionRef);

nxStatus_t _NXFUNC nxCreateSessionByRef (
                           u32 NumberOfDatabaseRef,
                           nxDatabaseRef_t * ArrayOfDatabaseRef,
                           const char * Interface,
                           u32 Mode,
                           nxSessionRef_t * SessionRef);

nxStatus_t _NXFUNC nxGetProperty (
                           nxSessionRef_t SessionRef,
                           u32 PropertyID,
                           u32 PropertySize,
                           void * PropertyValue);

nxStatus_t _NXFUNC nxGetPropertySize (
                           nxSessionRef_t SessionRef,
                           u32 PropertyID,
                           u32 * PropertySize);

nxStatus_t _NXFUNC nxSetProperty (
                           nxSessionRef_t SessionRef,
                           u32 PropertyID,
                           u32 PropertySize,
                           void * PropertyValue);

nxStatus_t _NXFUNC nxGetSubProperty (
                           nxSessionRef_t SessionRef,
                           u32 ActiveIndex,
                           u32 PropertyID,
                           u32 PropertySize,
                           void * PropertyValue);

nxStatus_t _NXFUNC nxGetSubPropertySize (
                           nxSessionRef_t SessionRef,
                           u32 ActiveIndex,
                           u32 PropertyID,
                           u32 * PropertySize);

nxStatus_t _NXFUNC nxSetSubProperty (
                           nxSessionRef_t SessionRef,
                           u32 ActiveIndex,
                           u32 PropertyID,
                           u32 PropertySize,
                           void * PropertyValue);

nxStatus_t _NXFUNC nxReadFrame (
                           nxSessionRef_t SessionRef,
                           void * Buffer,
                           u32 SizeOfBuffer,
                           f64 Timeout,
                           u32 * NumberOfBytesReturned);

nxStatus_t _NXFUNC nxReadSignalSinglePoint (
                           nxSessionRef_t SessionRef,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           nxTimestamp_t * TimestampBuffer,
                           u32 SizeOfTimestampBuffer);

nxStatus_t _NXFUNC nxReadSignalWaveform (
                           nxSessionRef_t SessionRef,
                           f64 Timeout,
                           nxTimestamp_t * StartTime,
                           f64 * DeltaTime,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           u32 * NumberOfValuesReturned);

nxStatus_t _NXFUNC nxReadSignalXY (
                           nxSessionRef_t SessionRef,
                           nxTimestamp_t * TimeLimit,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           nxTimestamp_t * TimestampBuffer,
                           u32 SizeOfTimestampBuffer,
                           u32 * NumPairsBuffer,
                           u32 SizeOfNumPairsBuffer);

nxStatus_t _NXFUNC nxReadState (
                           nxSessionRef_t SessionRef,
                           u32 StateID,
                           u32 StateSize,
                           void * StateValue,
                           nxStatus_t * Fault);

nxStatus_t _NXFUNC nxWriteFrame (
                           nxSessionRef_t SessionRef,
                           void * Buffer,
                           u32 NumberOfBytesForFrames,
                           f64 Timeout);

nxStatus_t _NXFUNC nxWriteSignalSinglePoint (
                           nxSessionRef_t SessionRef,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer);

nxStatus_t _NXFUNC nxWriteState (
                           nxSessionRef_t SessionRef,
                           u32 StateID,
                           u32 StateSize,
                           void * StateValue);

nxStatus_t _NXFUNC nxWriteSignalWaveform (
                           nxSessionRef_t SessionRef,
                           f64 Timeout,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer);

nxStatus_t _NXFUNC nxWriteSignalXY (
                           nxSessionRef_t SessionRef,
                           f64 Timeout,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           nxTimestamp_t * TimestampBuffer,
                           u32 SizeOfTimestampBuffer,
                           u32 * NumPairsBuffer,
                           u32 SizeOfNumPairsBuffer);

nxStatus_t _NXFUNC nxConvertFramesToSignalsSinglePoint (
                           nxSessionRef_t SessionRef,
                           void * FrameBuffer,
                           u32 NumberOfBytesForFrames,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           nxTimestamp_t * TimestampBuffer,
                           u32 SizeOfTimestampBuffer);

nxStatus_t _NXFUNC nxConvertSignalsToFramesSinglePoint (
                           nxSessionRef_t SessionRef,
                           f64 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           void * Buffer,
                           u32 SizeOfBuffer,
                           u32 * NumberOfBytesReturned);

nxStatus_t _NXFUNC nxConvertFramesToByteArraySinglePoint (
                           nxSessionRef_t SessionRef,
                           void * FrameBuffer,
                           u32 NumberOfBytesForFrames,
                           u8 * ValueBuffer,
                           u32 SizeOfValueBuffer);

nxStatus_t _NXFUNC nxConvertByteArrayToFramesSinglePoint (
                           nxSessionRef_t SessionRef,
                           u8 * ValueBuffer,
                           u32 SizeOfValueBuffer,
                           void * Buffer,
                           u32 SizeOfBuffer,
                           u32 * NumberOfBytesReturned);


nxStatus_t _NXFUNC nxBlink (
                           nxSessionRef_t InterfaceRef,
                           u32 Modifier);

nxStatus_t _NXFUNC nxClear (
                           nxSessionRef_t SessionRef);

nxStatus_t _NXFUNC nxConnectTerminals (
                           nxSessionRef_t SessionRef,
                           const char * source,
                           const char * destination);


nxStatus_t _NXFUNC nxDisconnectTerminals (
                           nxSessionRef_t SessionRef,
                           const char * source,
                           const char * destination);

nxStatus_t _NXFUNC nxFlush (
                           nxSessionRef_t SessionRef);

nxStatus_t _NXFUNC nxStart (
                           nxSessionRef_t SessionRef,
                           u32 Scope);

nxStatus_t _NXFUNC nxStop (
                           nxSessionRef_t SessionRef,
                           u32 Scope);

void _NXFUNC nxStatusToString (
                           nxStatus_t Status,
                           u32 SizeofString,
                           char * StatusDescription);

nxStatus_t _NXFUNC nxSystemOpen (
                           nxSessionRef_t * SystemRef);

nxStatus_t _NXFUNC nxSystemClose (
                           nxSessionRef_t SystemRef);

nxStatus_t _NXFUNC nxWait (
                           nxSessionRef_t SessionRef,
                           u32 Condition,
                           u32 ParamIn,
                           f64 Timeout,
                           u32 * ParamOut);


/***********************************************************************
       F U N C T I O N   P R O T O T Y P E S  :  D A T A B A S E
***********************************************************************/

nxStatus_t _NXFUNC nxdbOpenDatabase (
                           const char * DatabaseName,
                           nxDatabaseRef_t * DatabaseRef);

nxStatus_t _NXFUNC nxdbCloseDatabase (
                           nxDatabaseRef_t DatabaseRef,
                           u32 CloseAllRefs);

nxStatus_t _NXFUNC nxdbCreateObject (
                           nxDatabaseRef_t ParentObjectRef,
                           u32 ObjectClass,
                           const char * ObjectName,
                           nxDatabaseRef_t * DbObjectRef);

nxStatus_t _NXFUNC nxdbFindObject (
                           nxDatabaseRef_t ParentObjectRef,
                           u32 ObjectClass,
                           const char * ObjectName,
                           nxDatabaseRef_t * DbObjectRef);

nxStatus_t _NXFUNC nxdbDeleteObject (
                           nxDatabaseRef_t DbObjectRef);

nxStatus_t _NXFUNC nxdbSaveDatabase (
                           nxDatabaseRef_t DatabaseRef,
                           const char * DbFilepath);

nxStatus_t _NXFUNC nxdbGetProperty (
                           nxDatabaseRef_t DbObjectRef,
                           u32 PropertyID,
                           u32 PropertySize,
                           void * PropertyValue);

nxStatus_t _NXFUNC nxdbGetPropertySize (
                           nxDatabaseRef_t DbObjectRef,
                           u32 PropertyID,
                           u32 * PropertySize);

nxStatus_t _NXFUNC nxdbSetProperty (
                           nxDatabaseRef_t DbObjectRef,
                           u32 PropertyID,
                           u32 PropertySize,
                           void * PropertyValue);

/* The NI-XNET documentation does not describe the Mode parameter.
   The Mode parameter was added near release. It specifies the type of DBC attribute
   that you want to search for. In the v1.2 release there is only one value:
      nxGetDBCMode_Attribute (0)
   Other values will be supported in subsequent releases. */
nxStatus_t _NXFUNC nxdbGetDBCAttributeSize (
                           nxDatabaseRef_t DbObjectRef,
                           u32 Mode,
                           const char* AttributeName,
                           u32 *AttributeTextSize);

/* The NI-XNET documentation does not describe the Mode parameter.
   The Mode parameter was added near release. It specifies the type of DBC attribute
   that you want to search for. In the v1.2 release there is only one value:
      nxGetDBCMode_Attribute (0)
   Other values will be supported in subsequent releases. */
nxStatus_t _NXFUNC nxdbGetDBCAttribute (
                           nxDatabaseRef_t DbObjectRef,
                           u32 Mode,
                           const char* AttributeName,
                           u32 AttributeTextSize,
                           char* AttributeText,
                           u32 * IsDefault);

nxStatus_t _NXFUNC nxdbMerge (
                           nxDatabaseRef_t TargetClusterRef,
                           nxDatabaseRef_t SourceObjRef,
                           u32 CopyMode,
                           char *Prefix,
                           u32 WaitForComplete,
                           u32 *PercentComplete);


nxStatus_t _NXFUNC nxdbAddAlias (
                           const char * DatabaseAlias,
                           const char * DatabaseFilepath,
                           u32          DefaultBaudRate);

nxStatus_t _NXFUNC nxdbAddAlias64 (
                           const char * DatabaseAlias,
                           const char * DatabaseFilepath,
                           u64          DefaultBaudRate);

nxStatus_t _NXFUNC nxdbRemoveAlias (
                           const char * DatabaseAlias);

nxStatus_t _NXFUNC nxdbDeploy (
                           const char * IPAddress,
                           const char * DatabaseAlias,
                           u32 WaitForComplete,
                           u32 * PercentComplete);

nxStatus_t _NXFUNC nxdbUndeploy (
                           const char * IPAddress,
                           const char * DatabaseAlias);

nxStatus_t _NXFUNC nxdbGetDatabaseList (
                           const char * IPAddress,
                           u32 SizeofAliasBuffer,
                           char * AliasBuffer,
                           u32 SizeofFilepathBuffer,
                           char * FilepathBuffer,
                           u32 * NumberOfDatabases);

nxStatus_t _NXFUNC nxdbGetDatabaseListSizes (
                           const char * IPAddress,
                           u32 * SizeofAliasBuffer,
                           u32 * SizeofFilepathBuffer);

#endif // _NX_FUNCTION_EXPORT

#ifdef __cplusplus
   /* See top of header file.  */
   }
#endif // __cplusplus

#endif // ___nixnet_h___
