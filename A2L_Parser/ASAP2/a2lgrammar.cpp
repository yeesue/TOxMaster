// HEXplorer is an Asap and HEX file editor
// Copyright (C) 2011  <Christophe Hoel>
//
// This file is part of HEXplorer.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// please contact the author at : christophe.hoel@gmail.com

#include "a2lgrammar.h"
#include "Nodes/a2lfile.h"
#include "qdebug.h"

A2lGrammar::A2lGrammar()
{
    // initialize Nodes
    initSub_function();
    initIn_measurement();
    initOut_measurement();
    initAnnotation();
    initAnnotation_text();
    initAxis_descr();
    initAxis_pts();
    initCharacteristic();
    initCompu_method();
    initRecord_layout();
    initMeasurement();
    initCalibration_handle();
    initCalibration_method();
    initCompuTab();
    initCompuVtab();
    initCompuVtabRange();
    initDef_characteristic();
    initFormula();
    initFunction();
    initFunction_list();
    initGroup();
    initHeader();
    initProject();
    initModule();
    initMemory_segment();
    initMemory_layout();
    initMod_common();
    initMod_par();
    initUnit();
    initDaq();
    initIf_data();
    initEvent();
    initDaq_list();
    initXcp_on_can();
    initDaq_memory_consumption();
    initEvent_can_id_list();
    initDaq_list_can_id();
    initSegment();
    initChecksum();
    initPage();
    initCan_fd();

    // initialize Items
    initAddr_epk();
    initAlignment_byte();
    initAlignment_float32_ieee();
    initAlignment_float64_ieee();
    initAlignment_long();
    initAlignment_word();
    initAlignment_int64();
    initAnnotation_label();
    initAnnotation_origin();
    initArray_size();
    initAxis_pts_ref();
    initAxis_pts_x();
    initAxis_pts_y();
    initBit_mask();
    initByte_order();
    initCalibration_handle_text();
    initCoeffs();
    initCoeffs_linear();
    initCompu_tab_ref();
    initCpu_type();
    initCustomer();
    initCustomer_number();
    initData_size();
    initDefault_value();
    initDefault_value_numeric();
    initDeposit();
    initDisplay_identifier();
    initEcu();
    initEcu_address();
    initEcu_address_extension();
    initEpk();
    initExtended_limits();
    initFix_axis_par();
    initFix_axis_par_dist();
    initFnc_values();
    initFormat();
    initFormula_inv();
    initMatrix_dim();
    initMax_grad();
    initMax_refresh();
    initNo_axis_pts_x();
    initNo_axis_pts_y();
    initNumber();
    initPhone_no();
    initProject_no();
    initRead_only();
    initRef_unit();
    initStatus_string_ref();
    initSystem_constant();
    initUser();
    initVersion();
    initS_rec_layout();
    initSrc_addr_x();
    initSrc_addr_y();
    initSupplier();
    initFunction_version();
    initSi_exponents();
    initUnit_conversion();
    initSymbol_link();
    initDaq_list_type();
    initMax_odt();
    initMax_odt_entries();
    initFirst_pid();
    initEvent_fixed();
    initReserved();
    initCan_id_master();
    initCan_id_slave();
    initBaudrate();
    initSample_point();
    initSample_rate();
    initBtl_cycles();
    initSjw();
    initSync_edge();
    initMax_bus_load();
    initTransport_layer_instance();
    initFixed();
    initMax_blcok_size();
    initInit_segment();
    initNo_of_interfaces();
    initLayout();
    initRead_write();
    initPhys_unit();
    initMax_dlc();
    initCan_fd_data_transfer_baudrate();
    initSecondary_sample_point();
    initTransceiver_delay_compensation();
    initMax_dlc_required();
}

