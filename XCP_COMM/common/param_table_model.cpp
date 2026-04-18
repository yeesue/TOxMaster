#include "param_table_model.h"
#include "param.h"
#include <QDateTime>

ParamTableModel::ParamTableModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_updateTimer(nullptr)
    , m_editable(false)
    , m_detectChange(true)
{
}

ParamTableModel::~ParamTableModel()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
        delete m_updateTimer;
    }
}

int ParamTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.count();
}

int ParamTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColCount;
}

QVariant ParamTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_data.count())
        return QVariant();

    const ParamData &pd = m_data[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
        case ColName:
            return pd.info.pamName;
        case ColValue:
            return QString::number(pd.cachedValue, 'f', pd.info.decimal);
        case ColUnit:
            return pd.info.unit.isEmpty() ? pd.param->Unit : pd.info.unit;
        }
    }

    if (role == Qt::TextAlignmentRole) {
        switch (index.column()) {
        case ColName:
            return Qt::AlignLeft | Qt::AlignVCenter;
        case ColValue:
        case ColUnit:
            return Qt::AlignCenter;
        }
    }

    // 存储PARAM指针供Delegate使用
    if (role == Qt::UserRole && index.column() == ColValue) {
        return QVariant::fromValue(pd.param);
    }

    // 存储PamInfo的decimal供Delegate使用
    if (role == Qt::UserRole + 1 && index.column() == ColValue) {
        return pd.info.decimal;
    }

    // 存储最小最大值供Delegate使用
    if (role == Qt::UserRole + 2 && index.column() == ColValue) {
        return pd.info.showMin;
    }
    if (role == Qt::UserRole + 3 && index.column() == ColValue) {
        return pd.info.showMax;
    }

    return QVariant();
}

QVariant ParamTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case ColName:
        return tr("参数");
    case ColValue:
        return tr("值");
    case ColUnit:
        return tr("单位");
    }
    return QVariant();
}

bool ParamTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_data.count())
        return false;

    if (index.column() == ColValue && role == Qt::EditRole) {
        bool ok = false;
        double newValue = value.toDouble(&ok);
        if (!ok)
            return false;

        ParamData &pd = m_data[index.row()];
        pd.cachedValue = newValue;
        pd.valueChanged = true;

        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        emit dataEdited(index.row(), newValue);
        return true;
    }

    return false;
}

Qt::ItemFlags ParamTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractTableModel::flags(index);

    if (index.column() == ColValue && m_editable) {
        f |= Qt::ItemIsEditable;
    }

    return f;
}

void ParamTableModel::addParam(PARAM *param, const PamInfo &info)
{
    if (!param)
        return;

    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    ParamData pd;
    pd.param = param;
    pd.info = info;
    pd.cachedValue = param->getValue();
    pd.valueChanged = false;
    m_data.append(pd);
    endInsertRows();
}

void ParamTableModel::addParams(const QList<PARAM*> &params, const QList<PamInfo> &infos)
{
    if (params.isEmpty())
        return;

    int count = qMin(params.count(), infos.count());
    int startRow = m_data.count();

    beginInsertRows(QModelIndex(), startRow, startRow + count - 1);

    for (int i = 0; i < count; i++) {
        PARAM *param = params[i];
        if (!param)
            continue;

        ParamData pd;
        pd.param = param;
        pd.info = infos[i];
        pd.cachedValue = param->getValue();
        pd.valueChanged = false;
        m_data.append(pd);
    }

    endInsertRows();
}

void ParamTableModel::removeParam(int row)
{
    if (row < 0 || row >= m_data.count())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    m_data.remove(row);
    endRemoveRows();
}

void ParamTableModel::removeParams(const QList<int> &rows)
{
    if (rows.isEmpty())
        return;

    // 从大到小排序，确保删除顺序正确
    QList<int> sortedRows = rows;
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

    for (int row : sortedRows) {
        if (row >= 0 && row < m_data.count()) {
            beginRemoveRows(QModelIndex(), row, row);
            m_data.remove(row);
            endRemoveRows();
        }
    }
}

