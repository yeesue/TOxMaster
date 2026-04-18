#ifndef PARAM_TABLE_MODEL_H
#define PARAM_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QVector>
#include <QTimer>
#include "common.h"

class PARAM;

/**
 * @brief 参数表格模型 - 高性能显示大量参数
 * 
 * 优化点：
 * 1. 继承QAbstractTableModel而非QStandardItemModel，减少内存开销
 * 2. 缓存参数值，避免频繁调用PARAM::getValue()
 * 3. 批量更新机制，只发射一次dataChanged信号
 * 4. 支持可选的值变化检测，避免无意义刷新
 */
class ParamTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColName = 0,
        ColValue,
        ColUnit,
        ColCount
    };

    explicit ParamTableModel(QObject *parent = nullptr);
    ~ParamTableModel();

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // 参数管理
    void addParam(PARAM *param, const PamInfo &info);
    void addParams(const QList<PARAM*> &params, const QList<PamInfo> &infos);
    void removeParam(int row);
    void removeParams(const QList<int> &rows);
    void clear();
    void moveParam(int from, int to);

    // 获取参数
    PARAM* getParam(int row) const;
    PamInfo getPamInfo(int row) const;
    QList<PARAM*> getAllParams() const;
    QList<PamInfo> getAllPamInfos() const;

    // 刷新控制
    void updateAllValues();           // 批量更新所有值（推荐）
    void updateValue(int row);        // 更新单行值
    void setAutoUpdate(bool enabled, int intervalMs = 100);
    bool isAutoUpdateEnabled() const;

    // 编辑模式
    void setEditable(bool editable);
    bool isEditable() const;

signals:
    void dataEdited(int row, double value);  // 用户编辑值时发射

private slots:
    void onAutoUpdateTimer();

private:
    struct ParamData {
        PARAM *param;
        PamInfo info;
        double cachedValue;    // 缓存值，用于变化检测
        bool valueChanged;     // 值是否变化
    };

    QVector<ParamData> m_data;
    QTimer *m_updateTimer;
    bool m_editable;
    bool m_detectChange;       // 是否检测值变化
};

#endif // PARAM_TABLE_MODEL_H