QHash<QString, TokenTyp> A2lGrammar::initKeywords()
{
    //----------Token Type : Keyword ----------------------

    QHash<QString, TokenTyp> keywordsList;
    QList<std::string> list;

    list << "ASAP2_VERSION" << "A2ML_VERSION" << "AXIS_DESCR" << "AXIS_PTS_REF" << "PROJECT"
         << "PROJECT_NO" << "VERSION" << "HEADER" << "DISPLAY_IDENTIFIER"
         << "MODULE" << "A2ML" << "MOD_PAR" << "CHARACTERISTIC" << "FUNCTION_LIST"
        << "AXIS_PTS" << "BYTE_ORDER" << "ANNOTATION" << "MEASUREMENT" << "LOC_MEASUREMENT"
        << "COMPU_TAB" << "GROUP" <<  "RECORD_LAYOUT" << "REF_CHARACTERISTIC"
        << "IF_DATA" << "SYSTEM_CONSTANT" << "EXTENDED_LIMITS" << "ANNOTATION_LABEL"
        << "ANNOTATION_ORIGIN" << "ANNOTATION_TEXT" << "FORMAT" << "ECU_ADDRESS" << "ARRAY_SIZE"
        << "BIT_MASK" << "ADDR_EPK" << "EPK" << "CUSTOMER_NO" << "USER" << "PHONE_NO" << "ECU"
        << "CPU_TYPE" << "MEMORY_SEGMENT" << "CALIBRATION_METHOD" << "CALIBRATION_HANDLE"
        << "CALIBRATION_HANDLE_TEXT" << "FUNCTION" << "MOD_COMMON" << "COMPU_METHOD" << "COMPU_VTAB"
        << "COMPU_VTAB_RANGE" << "MEMORY_LAYOUT" << "S_REC_LAYOUT" << "SRC_ADDR_X" << "SRC_ADDR_Y"
        << "/begin" << "/end" << "DEPOSIT" << "NUMBER" << "READ_ONLY" << "FIX_AXIS_PAR"
        << "COEFFS" << "COEFFS_LINEAR" << "COMPU_TAB_REF" << "FORMULA" << "FORMULA_INV" << "REF_UNIT"
        << "STATUS_STRING_REF" << "DEFAULT_VALUE" << "DEFAULT_VALUE_NUMERIC" << "DEF_CHARACTERISTIC" <<"OUT_MEASUREMENT"
        << "NO_AXIS_PTS_X" << "NO_AXIS_PTS_Y" << "AXIS_PTS_X" << "AXIS_PTS_Y" << "FNC_VALUES"
        << "ALIGNMENT_FLOAT32_IEEE" << "ALIGNMENT_FLOAT64_IEEE" << "ALIGNMENT_BYTE" << "ALIGNMENT_WORD" << "ALIGNMENT_LONG" <<"ALIGNMENT_INT64"
        << "FUNCTION_VERSION" << "IN_MEASUREMENT" << "SUB_FUNCTION" << "MAX_REFRESH" << "CUSTOMER" << "SUPPLIER" << "UNIT" << "SI_EXPONENTS" << "UNIT_CONVERSION"
        << "DATA_SIZE" << "SUB_GROUP" << "ROOT" << "REF_MEASUREMENT" << "ECU_ADDRESS_EXTENSION" << "MATRIX_DIM" << "FIX_AXIS_PAR_DIST"
        << "STATIC_RECORD_LAYOUT" << "MAX_GRAD" << "VARIANT_CODING" << "VAR_NAMING" << "VAR_SEPARATOR" << "SYMBOL_LINK" << "DAQ" << "DAQ_MEMORY_CONSUMPTION"
        << "EVENT" << "STIM" << "DAQ_STIM" << "DAQ_LIST" << "DAQ_LIST_TYPE" << "MAX_ODT" << "MAX_ODT_ENTRIES" << "FIRST_PID" << "EVENT_FIXED"
        << "XCP_ON_CAN" << "CAN_ID_MASTER" << "CAN_ID_SLAVE" << "BAUDRATE" << "SAMPLE_POINT" << "SAMPLE_RATE" << "SJW"
        << "BTL_CYCLES" << "SYNC_EDGE" << "MAX_BUS_LOAD" << "TRANSPORT_LAYER_INSTANCE" << "EVENT_CAN_ID_LIST" <<"FIXED"<<"DAQ_LIST_CAN_ID"
        << "SEGMENT" << "CHECKSUM" << "PAGE" << "MAX_BLOCK_SIZE" << "INIT_SEGMENT" << "NO_OF_INTERFACES" << "LAYOUT" << "READ_WRITE" << "PHYS_UNIT"
        << "CAN_FD" << "MAX_DLC" << "CAN_FD_DATA_TRANSFER_BAUDRATE" << "SECONDARY_SAMPLE_POINT" << "TRANSCEIVER_DELAY_COMPENSATION";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Keyword);

    list.clear();

    //----------Token Type : Type ----------------------

    list << "ASCII" << "CURVE" << "MAP" << "CUBOID" << "CUBE_4" << "CUBE_5" << "VAL_BLK" << "VALUE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Type);

    list.clear();

    //----------Token Type : DataType ----------------------

    list << "UBYTE" << "SBYTE" << "UWORD" << "SWORD" << "ULONG" << "SLONG" << "A_UINT64"
            << "A_INT64" << "FLOAT32_IEEE" << "FLOAT64_IEEE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), DataType);

    list.clear();

    //----------Token Type : Datasize ----------------------

    list << "BYTE" << "WORD" << "LONG";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Datasize);

    list.clear();

    //----------Token Type : Addrtype ----------------------

    list << "PBYTE" << "PWORD" << "PLONG" << "DIRECT";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Addrtype);

    list.clear();

    //----------Token Type : Byteorder ----------------------

    list << "LITTLE_ENDIAN" << "BIG_ENDIAN" << "MSB_LAST" << "MSB_FIRST";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Byteorder);

    list.clear();

    //----------Token Type : IndexMode ----------------------

    list << "ALTERNATE_CURVES" <<"ALTERNATE_WITH_X" << "ALTERNATE_WITH_Y" << "COLUMN_DIR"
            << "ROW_DIR";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), IndexMode);

    list.clear();

    //----------Token Type : Indexorder ----------------------

    list << "INDEX_DECR" <<"INDEX_INCR";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Indexorder);

    list.clear();

    //----------Token Type : ConversionType ----------------------

    list << "RAT_FUNC" << "TAB_VERB" << "IDENTICAL" << "FORM" << "LINEAR" << "TAB_INTP"
            << "TAB_NOINTP";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), ConversionType);

    list.clear();

    //----------Token Type : Attribute ----------------------

    list << "CURVE_AXIS" << "COM_AXIS" << "FIX_AXIS" << "RES_AXIS" << "STD_AXIS";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Attribute);

    list.clear();

    //----------Token Type : Mode ----------------------

    list << "ABSOLUTE" << "REFERENCE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), Mode);

    list.clear();

    //----------Token Type : PrgType ----------------------

    list << "CALIBRATION_VARIABLES" << "CODE" << "DATA" << "EXCLUDE_FROM_FLASH" << "OFFLINE_DATA"
            << "RESERVED" << "SERAM" << "VARIABLES" << "PRG_CODE" << "PRG_DATA" << "PRG_RESERVED";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), PrgType);

    list.clear();

    //----------Token Type : MemoryType ----------------------

    list << "EEPROM" << "EPROM" << "FLASH" << "RAM" << "ROM" << "REGISTER";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), MemoryType);

    list.clear();

    //----------Token Type : MemAttribute ----------------------

    list << "INTERN" << "EXTERN";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), MemAttribute);
    list.clear();

    //----------Token Type : UnitType ----------------------

    list << "DERIVED" << "EXTENDED_SI";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), UnitType);
    list.clear();

    //----------Token Type : DaqType ----------------------

    list << "STATIC" << "DYNAMIC";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), DaqType);
    list.clear();

    //----------Token Type : OptimisationType ----------------------

    list << "OPTIMISATION_TYPE_DEFAULT" << "OPTIMISATION_TYPE_ODT_TYPE_16" << "OPTIMISATION_TYPE_ODT_TYPE_32"
         << "OPTIMISATION_TYPE_ODT_TYPE_64" << "OPTIMISATION_TYPE_ODT_TYPE_ALIGNMENT" << "OPTIMISATION_TYPE_MAX_ENTRY_SIZE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), OptimisationType);
    list.clear();

    //----------Token Type : AddressExtType ----------------------

    list << "ADDRESS_EXTENSION_FREE" << "ADDRESS_EXTENSION_ODT" << "ADDRESS_EXTENSION_DAQ";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), AddressExtType);
    list.clear();

    //----------Token Type : IdentificationFieldType ----------------------

    list << "IDENTIFICATION_FIELD_TYPE_ABSOLUTE" << "IDENTIFICATION_FIELD_TYPE_RELATIVE_BYTE"
         << "IDENTIFICATION_FIELD_TYPE_RELATIVE_WORD" << "IDENTIFICATION_FIELD_TYPE_RELATIVE_WORD_ALIGNED";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), IdentificationFieldType);
    list.clear();

    //----------Token Type : GranularityOdtEntrySize ----------------------

    list << "GRANULARITY_ODT_ENTRY_SIZE_DAQ_BYTE" << "GRANULARITY_ODT_ENTRY_SIZE_DAQ_WORD"
         << "GRANULARITY_ODT_ENTRY_SIZE_DAQ_DWORD" << "GRANULARITY_ODT_ENTRY_SIZE_DAQ_DLONG";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), GranularityOdtEntrySize);
    list.clear();

    //----------Token Type : OverloadIndication ----------------------

    list << "NO_OVERLOAD_INDICATION" << "OVERLOAD_INDICATION_PID"
         << "OVERLOAD_INDICATION_EVENT";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), OverloadIndication);
    list.clear();

    //----------Token Type : SampleRateType ----------------------

    list << "SINGLE" << "DUAL" <<"TRIPLE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), SampleRateType);
    list.clear();

    //----------Token Type : ChecksumType ----------------------

    list << "XCP_ADD_11" << "XCP_ADD_12" <<"XCP_ADD_14" << "XCP_ADD_22" << "XCP_ADD_24" <<"XCP_ADD_44"
         << "XCP_CRC_16" << "XCP_CRC_16_CITT" << "XCP_CRC_32" << "XCP_USER_DEFINED";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), ChecksumType);
    list.clear();

    //----------Token Type : EcuAccessType ----------------------

    list << "ECU_ACCESS_NOT_ALLOWED" << "ECU_ACCESS_WITHOUT_XCP_ONLY"
         <<"ECU_ACCESS_WITH_XCP_ONLY" << "ECU_ACCESS_DONT_CARE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), EcuAccessType);
    list.clear();

    //----------Token Type : XcpReadAccessType ----------------------

    list << "XCP_READ_ACCESS_NOT_ALLOWED" << "XCP_READ_ACCESS_WITHOUT_ECU_ONLY"
         <<"XCP_READ_ACCESS_WITH_ECU_ONLY" << "XCP_READ_ACCESS_DONT_CARE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), XcpReadAccessType);
    list.clear();

    //----------Token Type : XcpWriteAccessType ----------------------

    list << "XCP_WRITE_ACCESS_NOT_ALLOWED" << "XCP_WRITE_ACCESS_WITHOUT_ECU_ONLY"
         <<"XCP_WRITE_ACCESS_WITH_ECU_ONLY" << "XCP_WRITE_ACCESS_DONT_CARE";

    foreach (std::string str, list)
        keywordsList.insert(QString(str.c_str()), XcpWriteAccessType);
    list.clear();

    return keywordsList;
}

