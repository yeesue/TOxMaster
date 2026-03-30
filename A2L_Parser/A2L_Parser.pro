#-------------------------------------------------
#
# Project created by QtCreator 2019-05-30T15:35:23
#
#-------------------------------------------------

QT       += core gui opengl network xml xmlpatterns widgets concurrent printsupport axcontainer sql script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 启用更严格的编译警告
QMAKE_CXXFLAGS += -Wall -Wextra

TARGET = A2L_Parser
TEMPLATE = app

#TARGET = A2L_Parser_DLL
#TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += . "C:\Program Files (x86)\quex\quex-0.69.3" \
                ./ASAP2 \
                ./sqlite  \
                ./Quex \
                ./DataContainers \


    # --- debug config --- #
    CONFIG( debug, debug|release ) {
        #UI_DIR = debug/ui
        #MOC_DIR = debug/moc
        #OBJECTS_DIR = debug/obj
        #DEFINES += MY_DEBUG
        #LIBS += -lqwtd -lgomp  -lopengl32 -lglu32 -lgdi32  #-lmgl -lmgl-qt5 -lqscintilla2d -lqwtplot3dd
        #QMAKE_CXXFLAGS_DEBUG += -fopenmp
    }
    else {
    # --- release config --- #
        UI_DIR = release/ui
        MOC_DIR = release/moc
        OBJECTS_DIR = release/obj
        LIBS +=  -lgomp  -lopengl32 -lglu32 -lgdi32 #-lmgl -lmgl-qt5 -lqscintilla2 -lqwtplot3d  #-lqwt(报错，先去除lqwt)
        QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -DQUEX_OPTION_ASSERTS_DISABLED -DQUEX_OPTION_INFORMATIVE_BUFFER_OVERFLOW_MESSAGE
    }

