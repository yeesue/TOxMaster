#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QWidget>
#include <QSpinBox>

class HexSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    HexSpinBox(QWidget *parent = 0);
    ~HexSpinBox();

    void setMaximum(quint32 maxValue);

protected:
    QValidator::State validate(QString &text, int &pos) const;
    int valueFromText(const QString &text) const;
    QString textFromValue(int val) const;

private:
    QRegExpValidator *validator;
};

#endif // HEXSPINBOX_H