void A2lGrammar::initSub_function()
{

}

void A2lGrammar::initIn_measurement()
{

}

void A2lGrammar::initOut_measurement()
{

}

void A2lGrammar::initAnnotation()
{
    annotation.factoryOptNode.insert("ANNOTATION_TEXT", &ANNOTATION_TEXT::nodeFactory);

    annotation.factoryOptItem.insert("ANNOTATION_LABEL", &ANNOTATION_LABEL::itemFactory);
    annotation.factoryOptItem.insert("ANNOTATION_ORIGIN", &ANNOTATION_ORIGIN::itemFactory);
}

void A2lGrammar::initAnnotation_text()
{
    annotation_text.typePar << String;
    annotation_text.namePar << "Annotation_text";
}

void A2lGrammar::initAxis_descr()
{
    axis_descr.typePar << Attribute << Identifier << Identifier << Integer << Float << Float;
    axis_descr.namePar << "Attribute" << "InputQuantity" << "Conversion" << "MaxAxisPoints"
            << "LowerLimit" << "UpperLimit";

    axis_descr.factoryOptItem.insert("FORMAT", &FORMAT::itemFactory);
    axis_descr.factoryOptItem.insert("EXTENDED_LIMITS", &EXTENDED_LIMITS::itemFactory);
    axis_descr.factoryOptItem.insert("DEPOSIT", &DEPOSIT::itemFactory);
    axis_descr.factoryOptItem.insert("AXIS_PTS_REF", &AXIS_PTS_REF::itemFactory);
    axis_descr.factoryOptItem.insert("FIX_AXIS_PAR", &FIX_AXIS_PAR::itemFactory);
    axis_descr.factoryOptItem.insert("FIX_AXIS_PAR_DIST", &FIX_AXIS_PAR_DIST::itemFactory);
    axis_descr.factoryOptItem.insert("READ_ONLY", &READ_ONLY::itemFactory);
    axis_descr.factoryOptItem.insert("BYTE_ORDER", &Byte_Order::itemFactory);
    axis_descr.factoryOptItem.insert("MAX_GRAD", &MAX_GRAD::itemFactory);
}

void A2lGrammar::initCharacteristic()
{
    characteristic.typePar << Identifier << String << Type << Hex << Identifier << Float << Identifier << Float << Float;
    characteristic.namePar << "Name" << "LongIdentifier" << "Type" << "Address" << "Deposit" << "MaxDiff"
            << "Conversion" << "LowerLimit" << "UpperLimit";

    characteristic.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
    characteristic.factoryOptNode.insert("ANNOTATION", &ANNOTATION::nodeFactory);
    characteristic.factoryOptNode.insert("AXIS_DESCR", &AXIS_DESCR::nodeFactory);
    characteristic.factoryOptNode.insert("FUNCTION_LIST", &FUNCTION_LIST::nodeFactory);

    characteristic.factoryOptItem.insert("BIT_MASK", &BIT_MASK::itemFactory);
    characteristic.factoryOptItem.insert("BYTE_ORDER", &Byte_Order::itemFactory);
    characteristic.factoryOptItem.insert("FORMAT", &FORMAT::itemFactory);
    characteristic.factoryOptItem.insert("EXTENDED_LIMITS", &EXTENDED_LIMITS::itemFactory);
    characteristic.factoryOptItem.insert("NUMBER", &NUMBER::itemFactory);
    characteristic.factoryOptItem.insert("READ_ONLY", &READ_ONLY::itemFactory);
    characteristic.factoryOptItem.insert("MAX_REFRESH", &MAX_REFRESH::itemFactory);
    characteristic.factoryOptItem.insert("DISPLAY_IDENTIFIER", &DISPLAY_IDENTIFIER::itemFactory);
    characteristic.factoryOptItem.insert("ECU_ADDRESS_EXTENSION", &ECU_ADDRESS_EXTENSION::itemFactory);
    characteristic.factoryOptItem.insert("MATRIX_DIM", &MATRIX_DIM::itemFactory);
    characteristic.factoryOptItem.insert("SYMBOL_LINK", &SYMBOL_LINK::itemFactory);
}

void A2lGrammar::initAxis_pts()
{
    axis_pts.typePar << Identifier << String << Hex << Identifier << Identifier << Float << Identifier
            << Integer << Float << Float;
    axis_pts.namePar << "Name" << "LongIdentifier" << "Address" << "InputQuantity" << "Deposit" << "MaxDiff"
            << "Conversion" << "MaxAxisPoints" << "LowerLimit" << "UpperLimit";

    axis_pts.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
    axis_pts.factoryOptNode.insert("ANNOTATION", &ANNOTATION::nodeFactory);
    axis_pts.factoryOptNode.insert("FUNCTION_LIST", &FUNCTION_LIST::nodeFactory);
    axis_pts.factoryOptItem.insert("READ_ONLY", &READ_ONLY::itemFactory);
    axis_pts.factoryOptItem.insert("FORMAT", &FORMAT::itemFactory);
    axis_pts.factoryOptItem.insert("EXTENDED_LIMITS", &EXTENDED_LIMITS::itemFactory);
    axis_pts.factoryOptItem.insert("DEPOSIT", &DEPOSIT::itemFactory);
    axis_pts.factoryOptItem.insert("DISPLAY_IDENTIFIER", &DISPLAY_IDENTIFIER::itemFactory);
    axis_pts.factoryOptItem.insert("ECU_ADDRESS_EXTENSION", &ECU_ADDRESS_EXTENSION::itemFactory);
    axis_pts.factoryOptItem.insert("BYTE_ORDER", &Byte_Order::itemFactory);
    axis_pts.factoryOptItem.insert("SYMBOL_LINK", &SYMBOL_LINK::itemFactory);
}