void ParamTableModel::clear()
{
    if (m_data.isEmpty())
        return;

    beginResetModel();
    m_data.clear();
    endResetModel();
}

void ParamTableModel::moveParam(int from, int to)
{
    if (from < 0 || from >= m_data.count() || to < 0 || to >= m_data.count())
        return;

    if (from == to)
        return;

    // 使用beginMoveRows确保视图正确更新
    beginMoveRows(QModelIndex(), from, from, QModelIndex(), to > from ? to + 1 : to);
    m_data.move(from, to);
    endMoveRows();
}

PARAM* ParamTableModel::getParam(int row) const
{
    if (row < 0 || row >= m_data.count())
        return nullptr;
    return m_data[row].param;
}

PamInfo ParamTableModel::getPamInfo(int row) const
{
    if (row < 0 || row >= m_data.count())
        return PamInfo();
    return m_data[row].info;
}

QList<PARAM*> ParamTableModel::getAllParams() const
{
    QList<PARAM*> result;
    result.reserve(m_data.count());
    for (const ParamData &pd : m_data) {
        result.append(pd.param);
    }
    return result;
}

QList<PamInfo> ParamTableModel::getAllPamInfos() const
{
    QList<PamInfo> result;
    result.reserve(m_data.count());
    for (const ParamData &pd : m_data) {
        result.append(pd.info);
    }
    return result;
}

void ParamTableModel::updateAllValues()
{
    if (m_data.isEmpty())
        return;

    // 批量更新所有缓存值
    int firstChanged = -1;
    int lastChanged = -1;

    for (int i = 0; i < m_data.count(); i++) {
        ParamData &pd = m_data[i];
        double newValue = pd.param->getValue();

        if (m_detectChange) {
            // 检测值是否变化
            if (qAbs(newValue - pd.cachedValue) > 1e-9) {
                pd.cachedValue = newValue;
                pd.valueChanged = true;

                if (firstChanged < 0)
                    firstChanged = i;
                lastChanged = i;
            }
        } else {
            pd.cachedValue = newValue;
            if (firstChanged < 0)
                firstChanged = i;
            lastChanged = i;
        }
    }

    // 只发射一次dataChanged信号
    if (firstChanged >= 0) {
        QModelIndex topLeft = index(firstChanged, ColValue);
        QModelIndex bottomRight = index(lastChanged, ColValue);
        emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole});
    }
}

void ParamTableModel::updateValue(int row)
{
    if (row < 0 || row >= m_data.count())
        return;

    ParamData &pd = m_data[row];
    pd.cachedValue = pd.param->getValue();
    pd.valueChanged = true;

    QModelIndex idx = index(row, ColValue);
    emit dataChanged(idx, idx, {Qt::DisplayRole});
}

void ParamTableModel::setAutoUpdate(bool enabled, int intervalMs)
{
    if (enabled) {
        if (!m_updateTimer) {
            m_updateTimer = new QTimer(this);
            connect(m_updateTimer, &QTimer::timeout, this, &ParamTableModel::onAutoUpdateTimer);
        }
        m_updateTimer->start(intervalMs);
    } else {
        if (m_updateTimer) {
            m_updateTimer->stop();
        }
    }
}

bool ParamTableModel::isAutoUpdateEnabled() const
{
    return m_updateTimer && m_updateTimer->isActive();
}

void ParamTableModel::setEditable(bool editable)
{
    if (m_editable != editable) {
        m_editable = editable;
        // 通知视图刷新flags
        emit dataChanged(index(0, ColValue), index(m_data.count() - 1, ColValue), {Qt::EditRole});
    }
}

bool ParamTableModel::isEditable() const
{
    return m_editable;
}

void ParamTableModel::onAutoUpdateTimer()
{
    updateAllValues();
}