SOURCES += main.cpp\
    ASAP2/Items/can_fd_data_transfer_baudrate.cpp \
    ASAP2/Items/init_segment.cpp \
    ASAP2/Items/layout.cpp \
    ASAP2/Items/max_block_size.cpp \
    ASAP2/Items/max_dlc.cpp \
    ASAP2/Items/max_dlc_required.cpp \
    ASAP2/Items/no_of_interfaces.cpp \
    ASAP2/Items/phys_unit.cpp \
    ASAP2/Items/read_write.cpp \
    ASAP2/Items/secondary_sample_point.cpp \
    ASAP2/Items/transceiver_delay_compensation.cpp \
    ASAP2/Nodes/can_fd.cpp \
    ASAP2/Nodes/checksum.cpp \
    ASAP2/Nodes/page.cpp \
    ASAP2/Nodes/segment.cpp \
    ASAP2/a2ml_lexer.cpp \
    ASAP2/a2ml_parser.cpp \
        mainwindow.cpp \
    workproject.cpp \
    ASAP2/Items/addr_epk.cpp \
    ASAP2/Items/alignment_byte.cpp \
    ASAP2/Items/alignment_float32_ieee.cpp \
    ASAP2/Items/alignment_float64_ieee.cpp \
    ASAP2/Items/alignment_int64.cpp \
    ASAP2/Items/alignment_long.cpp \
    ASAP2/Items/alignment_word.cpp \
    ASAP2/Items/annotation_label.cpp \
    ASAP2/Items/annotation_origin.cpp \
    ASAP2/Items/array_size.cpp \
    ASAP2/Items/axis_pts_ref.cpp \
    ASAP2/Items/axis_pts_x.cpp \
    ASAP2/Items/axis_pts_y.cpp \
    ASAP2/Items/bit_mask.cpp \
    ASAP2/Items/byte_order.cpp \
    ASAP2/Items/calibration_handle_text.cpp \
    ASAP2/Items/coeffs.cpp \
    ASAP2/Items/coeffs_linear.cpp \
    ASAP2/Items/compu_tab_ref.cpp \
    ASAP2/Items/cpu_type.cpp \
    ASAP2/Items/customer.cpp \
    ASAP2/Items/customer_no.cpp \
    ASAP2/Items/data_size.cpp \
    ASAP2/Items/default_value.cpp \
    ASAP2/Items/default_value_numeric.cpp \
    ASAP2/Items/deposit.cpp \
    ASAP2/Items/display_identifier.cpp \
    ASAP2/Items/ecu.cpp \
    ASAP2/Items/ecu_address.cpp \
    ASAP2/Items/ecu_address_extension.cpp \
    ASAP2/Items/epk.cpp \
    ASAP2/Items/extended_limits.cpp \
    ASAP2/Items/fix_axis_par.cpp \
    ASAP2/Items/fix_axis_par_dist.cpp \
    ASAP2/Items/fnc_values.cpp \
    ASAP2/Items/format.cpp \
    ASAP2/Items/formula_inv.cpp \
    ASAP2/Items/function_version.cpp \
    ASAP2/Items/matrix_dim.cpp \
    ASAP2/Items/max_grad.cpp \
    ASAP2/Items/max_refresh.cpp \
    ASAP2/Items/no_axis_pts_x.cpp \
    ASAP2/Items/no_axis_pts_y.cpp \
    ASAP2/Items/number.cpp \
    ASAP2/Items/phone_no.cpp \
    ASAP2/Items/project_no.cpp \
    ASAP2/Items/read_only.cpp \
    ASAP2/Items/ref_unit.cpp \
    ASAP2/Items/root.cpp \
    ASAP2/Items/s_rec_layout.cpp \
    ASAP2/Items/si_exponents.cpp \
    ASAP2/Items/src_addr_x.cpp \
    ASAP2/Items/src_addr_y.cpp \
    ASAP2/Items/static_record_layout.cpp \
    ASAP2/Items/status_string_ref.cpp \
    ASAP2/Items/supplier.cpp \
    ASAP2/Items/symbol_link.cpp \
    ASAP2/Items/system_constant.cpp \
    ASAP2/Items/unit_conversion.cpp \
    ASAP2/Items/user.cpp \
    ASAP2/Items/version.cpp \
    ASAP2/Nodes/a2lfile.cpp \
    ASAP2/Nodes/a2ml.cpp \
    ASAP2/Nodes/annotation.cpp \
    ASAP2/Nodes/annotation_text.cpp \
    ASAP2/Nodes/axis_descr.cpp \
    ASAP2/Nodes/axis_pts.cpp \
    ASAP2/Nodes/calibration_handle.cpp \
    ASAP2/Nodes/calibration_method.cpp \
    ASAP2/Nodes/characteristic.cpp \
    ASAP2/Nodes/compu_method.cpp \
    ASAP2/Nodes/compu_tab.cpp \
    ASAP2/Nodes/compu_vtab.cpp \
    ASAP2/Nodes/compu_vtab_range.cpp \
    ASAP2/Nodes/dbfile.cpp \
    ASAP2/Nodes/def_characteristic.cpp \
    ASAP2/Nodes/formula.cpp \
    ASAP2/Nodes/function.cpp \
    ASAP2/Nodes/function_list.cpp \
    ASAP2/Nodes/group.cpp \
    ASAP2/Nodes/header.cpp \
    ASAP2/Nodes/if_data.cpp \
    ASAP2/Nodes/in_measurement.cpp \
    ASAP2/Nodes/loc_measurement.cpp \
    ASAP2/Nodes/measurement.cpp \
    ASAP2/Nodes/memory_layout.cpp \
    ASAP2/Nodes/memory_segment.cpp \
    ASAP2/Nodes/mod_common.cpp \
    ASAP2/Nodes/mod_par.cpp \
    ASAP2/Nodes/module.cpp \
    ASAP2/Nodes/out_measurement.cpp \
    ASAP2/Nodes/project.cpp \
    ASAP2/Nodes/record_layout.cpp \
    ASAP2/Nodes/ref_characteristic.cpp \
    ASAP2/Nodes/ref_measurement.cpp \
    ASAP2/Nodes/sub_function.cpp \
    ASAP2/Nodes/sub_group.cpp \
    ASAP2/Nodes/unit.cpp \
    ASAP2/Nodes/variant_coding.cpp \
    ASAP2/a2lgrammar.cpp \
    ASAP2/lexer.cpp \
    DataContainers/data.cpp \
    DataContainers/datacontainer.cpp \
    DataContainers/hexfile.cpp \
    a2l.cpp \
    a2ltreemodel.cpp \
    treemodelcompleter.cpp \
    Quex/a2l_quex_lexer/a2l_quex_lexer.cpp \
    item.cpp \
    node.cpp \
    charmodel.cpp \
    measmodel.cpp \
    dialogchoosemodule.cpp \
    sptablemodel.cpp \
    ASAP2/Nodes/daq.cpp \
    ASAP2/Nodes/event.cpp \
    ASAP2/Nodes/daq_list.cpp \
    ASAP2/Items/daq_list_type.cpp \
    ASAP2/Items/max_odt.cpp \
    ASAP2/Items/max_odt_entries.cpp \
    ASAP2/Items/first_pid.cpp \
    ASAP2/Items/event_fixed.cpp \
    ASAP2/Nodes/xcp_on_can.cpp \
    ASAP2/Items/reserved.cpp \
    compumethodmodel.cpp \
    compuvtabmodel.cpp \
    axisptsmodel.cpp \
    ASAP2/Items/can_id_master.cpp \
    ASAP2/Items/can_id_slave.cpp \
    ASAP2/Items/baudrate.cpp \
    ASAP2/Items/sample_point.cpp \
    ASAP2/Items/sample_rate.cpp \
    ASAP2/Items/btl_cycles.cpp \
    ASAP2/Items/sjw.cpp \
    ASAP2/Items/max_bus_load.cpp \
    ASAP2/Items/transport_layer_instance.cpp \
    ASAP2/Items/sync_edge.cpp \
    ASAP2/Nodes/daq_memory_consumption.cpp \
    ASAP2/Nodes/event_can_id_list.cpp \
    ASAP2/Items/fixed.cpp \
    ASAP2/Nodes/daq_list_can_id.cpp \
    a2l_parser_dll.cpp