void A2lGrammar::initRecord_layout()
{
    record_layout.typePar << Identifier;
    record_layout.namePar << "Name";

    record_layout.factoryOptItem.insert("NO_AXIS_PTS_X", &NO_AXIS_PTS_X::itemFactory);
    record_layout.factoryOptItem.insert("NO_AXIS_PTS_Y", &NO_AXIS_PTS_Y::itemFactory);
    record_layout.factoryOptItem.insert("AXIS_PTS_X", &AXIS_PTS_X::itemFactory);
    record_layout.factoryOptItem.insert("AXIS_PTS_Y", &AXIS_PTS_Y::itemFactory);
    record_layout.factoryOptItem.insert("FNC_VALUES", &FNC_VALUES::itemFactory);
    record_layout.factoryOptItem.insert("SRC_ADDR_X", &SRC_ADDR_X::itemFactory);
    record_layout.factoryOptItem.insert("SRC_ADDR_Y", &SRC_ADDR_Y::itemFactory);
    record_layout.factoryOptItem.insert("ALIGNMENT_BYTE", &ALIGNMENT_BYTE::itemFactory);
    record_layout.factoryOptItem.insert("ALIGNMENT_LONG", &ALIGNMENT_LONG::itemFactory);
    record_layout.factoryOptItem.insert("ALIGNMENT_WORD", &ALIGNMENT_WORD::itemFactory);
    record_layout.factoryOptItem.insert("ALIGNMENT_FLOAT32_IEEE", &ALIGNMENT_FLOAT32_IEEE::itemFactory);
    record_layout.factoryOptItem.insert("ALIGNMENT_FLOAT64_IEEE", &ALIGNMENT_FLOAT64_IEEE::itemFactory);
    record_layout.factoryOptItem.insert("ALIGNMENT_INT64", &ALIGNMENT_INT64::itemFactory);
    record_layout.factoryOptItem.insert("STATIC_RECORD_LAYOUT", &STATIC_RECORD_LAYOUT::itemFactory);
    record_layout.factoryOptItem.insert("RESERVED", &RESERVED::itemFactory);
}

void A2lGrammar::initCompu_method()
{
    compu_method.typePar << Identifier << String << ConversionType << StringFormat << String;
    compu_method.namePar << "Name" << "LongIdentifier" << "ConversionType" << "Format" << "Unit";

    compu_method.factoryOptNode.insert("FORMULA", &FORMULA::nodeFactory);

    compu_method.factoryOptItem.insert("COEFFS", &COEFFS::itemFactory);
    compu_method.factoryOptItem.insert("COEFFS_LINEAR", &COEFFS_LINEAR::itemFactory);
    compu_method.factoryOptItem.insert("COMPU_TAB_REF", &COMPU_TAB_REF::itemFactory);
    compu_method.factoryOptItem.insert("REF_UNIT", &REF_UNIT::itemFactory);
    compu_method.factoryOptItem.insert("STATUS_STRING_REF", &STATUS_STRING_REF::itemFactory);
}

void A2lGrammar::initMeasurement()
{

    measurement.typePar << Identifier << String << DataType << Identifier << Integer << Float << Float << Float;
    measurement.namePar << "Name" << "LongIdentifier" << "DataType" << "Conversion" << "Resolution" << "Accuracy"
            << "LowerLimit" << "UpperLimit";

    measurement.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
    measurement.factoryOptNode.insert("ANNOTATION", &ANNOTATION::nodeFactory);

    measurement.factoryOptItem.insert("FORMAT", &FORMAT::itemFactory);
    measurement.factoryOptItem.insert("ECU_ADDRESS", &ECU_ADDRESS::itemFactory);
    measurement.factoryOptItem.insert("ARRAY_SIZE", &ARRAY_SIZE::itemFactory);
    measurement.factoryOptItem.insert("BIT_MASK", &BIT_MASK::itemFactory);
    measurement.factoryOptItem.insert("BYTE_ORDER", &Byte_Order::itemFactory);
    measurement.factoryOptItem.insert("DISPLAY_IDENTIFIER", &DISPLAY_IDENTIFIER::itemFactory);
    measurement.factoryOptItem.insert("ECU_ADDRESS_EXTENSION", &ECU_ADDRESS_EXTENSION::itemFactory);
    measurement.factoryOptItem.insert("MATRIX_DIM", &MATRIX_DIM::itemFactory);
    measurement.factoryOptItem.insert("SYMBOL_LINK", &SYMBOL_LINK::itemFactory);
    measurement.factoryOptItem.insert("LAYOUT", &LAYOUT::itemFactory);
    measurement.factoryOptItem.insert("PHYS_UNIT", &PHYS_UNIT::itemFactory);
    measurement.factoryOptItem.insert("READ_WRITE", &READ_WRITE::itemFactory);
}

void A2lGrammar::initCalibration_handle()
{
    calibration_handle.typePar << Hex;
    calibration_handle.namePar << "Handle";

    calibration_handle.factoryOptItem.insert("CALIBRATION_HANDLE_TEXT", &CALIBRATION_HANDLE_TEXT::itemFactory);
}

void A2lGrammar::initCalibration_method()
{
    calibration_method.typePar << String << Hex;
    calibration_method.namePar << "Method" << "Version";
    calibration_method.factoryOptNode.insert("CALIBRATION_HANDLE", &CALIBRATION_HANDLE::nodeFactory);

}

void A2lGrammar::initCompuTab()
{
    compuTab.typePar << Identifier << String << ConversionType << Integer;
    compuTab.namePar << "Name" << "LongIdentifier" << "ConversionType" << "NumberValuePairs";

    compuTab.factoryOptItem.insert("DEFAULT_VALUE", &DEFAULT_VALUE::itemFactory);
    compuTab.factoryOptItem.insert("DEFAULT_VALUE_NUMERIC", &DEFAULT_VALUE_NUMERIC::itemFactory);

}

void A2lGrammar::initCompuVtab()
{
    compuVtab.typePar << Identifier << String << ConversionType << Integer;
    compuVtab.namePar << "Name" << "LongIdentifier" << "ConversionType" << "NumberValuePairs";

    compuVtab.factoryOptItem.insert("DEFAULT_VALUE", &DEFAULT_VALUE::itemFactory);

}

void A2lGrammar::initCompuVtabRange()
{
    compuVtabRange.typePar << Identifier << String  << Integer;
    compuVtabRange.namePar << "Name" << "LongIdentifier"  << "NumberValuePairs";

    compuVtabRange.factoryOptItem.insert("DEFAULT_VALUE", &DEFAULT_VALUE::itemFactory);

}

void A2lGrammar::initDef_characteristic()
{

}

void A2lGrammar::initSub_group()
{

}

void A2lGrammar::initRef_measurement()
{

}

void A2lGrammar::initFormula()
{
    formula.typePar << String;
    formula.namePar << "foo";

    formula.factoryOptItem.insert("FORMULA_INV", &FORMULA_INV::itemFactory);
}

void A2lGrammar::initFunction()
{
    function.typePar << Identifier << String;
    function.namePar << "Name" << "LongIdentifier";

    function.factoryOptNode.insert("DEF_CHARACTERISTIC", &DEF_CHARACTERISTIC::nodeFactory);
    function.factoryOptNode.insert("REF_CHARACTERISTIC", &REF_CHARACTERISTIC::nodeFactory);
    function.factoryOptNode.insert("LOC_MEASUREMENT", &LOC_MEASUREMENT::nodeFactory);
    function.factoryOptItem.insert("FUNCTION_VERSION", &FUNCTION_VERSION::itemFactory);
    function.factoryOptNode.insert("IN_MEASUREMENT", &IN_MEASUREMENT::nodeFactory);
    function.factoryOptNode.insert("OUT_MEASUREMENT", &OUT_MEASUREMENT::nodeFactory);
    function.factoryOptNode.insert("SUB_FUNCTION", &SUB_FUNCTION::nodeFactory);
}

void A2lGrammar::initFunction_list()
{

}

void A2lGrammar::initHeader()
{
    header.typePar  << String;
    header.namePar << "Comment";

    header.factoryOptItem.insert("PROJECT_NO", &PROJECT_NO::itemFactory);
    header.factoryOptItem.insert("VERSION", &VERSION::itemFactory);

}

