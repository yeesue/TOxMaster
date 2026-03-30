/**
 * @file a2l_data_manager.cpp
 * @brief A2L data manager implementation
 */

#include "a2l_data_manager.h"
#include "a2l_varchar.h"
#include "a2l_varmeas.h"
#include "param.h"
#include <QMutexLocker>

/**
 * @brief Private implementation structure
 */
struct A2lDataManager::Impl
{
    // Primary channel data
    QList<A2L_VarMeas*> measPamList;
    QList<A2L_VarMeas*> daqMeasPamList;
    QList<A2L_VarChar*> charPamList;
    
    // Secondary channel data
    QList<A2L_VarMeas*> measPamList2nd;
    QList<A2L_VarMeas*> daqMeasPamList2nd;
    QList<A2L_VarChar*> charPamList2nd;
    
    // DBC signals
    QList<dbc_signal*> dbcSignalsRead;
    QList<dbc_signal*> dbcSignalsWrite;
    
    // Function related hashes
    QHash<QString, QList<A2L_VarMeas*>> funMeasListHash;
    QHash<QString, QList<A2L_VarChar*>> funCharListHash;
    QHash<QString, QList<PARAM*>> funPamListHash;
    QHash<QString, QList<PARAM*>> funWritePamListHash;
    QHash<QString, QList<PARAM*>> funMapPamListHash;
    QHash<QString, PARAM*> mapPamHash;
    QHash<QString, PARAM*> pamHash;
    
    // Lookup hashes
    QHash<QString, A2L_VarMeas*> measHash;
    QHash<QString, A2L_VarMeas*> measHash2nd;
    QHash<QString, A2L_VarChar*> charHash;
    QHash<QString, A2L_VarChar*> charHash2nd;
    QHash<QString, dbc_signal*> dbcSignalHash;
};

// ==================== Singleton Implementation ====================

A2lDataManager& A2lDataManager::instance()
{
    static A2lDataManager instance;
    return instance;
}

A2lDataManager::A2lDataManager()
    : m_impl(std::unique_ptr<Impl>(new Impl()))
{
}

A2lDataManager::~A2lDataManager()
{
    clearAll();
}

// ==================== Measurement Parameter Lists ====================

QList<A2L_VarMeas*>& A2lDataManager::measPamList()
{
    return m_impl->measPamList;
}

const QList<A2L_VarMeas*>& A2lDataManager::measPamList() const
{
    return m_impl->measPamList;
}

QList<A2L_VarMeas*>& A2lDataManager::daqMeasPamList()
{
    return m_impl->daqMeasPamList;
}

const QList<A2L_VarMeas*>& A2lDataManager::daqMeasPamList() const
{
    return m_impl->daqMeasPamList;
}

QList<A2L_VarMeas*>& A2lDataManager::measPamList2nd()
{
    return m_impl->measPamList2nd;
}

const QList<A2L_VarMeas*>& A2lDataManager::measPamList2nd() const
{
    return m_impl->measPamList2nd;
}

QList<A2L_VarMeas*>& A2lDataManager::daqMeasPamList2nd()
{
    return m_impl->daqMeasPamList2nd;
}

const QList<A2L_VarMeas*>& A2lDataManager::daqMeasPamList2nd() const
{
    return m_impl->daqMeasPamList2nd;
}

// ==================== Calibration Parameter Lists ====================

QList<A2L_VarChar*>& A2lDataManager::charPamList()
{
    return m_impl->charPamList;
}

const QList<A2L_VarChar*>& A2lDataManager::charPamList() const
{
    return m_impl->charPamList;
}

QList<A2L_VarChar*>& A2lDataManager::charPamList2nd()
{
    return m_impl->charPamList2nd;
}

const QList<A2L_VarChar*>& A2lDataManager::charPamList2nd() const
{
    return m_impl->charPamList2nd;
}

// ==================== Function Hashes ====================

QHash<QString, A2L_VarMeas*>& A2lDataManager::measHash()
{
    return m_impl->measHash;
}

const QHash<QString, A2L_VarMeas*>& A2lDataManager::measHash() const
{
    return m_impl->measHash;
}

QHash<QString, A2L_VarMeas*>& A2lDataManager::measHash2nd()
{
    return m_impl->measHash2nd;
}

const QHash<QString, A2L_VarMeas*>& A2lDataManager::measHash2nd() const
{
    return m_impl->measHash2nd;
}

QHash<QString, A2L_VarChar*>& A2lDataManager::charHash()
{
    return m_impl->charHash;
}

const QHash<QString, A2L_VarChar*>& A2lDataManager::charHash() const
{
    return m_impl->charHash;
}

QHash<QString, A2L_VarChar*>& A2lDataManager::charHash2nd()
{
    return m_impl->charHash2nd;
}

const QHash<QString, A2L_VarChar*>& A2lDataManager::charHash2nd() const
{
    return m_impl->charHash2nd;
}

QHash<QString, dbc_signal*>& A2lDataManager::dbcSignalHash()
{
    return m_impl->dbcSignalHash;
}

const QHash<QString, dbc_signal*>& A2lDataManager::dbcSignalHash() const
{
    return m_impl->dbcSignalHash;
}

// ==================== Data Operations ====================

void A2lDataManager::clearAll()
{
    clearPrimary();
    clearSecondary();
    clearFunctionHashes();
    m_impl->dbcSignalsRead.clear();
    m_impl->dbcSignalsWrite.clear();
    m_impl->dbcSignalHash.clear();
    emit dataChanged();
}