HEADERS  += mainwindow.h \
    ASAP2/Items/can_fd_data_transfer_baudrate.h \
    ASAP2/Items/init_segment.h \
    ASAP2/Items/layout.h \
    ASAP2/Items/max_block_size.h \
    ASAP2/Items/max_dlc.h \
    ASAP2/Items/max_dlc_required.h \
    ASAP2/Items/no_of_interfaces.h \
    ASAP2/Items/phys_unit.h \
    ASAP2/Items/read_write.h \
    ASAP2/Items/secondary_sample_point.h \
    ASAP2/Items/transceiver_delay_compensation.h \
    ASAP2/Nodes/can_fd.h \
    ASAP2/Nodes/checksum.h \
    ASAP2/Nodes/page.h \
    ASAP2/Nodes/segment.h \
    ASAP2/a2ml_lexer.h \
    ASAP2/a2ml_parser.h \
    workproject.h \
    ASAP2/Items/addr_epk.h \
    ASAP2/Items/alignment_byte.h \
    ASAP2/Items/alignment_float32_ieee.h \
    ASAP2/Items/alignment_float64_ieee.h \
    ASAP2/Items/alignment_int64.h \
    ASAP2/Items/alignment_long.h \
    ASAP2/Items/alignment_word.h \
    ASAP2/Items/annotation_label.h \
    ASAP2/Items/annotation_origin.h \
    ASAP2/Items/array_size.h \
    ASAP2/Items/axis_pts_ref.h \
    ASAP2/Items/axis_pts_x.h \
    ASAP2/Items/axis_pts_y.h \
    ASAP2/Items/bit_mask.h \
    ASAP2/Items/byte_order.h \
    ASAP2/Items/calibration_handle_text.h \
    ASAP2/Items/coeffs.h \
    ASAP2/Items/coeffs_linear.h \
    ASAP2/Items/compu_tab_ref.h \
    ASAP2/Items/cpu_type.h \
    ASAP2/Items/customer.h \
    ASAP2/Items/customer_no.h \
    ASAP2/Items/data_size.h \
    ASAP2/Items/default_value.h \
    ASAP2/Items/default_value_numeric.h \
    ASAP2/Items/deposit.h \
    ASAP2/Items/display_identifier.h \
    ASAP2/Items/ecu.h \
    ASAP2/Items/ecu_address.h \
    ASAP2/Items/ecu_address_extension.h \
    ASAP2/Items/epk.h \
    ASAP2/Items/extended_limits.h \
    ASAP2/Items/fix_axis_par.h \
    ASAP2/Items/fix_axis_par_dist.h \
    ASAP2/Items/fnc_values.h \
    ASAP2/Items/format.h \
    ASAP2/Items/formula_inv.h \
    ASAP2/Items/function_version.h \
    ASAP2/Items/matrix_dim.h \
    ASAP2/Items/max_grad.h \
    ASAP2/Items/max_refresh.h \
    ASAP2/Items/no_axis_pts_x.h \
    ASAP2/Items/no_axis_pts_y.h \
    ASAP2/Items/number.h \
    ASAP2/Items/phone_no.h \
    ASAP2/Items/project_no.h \
    ASAP2/Items/read_only.h \
    ASAP2/Items/ref_unit.h \
    ASAP2/Items/root.h \
    ASAP2/Items/s_rec_layout.h \
    ASAP2/Items/si_exponents.h \
    ASAP2/Items/src_addr_x.h \
    ASAP2/Items/src_addr_y.h \
    ASAP2/Items/static_record_layout.h \
    ASAP2/Items/status_string_ref.h \
    ASAP2/Items/supplier.h \
    ASAP2/Items/symbol_link.h \
    ASAP2/Items/system_constant.h \
    ASAP2/Items/unit_conversion.h \
    ASAP2/Items/user.h \
    ASAP2/Items/version.h \
    ASAP2/Nodes/a2lfile.h \
    ASAP2/Nodes/a2ml.h \
    ASAP2/Nodes/annotation.h \
    ASAP2/Nodes/annotation_text.h \
    ASAP2/Nodes/axis_descr.h \
    ASAP2/Nodes/axis_pts.h \
    ASAP2/Nodes/calibration_handle.h \
    ASAP2/Nodes/calibration_method.h \
    ASAP2/Nodes/characteristic.h \
    ASAP2/Nodes/compu_method.h \
    ASAP2/Nodes/compu_tab.h \
    ASAP2/Nodes/compu_vtab.h \
    ASAP2/Nodes/compu_vtab_range.h \
    ASAP2/Nodes/dbfile.h \
    ASAP2/Nodes/def_characteristic.h \
    ASAP2/Nodes/formula.h \
    ASAP2/Nodes/function.h \
    ASAP2/Nodes/function_list.h \
    ASAP2/Nodes/group.h \
    ASAP2/Nodes/header.h \
    ASAP2/Nodes/if_data.h \
    ASAP2/Nodes/in_measurement.h \
    ASAP2/Nodes/loc_measurement.h \
    ASAP2/Nodes/measurement.h \
    ASAP2/Nodes/memory_layout.h \
    ASAP2/Nodes/memory_segment.h \
    ASAP2/Nodes/mod_common.h \
    ASAP2/Nodes/mod_par.h \
    ASAP2/Nodes/module.h \
    ASAP2/Nodes/out_measurement.h \
    ASAP2/Nodes/project.h \
    ASAP2/Nodes/record_layout.h \
    ASAP2/Nodes/ref_characteristic.h \
    ASAP2/Nodes/ref_measurement.h \
    ASAP2/Nodes/sub_function.h \
    ASAP2/Nodes/sub_group.h \
    ASAP2/Nodes/unit.h \
    ASAP2/Nodes/variant_coding.h \
    ASAP2/a2lgrammar.h \
    ASAP2/basefactory.h \
    ASAP2/lexer.h \
    DataContainers/data.h \
    DataContainers/datacontainer.h \
    DataContainers/hexfile.h \
    a2l.h \
    a2ltreemodel.h \
    treemodelcompleter.h \
    item.h \
    node.h \
    charmodel.h \
    measmodel.h \
    dialogchoosemodule.h \
    sptablemodel.h \
    ASAP2/Nodes/daq.h \
    ASAP2/Nodes/event.h \
    ASAP2/Nodes/daq_list.h \
    ASAP2/Items/daq_list_type.h \
    ASAP2/Items/max_odt.h \
    ASAP2/Items/max_odt_entries.h \
    ASAP2/Items/first_pid.h \
    ASAP2/Items/event_fixed.h \
    ASAP2/Nodes/xcp_on_can.h \
    ASAP2/Items/reserved.h \
    compumethodmodel.h \
    compuvtabmodel.h \
    axisptsmodel.h \
    ASAP2/Items/can_id_master.h \
    ASAP2/Items/can_id_slave.h \
    ASAP2/Items/baudrate.h \
    ASAP2/Items/sample_point.h \
    ASAP2/Items/sample_rate.h \
    ASAP2/Items/btl_cycles.h \
    ASAP2/Items/sjw.h \
    ASAP2/Items/max_bus_load.h \
    ASAP2/Items/transport_layer_instance.h \
    ASAP2/Items/sync_edge.h \
    ASAP2/Nodes/daq_memory_consumption.h \
    ASAP2/Nodes/event_can_id_list.h \
    ASAP2/Items/fixed.h \
    ASAP2/Nodes/daq_list_can_id.h \
    a2l_parser_dll.h \
    a2l_parser_dll_global.h

FORMS    += mainwindow.ui \
    dialogchoosemodule.ui

DISTFILES += \
    icons/A2L.png \
    icons/Charac.png \
    icons/Hex.png \
    icons/Meas.png \
    icons/WP.png \
    Quex/a2l_quex_lexer/lib/analyzer/configuration/derived \
    Quex/toto.txt

RESOURCES += \
    icons.qrc

INCLUDEPATH += . "C:\Program Files (x86)\quex\quex-0.69.3" \
                ./ASAP2 \
                ./ASAP2/NODES \
                ./sqlite  \
                ./Quex \
                ./DataContainers \
                ./core \
                ../XCP_COMM/common

# Core performance modules
include(core/core.pri)
