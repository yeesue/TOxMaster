/**
 * @file a2l_interface.cpp
 * @brief A2L module interface implementation
 */

#include "a2l_interface.h"
#include "a2l_varmeas.h"
#include "a2l_varchar.h"
#include "a2l_project.h"

namespace xcp {

/**
 * @brief A2L measurement variable adapter
 */
class A2LMeasVarAdapter : public IMeasVar
{
public:
    explicit A2LMeasVarAdapter(A2L_VarMeas* var)
        : m_var(var)
    {}
    
    ~A2LMeasVarAdapter() override = default;
    
    QString name() const override
    {
        return m_var->Name;
    }
    
    quint32 address() const override
    {
        return m_var->ECUAddress;
    }
    
    quint8 size() const override
    {
        return static_cast<quint8>(m_var->DataSizeAG);
    }
    
    float physicalValue(float rawValue) const override
    {
        return rawValue * m_var->ScalingFactor + m_var->ScalingOffset;
    }
    
    float rawValue(float physicalValue) const override
    {
        return (physicalValue - m_var->ScalingOffset) / m_var->ScalingFactor;
    }
    
private:
    A2L_VarMeas* m_var;
};

/**
 * @brief A2L calibration variable adapter
 */
class A2LCalVarAdapter : public ICalVar
{
public:
    explicit A2LCalVarAdapter(A2L_VarChar* var)
        : m_var(var)
    {}
    
    ~A2LCalVarAdapter() override = default;
    
    QString name() const override
    {
        return m_var->Name;
    }
    
    quint32 address() const override
    {
        return m_var->ECUAddress;
    }
    
    quint8 size() const override
    {
        return static_cast<quint8>(m_var->DataSizeAG);
    }
    
    float physicalValue(float rawValue) const override
    {
        return rawValue * m_var->ScalingFactor + m_var->ScalingOffset;
    }
    
    float rawValue(float physicalValue) const override
    {
        return (physicalValue - m_var->ScalingOffset) / m_var->ScalingFactor;
    }
    
    bool isArray() const override
    {
        return m_var->charType == "CURVE" || m_var->charType == "MAP";
    }
    
    int arraySize() const override
    {
        if (m_var->charType == "CURVE") {
            return m_var->zCount;
        } else if (m_var->charType == "MAP") {
            return m_var->zCount * m_var->yCount;
        }
        return 1;
    }
    
private:
    A2L_VarChar* m_var;
};

/**
 * @brief A2L DAQ list adapter
 */
class A2LDaqListAdapter : public IDaqList
{
public:
    explicit A2LDaqListAdapter(A2L_Daq_List* daqList)
        : m_daqList(daqList)
    {}
    
    ~A2LDaqListAdapter() override = default;
    
    quint16 id() const override
    {
        return m_daqList->DaqListNumber;
    }
    
    float rate() const override
    {
        // A2L_Daq_List没有直接的rate()方法，这里返回0
        return 0.0f;
    }
    
    QList<IMeasVar*> variables() const override
    {
        // A2L_Daq_List没有直接的variables()方法，这里返回空列表
        return QList<IMeasVar*>();
    }
    
private:
    A2L_Daq_List* m_daqList;
};

/**
 * @brief A2L event adapter
 */
class A2LEventAdapter : public IEvent
{
public:
    explicit A2LEventAdapter(A2L_Event* event, quint32 canId)
        : m_event(event)
        , m_canId(canId)
    {}
    
    ~A2LEventAdapter() override = default;
    
    quint8 id() const override
    {
        return m_event->ChannelNumber;
    }
    
    float rate() const override
    {
        return m_event->Rate_ms;
    }
    
    quint32 canId() const override
    {
        return m_canId;
    }
    
private:
    A2L_Event* m_event;
    quint32 m_canId;
};

/**
 * @brief A2L memory segment adapter
 */
class A2LMemorySegmentAdapter : public IMemorySegment
{
public:
    explicit A2LMemorySegmentAdapter(A2L_MemorySegment* segment)
        : m_segment(segment)
    {}
    
    ~A2LMemorySegmentAdapter() override = default;
    
    QString name() const override
    {
        return m_segment->memName;
    }
    
    quint32 startAddress() const override
    {
        return m_segment->memAddress;
    }
    
    quint32 size() const override
    {
        return m_segment->memSize;
    }
    
