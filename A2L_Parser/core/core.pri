# Core modules for A2L_Parser
# Performance optimization components

# Memory management
HEADERS += \
    $$PWD/memory/object_pool.h \
    $$PWD/memory/string_intern.h \
    $$PWD/memory/node_pool.h

# Concurrent processing
HEADERS += \
    $$PWD/concurrent/parallel_parser.h

# Include paths
INCLUDEPATH += $$PWD