void A2lGrammar::initProject()
{
    project.typePar << Identifier << String;
    project.namePar << "name" << "LongIdentifier";

    project.factoryOptNode.insert("HEADER", &HEADER::nodeFactory);
    project.factoryOptNode.insert("MODULE", &MODULE::nodeFactory);
}

void A2lGrammar::initModule()
{
    module.typePar << Identifier << String;
    module.namePar << "name" << "LongIdentifier";

    module.factoryOptNode.insert("MEASUREMENT", &MEASUREMENT::nodeFactory);
    module.factoryOptNode.insert("CHARACTERISTIC", &CHARACTERISTIC::nodeFactory);
    module.factoryOptNode.insert("AXIS_PTS", &AXIS_PTS::nodeFactory);
    module.factoryOptNode.insert("A2ML", &A2ML::nodeFactory);
    module.factoryOptNode.insert("MOD_PAR", &MOD_PAR::nodeFactory);
    module.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
    module.factoryOptNode.insert("FUNCTION", &FUNCTION::nodeFactory);
    module.factoryOptNode.insert("MOD_COMMON", &MOD_COMMON::nodeFactory);
    module.factoryOptNode.insert("COMPU_METHOD", &COMPU_METHOD::nodeFactory);
    module.factoryOptNode.insert("COMPU_TAB", &COMPU_TAB::nodeFactory);
    module.factoryOptNode.insert("COMPU_VTAB", &COMPU_VTAB::nodeFactory);
    module.factoryOptNode.insert("COMPU_VTAB_RANGE", &COMPU_VTAB_RANGE::nodeFactory);
    module.factoryOptNode.insert("RECORD_LAYOUT", &RECORD_LAYOUT::nodeFactory);
    module.factoryOptNode.insert("GROUP", &GGROUP::nodeFactory);
    module.factoryOptNode.insert("UNIT", &UNIT::nodeFactory);
    module.factoryOptNode.insert("VARIANT_CODING", &VARIANT_CODING::nodeFactory);

}

void A2lGrammar::initMemory_segment()
{

    memory_segment.typePar << Identifier << String << PrgType << MemoryType << MemAttribute << Hex << Hex
            << Integer << Integer << Integer << Integer << Integer;
    memory_segment.namePar << "Name" << "LongIdentifier" << "PrgType" << "Memorytype" << "Attribute" << "Address"
            << "Size" << "Offset1" << "Offset2" << "Offset3" << "Offset4" << "Offset5" ;

    memory_segment.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
}

void A2lGrammar::initMemory_layout()
{

    memory_layout.typePar << PrgType << Hex << Hex
            << Integer << Integer << Integer << Integer << Integer;
    memory_layout.namePar << "PrgType" << "Address"
            << "Size" << "Offset1" << "Offset2" << "Offset3" << "Offset4" << "Offset5" ;

    memory_layout.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
}

void A2lGrammar::initMod_common()
{
    mod_common.typePar << String;
    mod_common.namePar << "Comment";

    mod_common.factoryOptItem.insert("BYTE_ORDER", &Byte_Order::itemFactory);
    mod_common.factoryOptItem.insert("ALIGNMENT_BYTE", &ALIGNMENT_BYTE::itemFactory);
    mod_common.factoryOptItem.insert("ALIGNMENT_WORD", &ALIGNMENT_WORD::itemFactory);
    mod_common.factoryOptItem.insert("ALIGNMENT_LONG", &ALIGNMENT_LONG::itemFactory);
    mod_common.factoryOptItem.insert("S_REC_LAYOUT", &S_REC_LAYOUT::itemFactory);
    mod_common.factoryOptItem.insert("DEPOSIT", &DEPOSIT::itemFactory);
    mod_common.factoryOptItem.insert("DATA_SIZE", &DATA_SIZE::itemFactory);
    mod_common.factoryOptItem.insert("ALIGNMENT_FLOAT32_IEEE", &ALIGNMENT_FLOAT32_IEEE::itemFactory);
    mod_common.factoryOptItem.insert("ALIGNMENT_FLOAT64_IEEE", &ALIGNMENT_FLOAT64_IEEE::itemFactory);
    mod_common.factoryOptItem.insert("ALIGNMENT_INT64", &ALIGNMENT_INT64::itemFactory);

}

void A2lGrammar::initMod_par()
{
    mod_par.typePar << String;
    mod_par.namePar << "Comment";

    mod_par.factoryOptNode.insert("MEMORY_SEGMENT", &MEMORY_SEGMENT::nodeFactory);
    mod_par.factoryOptNode.insert("MEMORY_LAYOUT", &MEMORY_LAYOUT::nodeFactory);
    mod_par.factoryOptNode.insert("CALIBRATION_METHOD", &CALIBRATION_METHOD::nodeFactory);

    mod_par.factoryOptItem.insert("VERSION", &VERSION::itemFactory);
    mod_par.factoryOptItem.insert("ADDR_EPK", &ADDR_EPK::itemFactory);
    mod_par.factoryOptItem.insert("CUSTOMER", &CUSTOMER::itemFactory);
    mod_par.factoryOptItem.insert("CUSTOMER_NO", &CUSTOMER_NO::itemFactory);
    mod_par.factoryOptItem.insert("EPK", &EPK::itemFactory);
    mod_par.factoryOptItem.insert("USER", &USER::itemFactory);
    mod_par.factoryOptItem.insert("ECU", &ECU::itemFactory);
    mod_par.factoryOptItem.insert("CPU_TYPE", &CPU_TYPE::itemFactory);
    mod_par.factoryOptItem.insert("NO_OF_INTERFACES", &NO_OF_INTERFACES::itemFactory);
    mod_par.factoryOptItem.insert("PHONE_NO", &PHONE_NO::itemFactory);
    mod_par.factoryOptItem.insert("SUPPLIER", &SUPPLIER::itemFactory);
    mod_par.factoryOptItem.insert("SYSTEM_CONSTANT", &SYSTEM_CONSTANT::itemFactory);
}

void A2lGrammar::initGroup()
{
    group.typePar << Identifier << String;
    group.namePar << "GroupName" << "GroupLongIdentifier";

    group.factoryOptNode.insert("IF_DATA", &IF_DATA::nodeFactory);
    group.factoryOptNode.insert("ANNOTATION", &ANNOTATION::nodeFactory);
    group.factoryOptNode.insert("FUNCTION_LIST", &FUNCTION_LIST::nodeFactory);
    group.factoryOptNode.insert("REF_CHARACTERISTIC", &REF_CHARACTERISTIC::nodeFactory);
    group.factoryOptNode.insert("REF_MEASUREMENT", &REF_MEASUREMENT::nodeFactory);
    group.factoryOptNode.insert("SUB_GROUP", &SUB_GROUP::nodeFactory);

    group.factoryOptItem.insert("ROOT", &ROOT::itemFactory);

}

void A2lGrammar::initUnit()
{
    unit.typePar << Identifier << String << String << UnitType;
    unit.namePar << "Name" << "LongIdentifier" << "Display" << "Type";

    unit.factoryOptItem.insert("REF_UNIT", &REF_UNIT::itemFactory);
    unit.factoryOptItem.insert("SI_EXPONENTS", &SI_EXPONENTS::itemFactory);
    unit.factoryOptItem.insert("UNIT_CONVERSION", &UNIT_CONVERSION::itemFactory);

}