void A2lDataManager::clearPrimary()
{
    m_impl->measPamList.clear();
    m_impl->daqMeasPamList.clear();
    m_impl->charPamList.clear();
    m_impl->measHash.clear();
    m_impl->charHash.clear();
    emit primaryDataChanged();
}

void A2lDataManager::clearSecondary()
{
    m_impl->measPamList2nd.clear();
    m_impl->daqMeasPamList2nd.clear();
    m_impl->charPamList2nd.clear();
    m_impl->measHash2nd.clear();
    m_impl->charHash2nd.clear();
    emit secondaryDataChanged();
}

void A2lDataManager::clearFunctionHashes()
{
    m_impl->funMeasListHash.clear();
    m_impl->funCharListHash.clear();
    m_impl->funPamListHash.clear();
    m_impl->funWritePamListHash.clear();
    m_impl->funMapPamListHash.clear();
    m_impl->mapPamHash.clear();
    m_impl->pamHash.clear();
}

// ==================== Thread-Safe Operations ====================

A2L_VarMeas* A2lDataManager::findMeasByName(const QString& name)
{
    QMutexLocker locker(&m_mutex);
    
    // Search in primary channel
    for (A2L_VarMeas* meas : m_impl->measPamList) {
        if (meas && meas->Name == name) {
            return meas;
        }
    }
    
    // Search in DAQ channel
    for (A2L_VarMeas* meas : m_impl->daqMeasPamList) {
        if (meas && meas->Name == name) {
            return meas;
        }
    }
    
    // Search in secondary channel
    for (A2L_VarMeas* meas : m_impl->measPamList2nd) {
        if (meas && meas->Name == name) {
            return meas;
        }
    }
    
    return nullptr;
}

A2L_VarChar* A2lDataManager::findCharByName(const QString& name)
{
    QMutexLocker locker(&m_mutex);
    
    // Search in primary channel
    for (A2L_VarChar* ch : m_impl->charPamList) {
        if (ch && ch->Name == name) {
            return ch;
        }
    }
    
    // Search in secondary channel
    for (A2L_VarChar* ch : m_impl->charPamList2nd) {
        if (ch && ch->Name == name) {
            return ch;
        }
    }
    
    return nullptr;
}

void A2lDataManager::clearAllSafe()
{
    QMutexLocker locker(&m_mutex);
    clearAll();
}

void A2lDataManager::setMeasPamListSafe(const QList<A2L_VarMeas*>& list)
{
    QMutexLocker locker(&m_mutex);
    m_impl->measPamList = list;
}

void A2lDataManager::setCharPamListSafe(const QList<A2L_VarChar*>& list)
{
    QMutexLocker locker(&m_mutex);
    m_impl->charPamList = list;
}

void A2lDataManager::setDaqMeasPamListSafe(const QList<A2L_VarMeas*>& list)
{
    QMutexLocker locker(&m_mutex);
    m_impl->daqMeasPamList = list;
}

void A2lDataManager::setMeasPamList2ndSafe(const QList<A2L_VarMeas*>& list)
{
    QMutexLocker locker(&m_mutex);
    m_impl->measPamList2nd = list;
}

void A2lDataManager::setCharPamList2ndSafe(const QList<A2L_VarChar*>& list)
{
    QMutexLocker locker(&m_mutex);
    m_impl->charPamList2nd = list;
}

void A2lDataManager::setDaqMeasPamList2ndSafe(const QList<A2L_VarMeas*>& list)
{
    QMutexLocker locker(&m_mutex);
    m_impl->daqMeasPamList2nd = list;
}

void A2lDataManager::clearMeasPamListSafe()
{
    QMutexLocker locker(&m_mutex);
    m_impl->measPamList.clear();
}

void A2lDataManager::clearCharPamListSafe()
{
    QMutexLocker locker(&m_mutex);
    m_impl->charPamList.clear();
}

void A2lDataManager::clearDaqMeasPamListSafe()
{
    QMutexLocker locker(&m_mutex);
    m_impl->daqMeasPamList.clear();
}

// ==================== Function-Related Lists ====================

QHash<QString, QList<A2L_VarMeas*>>& A2lDataManager::funMeasListHash()
{
    return m_impl->funMeasListHash;
}

QHash<QString, QList<A2L_VarChar*>>& A2lDataManager::funCharListHash()
{
    return m_impl->funCharListHash;
}

QHash<QString, QList<PARAM*>>& A2lDataManager::funPamListHash()
{
    return m_impl->funPamListHash;
}

QHash<QString, QList<PARAM*>>& A2lDataManager::funWritePamListHash()
{
    return m_impl->funWritePamListHash;
}

QHash<QString, QList<PARAM*>>& A2lDataManager::funMapPamListHash()
{
    return m_impl->funMapPamListHash;
}

QHash<QString, PARAM*>& A2lDataManager::mapPamHash()
{
    return m_impl->mapPamHash;
}

QHash<QString, PARAM*>& A2lDataManager::pamHash()
{
    return m_impl->pamHash;
}

// ==================== DBC Signal Lists ====================

QList<dbc_signal*>& A2lDataManager::dbcSignalsRead()
{
    return m_impl->dbcSignalsRead;
}

QList<dbc_signal*>& A2lDataManager::dbcSignalsWrite()
{
    return m_impl->dbcSignalsWrite;
}
