#ifndef MDF3BLOCK_H
#define MDF3BLOCK_H

#include "mdfTypes.h"

#pragma pack(2)

#define  SIZE_ID     sizeof(mdf3FileId)
#define  SIZE_HD     sizeof(mdf3HDBlock)
#define  SIZE_TX     sizeof(mdf3TXBlock)
#define  SIZE_DG     sizeof(mdf3DGBlock)
#define  SIZE_CG     sizeof(mdf3CGBlock)
#define  SIZE_CN     sizeof(mdf3CNBlock)
#define  SIZE_CC     sizeof(mdf3CCBlock)


struct mdf3FileId
{
    M_CHAR   id_file[8];  // File identifier always contains "MDF". ("MDF" followed by five spaces, no zero termination!).
    M_CHAR   id_format[8];  // Format identifier, a textual representation of the format version for display, e.g. “4.00 ” (no zero termination required).
    M_CHAR   id_prog[8];  // Program identifier, to identify the program which generated the MDF file (no zero termination required). This program identifier serves only for compatibility with previous MDF format versions. Detailed information about the generating application must be written to the first FHBLOCK referenced by the HDBLOCK. As a recommendation, the program identifier inserted into the 8 characters should be the base name (first 8 characters) of the EXE/DLL of the writing application. Alternatively, also version information of the application can be appended (e.g. "MyApp45" for version 4.5 of MyApp.exe).
    M_UINT16 id_order;    // Byte Order: 0=Intel (MDF4: always 0, MDF3: only 0 supported)
    M_UINT16 id_float;    // Float Format: 0=IEEE 754 (MDF4: always 0, MDF3: only 0 supported)
    M_UINT16 id_ver;      // Version number of the MDF format, i.e. 400 for this version
    M_UINT16 id_codepage;
    M_BYTE   id_res2[32]; // Reserved
};

struct mdf3HDBlock
{
    M_CHAR   hd_id[2];
    M_UINT16 hd_size;
    M3LINK   hd_dg_link;
    M3LINK   hd_tx_link;
    M3LINK   hd_pr_link;
    M_UINT16 hd_num_dg;
    M_CHAR   hd_start_date[10]; //DD:MM:YYYY
    M_CHAR   hd_start_time[8];  //HH:MM:SS
    M_CHAR   hd_author[32];
    M_CHAR   hd_org[32];
    M_CHAR   hd_proj[32];
    M_CHAR   hd_subj[32];
    M_UINT64 hd_timestamp_ns;
    M_INT16  hd_time_UTC;
    M_UINT16 hd_time_quality;
    M_CHAR   hd_time_id[32];
};

struct mdf3TXBlock
{
    M_CHAR   tx_id[2];
    M_UINT16 tx_size;
    M_CHAR   tx_comment[196];
};

struct mdf3DGBlock
{
    M_CHAR   dg_id[2];
    M_UINT16 dg_size;
    M3LINK   dg_nextdg_link;
    M3LINK   dg_cg_link;
    M3LINK   dg_tr_link;
    M3LINK   dg_data_link;
    M_UINT16 dg_numcg;
    M_UINT16 dg_num_rcdid;
    M_UINT32 dg_res;

};

struct mdf3CGBlock
{
    M_CHAR   cg_id[2];
    M_UINT16 cg_size;
    M3LINK   cg_nextcg_link;
    M3LINK   cg_cn_link;
    M3LINK   cg_tx_link;
    M_UINT16 cg_rcd_id;
    M_UINT16 cg_numcn;
    M_UINT16 cg_size_data;
    M_UINT32 cg_num_rcd;
    M3LINK   cg_sample;

};

struct mdf3CNBlock
{
    M_CHAR   cn_id[2];
    M_UINT16 cn_size;
    M3LINK   cn_nextcn_link;
    M3LINK   cn_cc_link;
    M3LINK   cn_ce_link;
    M3LINK   cn_cdb_link;
    M3LINK   cn_tx_link;
    M_UINT16 cn_cn_type;        //0=data channel; 1=time channel
    M_CHAR   cn_shortname[32];
    M_CHAR   cn_sig_desc[128];
    M_UINT16 cn_startbit;
    M_UINT16 cn_lengthbit;
    M_UINT16 cn_datatype;
    M_UINT16 cn_range_valid;
    M_REAL   cn_min;
    M_REAL   cn_max;
    M_REAL   cn_sample_rate;
    M3LINK   cn_longname_link;
    M3LINK   cn_displayname_link;
    M_UINT16 cn_byteoffset;

};

struct mdf3CCBlock
{
    M_CHAR   cc_id[2];
    M_UINT16 cc_size;
    M_UINT16   cc_phy_rangevalid;
    M_REAL   cc_phy_min;
    M_REAL   cc_phy_max;
    M_CHAR   cc_phy_unit[20];
    M_UINT16 cc_convtype;
    M_UINT16 cc_addsize_info;

    M_REAL   cc_variables[2];

};

struct SigAttr
{
    M_CHAR    sig_shortname[32];
    M_CHAR    sig_desc[128];
    M_UINT16  sig_cntype;            //0=data channel; 1=time channel
    M_UINT16  sig_startbit;
    M_UINT16  sig_lengthbit;
    M_UINT16  sig_datatype;        //for default byteorder in IDBLOCK: 0=unsigned integer; 1=signed integer; 2=float(4 bytes); 3=double(8 bytes)
    M_REAL    sig_min;
    M_REAL    sig_max;
    M_REAL    sig_sample_rate;
};

#endif // MDF3BLOCK_H