void A2lGrammar::initVariant_coding()
{

}

void A2lGrammar::initDaq()
{
    //daq.typePar << DaqType << Hex << Hex << Hex << OptimisationType << AddressExtType << IdentificationFieldType
    //             <<GranularityOdtEntrySize << Hex << OverloadIndication;
    daq.typePar << DaqType << Hex << Hex << Hex << OptimisationType << AddressExtType << IdentificationFieldType
                 <<GranularityOdtEntrySize << Hex << OverloadIndication;
    daq.namePar << "DaqType" << "MAX_DAQ" << "MAX_EVENT_CHANNEL" << "MIN_DAQ" << "OptimisationType" << "AddressExtType"
                 << "IdentificationField" << "GranularityOdtEntrySize" << "MAX_ODT_ENTRY_SIZE_DAQ" << "OverloadIndication"
                 ;
    daq.factoryOptNode.insert("DAQ_MEMORY_CONSUMPTION", &DAQ_MEMORY_CONSUMPTION::nodeFactory);
    daq.factoryOptNode.insert("EVENT", &EVENT::nodeFactory);
    daq.factoryOptNode.insert("DAQ_LIST", &DAQ_LIST::nodeFactory);
}

void A2lGrammar::initIf_data()
{
    if_data.typePar  << Identifier;
    if_data.namePar << "Identifier";

    if_data.factoryOptNode.insert("DAQ", &DAQ::nodeFactory);
    if_data.factoryOptNode.insert("XCP_ON_CAN", &XCP_ON_CAN::nodeFactory);
    if_data.factoryOptNode.insert("SEGMENT", &SEGMENT::nodeFactory);
}

void A2lGrammar::initEvent()
{
    event.typePar << String << String << Hex << Keyword << Hex
                  << Hex << Hex << Hex;
    event.namePar << "EventChannelName" << "ShortName" << "ChannelNumber"
                  <<"EventType" << "MAX_DAQ_LIST"<< "EventChannelTimeCycle"
                 <<"EventChannelTimeUnit" << "EventChannelPriority";
}

void A2lGrammar::initDaq_list()
{
    daq_list.typePar  << Hex;
    daq_list.namePar << "ListNum";

    daq_list.factoryOptItem.insert("DAQ_LIST_TYPE", &DAQ_LIST_TYPE::itemFactory);
    daq_list.factoryOptItem.insert("MAX_ODT", &MAX_ODT::itemFactory);
    daq_list.factoryOptItem.insert("MAX_ODT_ENTRIES", &MAX_ODT_ENTRIES::itemFactory);
    daq_list.factoryOptItem.insert("FIRST_PID", &FIRST_PID::itemFactory);
    daq_list.factoryOptItem.insert("EVENT_FIXED", &EVENT_FIXED::itemFactory);
}

void A2lGrammar::initXcp_on_can()
{
    xcp_on_can.typePar << Hex ;
    xcp_on_can.namePar << "Version";

    xcp_on_can.factoryOptNode.insert("DAQ", &DAQ::nodeFactory);
    xcp_on_can.factoryOptNode.insert("EVENT_CAN_ID_LIST", &EVENT_CAN_ID_LIST::nodeFactory);
    xcp_on_can.factoryOptNode.insert("DAQ_LIST_CAN_ID", &DAQ_LIST_CAN_ID::nodeFactory);

    xcp_on_can.factoryOptNode.insert("CAN_FD", &CAN_FD::nodeFactory);

    xcp_on_can.factoryOptItem.insert("CAN_ID_MASTER", &CAN_ID_MASTER::itemFactory);
    xcp_on_can.factoryOptItem.insert("CAN_ID_SLAVE", &CAN_ID_SLAVE::itemFactory);
    xcp_on_can.factoryOptItem.insert("BAUDRATE", &BAUDRATE::itemFactory);
    xcp_on_can.factoryOptItem.insert("SAMPLE_POINT", &SAMPLE_POINT::itemFactory);
    xcp_on_can.factoryOptItem.insert("SAMPLE_RATE", &SAMPLE_RATE::itemFactory);
    xcp_on_can.factoryOptItem.insert("BTL_CYCLES", &BTL_CYCLES::itemFactory);
    xcp_on_can.factoryOptItem.insert("SJW", &SJW::itemFactory);
    xcp_on_can.factoryOptItem.insert("SYNC_EDGE", &SYNC_EDGE::itemFactory);
    xcp_on_can.factoryOptItem.insert("MAX_BUS_LOAD", &MAX_BUS_LOAD::itemFactory);
    xcp_on_can.factoryOptItem.insert("TRANSPORT_LAYER_INSTANCE", &TRANSPORT_LAYER_INSTANCE::itemFactory);
}

void A2lGrammar::initDaq_memory_consumption()
{
    daq_memory_consumption.typePar << Hex << Hex << Hex << Hex << Hex << Hex;
    daq_memory_consumption.namePar << "DAQ_MEMORY_LIMIT" << "DAQ_SIZE" << "ODT_SIZE" << "ODT_ENTRY_SIZE"
                                   <<"ODT_DAQ_BUFFER_ELEMENT_SIZE" << "ODT_STIM_BUFFER_ELEMENT_SIZE";
}

void A2lGrammar::initEvent_can_id_list()
{
    event_can_id_list.typePar << Hex;
    event_can_id_list.namePar << "EventNumber";

    event_can_id_list.factoryOptItem.insert("FIXED", &FIXED::itemFactory);
}

void A2lGrammar::initDaq_list_can_id()
{
    daq_list_can_id.typePar << Hex;
    daq_list_can_id.namePar << "DaqList";

    daq_list_can_id.factoryOptItem.insert("FIXED", &FIXED::itemFactory);
}

void A2lGrammar::initSegment()
{
    segment.typePar << Hex << Hex << Hex << Hex << Hex;
    segment.namePar << "SEGMENT_INDEX" << "PAGE_NUMBER" << "ADDRESS_EXTENSION" << "COMPRESSION_METHOD" << "ENCRYPTION_METHOD";

    segment.factoryOptNode.insert("CHECKSUM", &CHECKSUM::nodeFactory);
    segment.factoryOptNode.insert("PAGE", &PAGE::nodeFactory);
}

void A2lGrammar::initChecksum()
{
    checksum.typePar << ChecksumType;
    checksum.namePar << "CHECKSUM_TYPE";

    checksum.factoryOptItem.insert("MAX_BLOCK_SIZE", &MAX_BLOCK_SIZE::itemFactory);
}

void A2lGrammar::initPage()
{
    page.typePar << Hex << EcuAccessType << XcpReadAccessType << XcpWriteAccessType;
    page.namePar << "PAGE_INDEX" << "ECU_ACCESS_TYPE" << "XCP_READ_ACCESS_TYPE" << "XCP_WRITE_ACCESS_TYPE";

    page.factoryOptItem.insert("INIT_SEGMENT", &INIT_SEGMENT::itemFactory);
}

