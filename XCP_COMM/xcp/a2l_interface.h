/**
 * @file a2l_interface.h
 * @brief A2L module interface for XCP
 * 
 * Abstract interface to decouple XCP module from A2L module
 */

#ifndef A2L_INTERFACE_H
#define A2L_INTERFACE_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QSharedPointer>

namespace xcp {

/**
 * @brief Abstract measurement variable interface
 */
class IMeasVar
{
public:
    virtual ~IMeasVar() = default;
    
    virtual QString name() const = 0;
    virtual quint32 address() const = 0;
    virtual quint8 size() const = 0;
    virtual float physicalValue(float rawValue) const = 0;
    virtual float rawValue(float physicalValue) const = 0;
};

/**
 * @brief Abstract calibration variable interface
 */
class ICalVar
{
public:
    virtual ~ICalVar() = default;
    
    virtual QString name() const = 0;
    virtual quint32 address() const = 0;
    virtual quint8 size() const = 0;
    virtual float physicalValue(float rawValue) const = 0;
    virtual float rawValue(float physicalValue) const = 0;
    virtual bool isArray() const = 0;
    virtual int arraySize() const = 0;
};

/**
 * @brief Abstract DAQ list interface
 */
class IDaqList
{
public:
    virtual ~IDaqList() = default;
    
    virtual quint16 id() const = 0;
    virtual float rate() const = 0;
    virtual QList<IMeasVar*> variables() const = 0;
};

/**
 * @brief Abstract event interface
 */
class IEvent
{
public:
    virtual ~IEvent() = default;
    
    virtual quint8 id() const = 0;
    virtual float rate() const = 0;
    virtual quint32 canId() const = 0;
};

/**
 * @brief Abstract memory segment interface
 */
class IMemorySegment
{
public:
    virtual ~IMemorySegment() = default;
    
    virtual QString name() const = 0;
    virtual quint32 startAddress() const = 0;
    virtual quint32 size() const = 0;
    virtual quint8 page() const = 0;
};

/**
 * @brief Abstract A2L data provider interface
 */
class IA2LDataProvider
{
public:
    virtual ~IA2LDataProvider() = default;
    
    // Measurement variables
    virtual QList<IMeasVar*> getAllMeasVars() const = 0;
    virtual IMeasVar* findMeasVarByName(const QString& name) const = 0;
    
    // Calibration variables
    virtual QList<ICalVar*> getAllCalVars() const = 0;
    virtual ICalVar* findCalVarByName(const QString& name) const = 0;
    
    // DAQ configuration
    virtual QList<IDaqList*> getAllDaqLists() const = 0;
    virtual QList<IEvent*> getAllEvents() const = 0;
    
    // Memory segments
    virtual QList<IMemorySegment*> getAllMemorySegments() const = 0;
    virtual IMemorySegment* findMemorySegmentByName(const QString& name) const = 0;
};

/**
 * @brief A2L data provider factory
 */
class A2LDataProviderFactory
{
public:
    static IA2LDataProvider* createFromA2LFile(const QString& filePath);
    static IA2LDataProvider* createFromExistingData(void* a2lProject);
};

} // namespace xcp

#endif // A2L_INTERFACE_H