//******************************************************************************
//                              def16n32.h
//******************************************************************************
// Version:      1.0
// Date:         04/97
// Compiler:     BC 4.5 'n VC 4.0
// Author:       Matthias Lohse / Alex Lunkenheimer
//               Tel.:     0711/811-3949
//               Memo IKS: ETESW_LR
//               e-mail:   allunken@si.bosch.de
//                         etesw_lr@siiks_a1.bosch.de
// Copyright:    ETAS GmbH & Co KG, Schwieberdingen
//******************************************************************************
// Brief description: This file provides compiler independend function 
// declaration via 'returnType CALL_CONV function(...)'.
//******************************************************************************

// undef used constants
#undef CALL_CONV
#undef CPLUSPLUS
#undef EXTERN_C

// enable mixing C 'n C++
#if defined __cplusplus || defined _cplusplus
 #define CPLUSPLUS
 #define EXTERN_C extern "C"
#else
 #define EXTERN_C
#endif

// enable common BC++ 'n VC++ DLL function declaration
#ifdef _MSC_VER		// VC++
	#define CALL_CONV __declspec( dllexport )
#else			// BC++
	#define CALL_CONV _stdcall _export
#endif


// example:
//
// #include "..\include\BcNVcDll.h"
// EXTERN_C BOOL CALL_CONV asapRead(PR *pr);
// ...
