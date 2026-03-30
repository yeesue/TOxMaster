/**
 * @file a2l_data_manager.h
 * @brief A2L data manager - replaces global variables
 */

#ifndef A2L_DATA_MANAGER_H
#define A2L_DATA_MANAGER_H

#include <QObject>
#include <QList>
#include <QHash>
#include <QMutex>
#include <memory>

// Forward declarations
class A2L_VarChar;
class A2L_VarMeas;
struct dbc_signal;
struct PARAM;

/**
 * @brief A2L data manager
 * 
 * Singleton pattern, centrally manages all A2L related global data
 */
class A2lDataManager : public QObject
{
    Q_OBJECT

public:
    static A2lDataManager& instance();

    // ==================== Measurement parameter lists ====================
    
    QList<A2L_VarMeas*>& measPamList();
    const QList<A2L_VarMeas*>& measPamList() const;
    
    QList<A2L_VarMeas*>& daqMeasPamList();
    const QList<A2L_VarMeas*>& daqMeasPamList() const;
    
    QList<A2L_VarMeas*>& measPamList2nd();
    const QList<A2L_VarMeas*>& measPamList2nd() const;
    
    QList<A2L_VarMeas*>& daqMeasPamList2nd();
    const QList<A2L_VarMeas*>& daqMeasPamList2nd() const;

    // ==================== Calibration parameter lists ====================
    
    QList<A2L_VarChar*>& charPamList();
    const QList<A2L_VarChar*>& charPamList() const;
    
    QList<A2L_VarChar*>& charPamList2nd();
    const QList<A2L_VarChar*>& charPamList2nd() const;

    // ==================== Function hashes ====================
    
    QHash<QString, A2L_VarMeas*>& measHash();
    const QHash<QString, A2L_VarMeas*>& measHash() const;
    
    QHash<QString, A2L_VarMeas*>& measHash2nd();
    const QHash<QString, A2L_VarMeas*>& measHash2nd() const;
    
    QHash<QString, A2L_VarChar*>& charHash();
    const QHash<QString, A2L_VarChar*>& charHash() const;
    
    QHash<QString, A2L_VarChar*>& charHash2nd();
    const QHash<QString, A2L_VarChar*>& charHash2nd() const;
    
    QHash<QString, dbc_signal*>& dbcSignalHash();
    const QHash<QString, dbc_signal*>& dbcSignalHash() const;
    
    // ==================== Function-related lists (for globals compatibility) ====================
    
    QHash<QString, QList<A2L_VarMeas*>>& funMeasListHash();
    QHash<QString, QList<A2L_VarChar*>>& funCharListHash();
    QHash<QString, QList<PARAM*>>& funPamListHash();
    QHash<QString, QList<PARAM*>>& funWritePamListHash();
    QHash<QString, QList<PARAM*>>& funMapPamListHash();
    QHash<QString, PARAM*>& mapPamHash();
    QHash<QString, PARAM*>& pamHash();
    
    // ==================== DBC signal lists ====================
    
    QList<dbc_signal*>& dbcSignalsRead();
    QList<dbc_signal*>& dbcSignalsWrite();

    // ==================== Data operations ====================
    
    void clearAll();
    void clearPrimary();
    void clearSecondary();
    void clearFunctionHashes();

    // ==================== Thread-safe operations ====================
    
    QMutex& mutex() { return m_mutex; }
    
    A2L_VarMeas* findMeasByName(const QString& name);
    A2L_VarChar* findCharByName(const QString& name);
    
    void clearAllSafe();
    void setMeasPamListSafe(const QList<A2L_VarMeas*>& list);
    void setCharPamListSafe(const QList<A2L_VarChar*>& list);
    void setDaqMeasPamListSafe(const QList<A2L_VarMeas*>& list);
    void setMeasPamList2ndSafe(const QList<A2L_VarMeas*>& list);
    void setCharPamList2ndSafe(const QList<A2L_VarChar*>& list);
    void setDaqMeasPamList2ndSafe(const QList<A2L_VarMeas*>& list);
    void clearMeasPamListSafe();
    void clearCharPamListSafe();
    void clearDaqMeasPamListSafe();

signals:
    void dataChanged();
    void primaryDataChanged();
    void secondaryDataChanged();

private:
    A2lDataManager();
    ~A2lDataManager();
    A2lDataManager(const A2lDataManager&) = delete;
    A2lDataManager& operator=(const A2lDataManager&) = delete;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    mutable QMutex m_mutex;
};

#endif // A2L_DATA_MANAGER_H