void A2lGrammar::initCan_fd()
{
    //can_fd.typePar;
    //can_fd.namePar;

    can_fd.factoryOptItem.insert("MAX_DLC", &MAX_DLC::itemFactory);
    can_fd.factoryOptItem.insert("CAN_FD_DATA_TRANSFER_BAUDRATE", &CAN_FD_DATA_TRANSFER_BAUDRATE::itemFactory);
    can_fd.factoryOptItem.insert("SAMPLE_POINT", &SAMPLE_POINT::itemFactory);
    can_fd.factoryOptItem.insert("BTL_CYCLES", &BTL_CYCLES::itemFactory);
    can_fd.factoryOptItem.insert("SJW", &SJW::itemFactory);
    can_fd.factoryOptItem.insert("SYNC_EDGE", &SYNC_EDGE::itemFactory);
    can_fd.factoryOptItem.insert("SECONDARY_SAMPLE_POINT", &SECONDARY_SAMPLE_POINT::itemFactory);
    can_fd.factoryOptItem.insert("MAX_DLC_REQUIRED", &MAX_DLC_REQUIRED::itemFactory);
    can_fd.factoryOptItem.insert("TRANSCEIVER_DELAY_COMPENSATION", &TRANSCEIVER_DELAY_COMPENSATION::itemFactory);
}

void A2lGrammar::initAddr_epk()
{
    addr_epk.typePar  << Hex ;
    addr_epk.namePar << "Address";
}

void A2lGrammar::initAlignment_byte()
{
    alignment_byte.typePar  << Integer;
    alignment_byte.namePar << "AlignmentBorder";
}

void A2lGrammar::initAlignment_float32_ieee()
{
    alignment_float32_ieee.typePar  << Integer;
    alignment_float32_ieee.namePar << "AlignmentBorder";
}

void A2lGrammar::initAlignment_float64_ieee()
{
    alignment_float64_ieee.typePar  << Integer;
    alignment_float64_ieee.namePar << "AlignmentBorder";
}

void A2lGrammar::initAlignment_long()
{
    alignment_long.typePar  << Integer;
    alignment_long.namePar << "AlignmentBorder";
}

void A2lGrammar::initAlignment_word()
{
    alignment_word.typePar  << Integer;
    alignment_word.namePar << "AlignmentBorder";
}

void A2lGrammar::initAlignment_int64()
{
    alignment_int64.typePar  << Integer;
    alignment_int64.namePar << "AlignmentBorder";
}

void A2lGrammar::initAnnotation_label()
{
    annotation_label.typePar  << String;
    annotation_label.namePar << "Label";
}

void A2lGrammar::initAnnotation_origin()
{
    annotation_origin.typePar  << String;
    annotation_origin.namePar << "Origin";

}

void A2lGrammar::initArray_size()
{
    array_size.typePar  << Integer;
    array_size.namePar << "Number";
}

void A2lGrammar::initAxis_pts_ref()
{
    axis_pts_ref.typePar  << Identifier;
    axis_pts_ref.namePar << "AxisPoints";
}

void A2lGrammar::initAxis_pts_x()
{
    axis_pts_x.typePar  << Integer << DataType << Indexorder << Addrtype;
    axis_pts_x.namePar << "Position" << "Datatype" << "IndexIncr" << "Addessing";
}

void A2lGrammar::initAxis_pts_y()
{
    axis_pts_y.typePar  << Integer << DataType << Indexorder << Addrtype;
    axis_pts_y.namePar << "Position" << "Datatype" << "IndexIncr" << "Addessing";
}

void A2lGrammar::initBit_mask()
{
    bit_mask.typePar  << Hex;
    bit_mask.namePar << "Mask";
}

void A2lGrammar::initByte_order()
{
    byte_order.typePar  << Byteorder;
    byte_order.namePar << "ByteOrder";
}

void A2lGrammar::initCalibration_handle_text()
{
    calibration_handle_text.typePar  << String;
    calibration_handle_text.namePar << "Text";
}

void A2lGrammar::initCoeffs()
{
    coeffs.typePar  << Float << Float << Float << Float << Float << Float;
    coeffs.namePar << "float1" << "float2" << "float3" << "float4" << "float5" << "float6";
}

void A2lGrammar::initCoeffs_linear()
{
    coeffs_linear.typePar  << Float << Float;
    coeffs_linear.namePar << "float1" << "float2";
}

void A2lGrammar::initCompu_tab_ref()
{
    compu_tab_ref.typePar  << Identifier;
    compu_tab_ref.namePar << "ConversionTable";
}

void A2lGrammar::initCpu_type()
{
    cpu_type.typePar  << String;
    cpu_type.namePar << "CPU";
}

void A2lGrammar::initCustomer()
{
    customer.typePar  << String;
    customer.namePar << "Customer";
}

void A2lGrammar::initCustomer_number()
{
    customer_number.typePar  << String;
    customer_number.namePar << "Number";
}

void A2lGrammar::initData_size()
{
    data_size.typePar  << Integer;
    data_size.namePar << "Size";
}

void A2lGrammar::initDefault_value()
{
    default_value.typePar  << String;
    default_value.namePar << "display_string";
}


void A2lGrammar::initDefault_value_numeric()
{
    default_value_numeric.typePar  << Float;
    default_value_numeric.namePar << "display_float";
}

void A2lGrammar::initDeposit()
{
    deposit.typePar  << Mode;
    deposit.namePar << "Mode";
}

void A2lGrammar::initDisplay_identifier()
{
    display_identifier.typePar  << Identifier;
    display_identifier.namePar << "display_name";
}

void A2lGrammar::initEcu()
{
    ecu.typePar  << String;
    ecu.namePar << "ControlUnit";
}

void A2lGrammar::initEcu_address()
{
    ecu_address.typePar  << Hex;
    ecu_address.namePar << "Address";
}

void A2lGrammar::initEcu_address_extension()
{
    ecu_address_extension.typePar  << Hex;
    ecu_address_extension.namePar << "Extension";
}

void A2lGrammar::initEpk()
{
    epk.typePar  << String;
    epk.namePar << "Identifier";
}

void A2lGrammar::initExtended_limits()
{
    extended_limits.typePar  << Float << Float;
    extended_limits.namePar << "LowerLimit" << "UpperLimit";
}

void A2lGrammar::initMax_refresh()
{
    max_refresh.typePar  << Integer << Integer;
    max_refresh.namePar << "Scaling_unit" << "Rate";
}

void A2lGrammar::initMax_grad()
{
    max_grad.typePar << Float;
    max_refresh.namePar << "max_grad";
}

void A2lGrammar::initFix_axis_par()
{
    fix_axis_par.typePar  << Integer << Integer << Integer;
    fix_axis_par.namePar << "Offset" << "Shift" << "Numberapo";

}

void A2lGrammar::initFix_axis_par_dist()
{
    fix_axis_par_dist.typePar  << Integer << Integer << Integer;
    fix_axis_par_dist.namePar << "Offset" << "Distance" << "Numberapo";

}

void A2lGrammar::initFnc_values()
{
    fnc_values.typePar  << Integer << DataType << IndexMode << Addrtype;
    fnc_values.namePar << "Position" << "Datatype" << "IndexMode" << "Addesstype";
}

void A2lGrammar::initFunction_version()
{

    function_version.typePar  << String;
    function_version.namePar << "VersionIdentifier";
}

void A2lGrammar::initFormat()
{
    format.typePar  << StringFormat;
    format.namePar << "FormatString";
}

void A2lGrammar::initFormula_inv()
{
    formula_inv.typePar  << String;
    formula_inv.namePar << "goo";
}

