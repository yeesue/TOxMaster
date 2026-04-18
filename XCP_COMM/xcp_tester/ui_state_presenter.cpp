#include "ui_state_presenter.h"

#include <QLabel>
#include <QString>
#include <QStyle>
#include <QVariant>

UiStatePresenter::UiStatePresenter(QObject *parent)
    : QObject(parent)
{
}

void UiStatePresenter::applyLedState(QLabel *label, int color, int size) const
{
    if (!label)
        return;

    label->setText("");

    const QString minWidth = QString("min-width: %1px;").arg(size);
    const QString minHeight = QString("min-height: %1px;").arg(size);
    const QString maxWidth = QString("max-width: %1px;").arg(size);
    const QString maxHeight = QString("max-height: %1px;").arg(size);
    const QString borderRadius = QString("border-radius: %1px;").arg(size / 8);
    const QString border = QString("border:0px solid black;");

    QString background = "background-color:";
    switch (color) {
    case 0:
        background += "rgb(190,190,190)";
        break;
    case 1:
        background += "rgb(255,0,0)";
        break;
    case 2:
        background += "rgb(0,255,0)";
        break;
    case 3:
        background += "rgb(255,255,0)";
        break;
    default:
        background += "rgb(190,190,190)";
        break;
    }

    label->setStyleSheet(minWidth + minHeight + maxWidth + maxHeight + borderRadius + border + background);
}

void UiStatePresenter::applyRecordIndicator(QLabel *label, const QString &text, const QString &stateProperty) const
{
    if (!label)
        return;

    label->setText(text);
    label->setProperty("ledState", QVariant(stateProperty));
    label->style()->unpolish(label);
    label->style()->polish(label);
}