    quint8 page() const override
    {
        return m_segment->segIndex;
    }
    
private:
    A2L_MemorySegment* m_segment;
};

/**
 * @brief A2L data provider implementation
 */
class A2LDataProviderImpl : public IA2LDataProvider
{
public:
    explicit A2LDataProviderImpl(A2L_Project* project)
        : m_project(project)
    {}
    
    ~A2LDataProviderImpl() override
    {
        // Clean up adapters
        qDeleteAll(m_measVarAdapters);
        qDeleteAll(m_calVarAdapters);
        qDeleteAll(m_daqListAdapters);
        qDeleteAll(m_eventAdapters);
        qDeleteAll(m_memorySegmentAdapters);
    }
    
    QList<IMeasVar*> getAllMeasVars() const override
    {
        QList<IMeasVar*> result;
        if (m_project) {
            for (A2L_VarMeas* var : m_project->getSelectedMeasVarList()) {
                result.append(new A2LMeasVarAdapter(var));
            }
        }
        return result;
    }
    
    IMeasVar* findMeasVarByName(const QString& name) const override
    {
        if (m_project) {
            A2L_VarMeas* var = m_project->getMeasVarByName(name);
            if (var) {
                return new A2LMeasVarAdapter(var);
            }
        }
        return nullptr;
    }
    
    QList<ICalVar*> getAllCalVars() const override
    {
        QList<ICalVar*> result;
        // A2L_Project没有直接的getVarCharList()方法，这里返回空列表
        return result;
    }
    
    ICalVar* findCalVarByName(const QString& name) const override
    {
        if (m_project) {
            A2L_VarChar* var = m_project->getCharVarByName(name);
            if (var) {
                return new A2LCalVarAdapter(var);
            }
        }
        return nullptr;
    }
    
    QList<IDaqList*> getAllDaqLists() const override
    {
        QList<IDaqList*> result;
        if (m_project) {
            for (A2L_Daq_List* daqList : m_project->getDaqListStructList()) {
                result.append(new A2LDaqListAdapter(daqList));
            }
        }
        return result;
    }
    
    QList<IEvent*> getAllEvents() const override
    {
        QList<IEvent*> result;
        if (m_project) {
            for (A2L_Event* event : m_project->getEventList()) {
                // Find corresponding CAN ID
                quint32 canId = 0;
                QList<A2L_EventCanId*> eventCanIdList = m_project->getA2lEventCanIdList();
                for (A2L_EventCanId* eventCanId : eventCanIdList) {
                    if (eventCanId->EventNumber == event->ChannelNumber) {
                        canId = eventCanId->Event_CAN_ID;
                        break;
                    }
                }
                result.append(new A2LEventAdapter(event, canId));
            }
        }
        return result;
    }
    
    QList<IMemorySegment*> getAllMemorySegments() const override
    {
        QList<IMemorySegment*> result;
        if (m_project) {
            for (A2L_MemorySegment* segment : m_project->getA2lMemSegList()) {
                result.append(new A2LMemorySegmentAdapter(segment));
            }
        }
        return result;
    }
    
    IMemorySegment* findMemorySegmentByName(const QString& name) const override
    {
        if (m_project) {
            QList<A2L_MemorySegment*> segments = m_project->getA2lMemSegList();
            for (A2L_MemorySegment* segment : segments) {
                if (segment->memName == name) {
                    return new A2LMemorySegmentAdapter(segment);
                }
            }
        }
        return nullptr;
    }
    
private:
    A2L_Project* m_project;
    mutable QList<IMeasVar*> m_measVarAdapters;
    mutable QList<ICalVar*> m_calVarAdapters;
    mutable QList<IDaqList*> m_daqListAdapters;
    mutable QList<IEvent*> m_eventAdapters;
    mutable QList<IMemorySegment*> m_memorySegmentAdapters;
};

/**
 * @brief A2L data provider factory implementation
 */
IA2LDataProvider* A2LDataProviderFactory::createFromA2LFile(const QString& filePath)
{
    // TODO: Implement A2L file parsing
    // For now, return null
    return nullptr;
}

IA2LDataProvider* A2LDataProviderFactory::createFromExistingData(void* a2lProject)
{
    if (a2lProject) {
        A2L_Project* project = static_cast<A2L_Project*>(a2lProject);
        return new A2LDataProviderImpl(project);
    }
    return nullptr;
}

} // namespace xcp