void A2lGrammar::initMatrix_dim()
{
    matrix_dim.typePar  << Integer << Integer << Integer;
    matrix_dim.namePar << "xDim" << "yDim" << "zDim";
}

void A2lGrammar::initNo_axis_pts_x()
{
    no_axis_pts_x.typePar  << Integer << DataType;
    no_axis_pts_x.namePar << "Position" << "Datatype";
}

void A2lGrammar::initNo_axis_pts_y()
{
    no_axis_pts_y.typePar  << Integer << DataType;
    no_axis_pts_y.namePar << "Position" << "Datatype";
}

void A2lGrammar::initNumber()
{
    number.typePar  << Integer;
    number.namePar << "Number";
}

void A2lGrammar::initPhone_no()
{
    phone_no.typePar  << String;
    phone_no.namePar << "TelNum";
}

void A2lGrammar::initProject_no()
{
    project_no.typePar  << Identifier;
    project_no.namePar << "ProjectNumber";
}

void A2lGrammar::initS_rec_layout()
{
    s_rec_layout.typePar  << Identifier;
    s_rec_layout.namePar << "Name";
}

void A2lGrammar::initRead_only()
{

}

void A2lGrammar::initRoot()
{

}

void A2lGrammar::initRef_unit()
{
    ref_unit.typePar  << Identifier;
    ref_unit.namePar << "Unit";
}

void A2lGrammar::initStatus_string_ref()
{
    status_string_ref.typePar  << Identifier;
    status_string_ref.namePar << "ConversionTable";
}

void A2lGrammar::initSystem_constant()
{
    system_constant.typePar  << String << String;
    system_constant.namePar << "Name" << "Value";
}

void A2lGrammar::initSrc_addr_x()
{
    src_addr_x.typePar  << Integer << DataType;
    src_addr_x.namePar << "Position" << "Datatype";
}

void A2lGrammar::initSrc_addr_y()
{
    src_addr_y.typePar  << Integer << DataType;
    src_addr_y.namePar << "Position" << "Datatype";
}

void A2lGrammar::initStatic_record_layout()
{

}

void A2lGrammar::initUser()
{
    user.typePar  << String;
    user.namePar << "UserName";
}

void A2lGrammar::initVersion()
{
    version.typePar  << String;
    version.namePar << "VersionIdentifier";
}

void A2lGrammar::initSupplier()
{
    supplier.typePar  << String;
    supplier.namePar << "Supplier";
}

void A2lGrammar::initSymbol_link()
{
    symbol_link.typePar << String << Integer;
    symbol_link.namePar << "SymbolName" << "Offset";
}

void A2lGrammar::initDaq_list_type()
{
    daq_list_type.typePar  << Keyword;
    daq_list_type.namePar << "Daq_list_type";
}

void A2lGrammar::initMax_odt()
{
    max_odt.typePar << Hex;
    max_odt.namePar << "MAX_ODT";
}

void A2lGrammar::initMax_odt_entries()
{
    max_odt_entries.typePar << Hex;
    max_odt_entries.namePar << "MAX_ODT_ENTRIES";
}

void A2lGrammar::initFirst_pid()
{
    first_pid.typePar << Hex;
    first_pid.namePar << "FIRST_PID";
}

void A2lGrammar::initEvent_fixed()
{
    event_fixed.typePar << Hex;
    event_fixed.namePar << "EVENT_FIXED";
}

void A2lGrammar::initReserved()
{
    reserved.typePar << Integer << Datasize;
    reserved.namePar << "Position" << "DataSize";
}

void A2lGrammar::initCan_id_master()
{
    can_id_master.typePar << Hex;
    can_id_master.namePar << "CAN_ID_MASTER";
}

void A2lGrammar::initCan_id_slave()
{
    can_id_slave.typePar << Hex;
    can_id_slave.namePar << "CAN_ID_SLAVE";
}

void A2lGrammar::initBaudrate()
{
    baudrate.typePar << Hex;
    baudrate.namePar << "BAUDRATE";
}

void A2lGrammar::initSample_point()
{
    sample_point.typePar << Hex;
    sample_point.namePar << "SAMPLE_POINT";
}

void A2lGrammar::initSample_rate()
{
    sample_rate.typePar << SampleRateType;
    sample_rate.namePar << "SAMPLE_RATE";
}

void A2lGrammar::initBtl_cycles()
{
    btl_cycles.typePar << Hex;
    btl_cycles.namePar << "BTL_CYCLES";
}

void A2lGrammar::initSjw()
{
    sjw.typePar << Hex;
    sjw.namePar << "SJW";
}

void A2lGrammar::initSync_edge()
{
    sync_edge.typePar << SampleRateType;
    sync_edge.namePar << "SYNC_EDGE";
}

void A2lGrammar::initMax_bus_load()
{
    max_bus_load.typePar << Hex;
    max_bus_load.namePar << "MAX_BUS_LOAD";
}

void A2lGrammar::initTransport_layer_instance()
{
    transport_layer_instance.typePar << String;
    transport_layer_instance.namePar << "TRANSPORT_LAYER_INSTANCE";
}

void A2lGrammar::initFixed()
{
    fixed.typePar << Hex;
    fixed.namePar << "CAN_ID";
}

void A2lGrammar::initMax_blcok_size()
{
    max_block_size.typePar << Hex;
    max_block_size.namePar << "MAX_BLOCK_SIZE";
}

void A2lGrammar::initInit_segment()
{
    init_segment.typePar << Integer;
    init_segment.namePar << "INIT_SEGMENT";
}

void A2lGrammar::initNo_of_interfaces()
{
    no_of_interfaces.typePar << Integer;
    no_of_interfaces.namePar << "NoOfInterfaces";
}

void A2lGrammar::initLayout()
{
    layout.typePar << IndexMode;
    layout.namePar << "Layout";
}

void A2lGrammar::initRead_write()
{
    //read_write.typePar;
    //read_write.namePar;
}

void A2lGrammar::initPhys_unit()
{
    phys_unit.typePar << String;
    phys_unit.namePar << "PhysUnit";
}

void A2lGrammar::initMax_dlc()
{
    max_dlc.typePar << Hex;
    max_dlc.namePar << "MaxDlc";
}

void A2lGrammar::initCan_fd_data_transfer_baudrate()
{
    can_fd_data_transfer_baudrate.typePar << Hex;
    can_fd_data_transfer_baudrate.namePar <<"CANFD_Baudrate";
}

void A2lGrammar::initSecondary_sample_point()
{
    secondary_sample_point.typePar << Hex;
    secondary_sample_point.namePar <<"secondarySamplePoint";
}

void A2lGrammar::initTransceiver_delay_compensation()
{
    transceiver_delay_compensation.typePar << Identifier;
    transceiver_delay_compensation.namePar << "TransceiverDelayCompensation";
}

void A2lGrammar::initMax_dlc_required()
{
    //max_dlc_required.typePar;
    //max_dlc_required.namePar;
}

void A2lGrammar::initSi_exponents()
{
    si_exponents.typePar  << Integer  << Integer  << Integer  << Integer  << Integer  << Integer  << Integer;
    si_exponents.namePar << "Length" << "Mass" << "Time" << "ElectricCurrent" << "Temeprature" << "AmountOfSubstance" << "LuminousIntensity";
}

void A2lGrammar::initUnit_conversion()
{
    unit_conversion.typePar  << Float  << Float;
    unit_conversion.namePar << "Gradient" << "Offset";
}
