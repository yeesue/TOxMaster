#include "pampropdlg.h"
#include "ui_pampropdlg.h"
#include <QMessageBox>
#include <QCheckBox>

PamPropDlg::PamPropDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PamPropDlg)
{
    ui->setupUi(this);
    model = new QStandardItemModel();

}

PamPropDlg::~PamPropDlg()
{
    delete ui;
}

void PamPropDlg::setPams(QList<PARAM *> pams)
{
    this->pamList = pams;

    model->clear();
    ui->tableWidget->clear();

    if(pamList.isEmpty())
        return;

    ui->tableWidget->setRowCount(pamList.count());
    ui->tableWidget->setColumnCount(5);


    model->setRowCount(pamList.count());
    model->setColumnCount(5);

    for(int row = 0; row < pamList.count(); row++)
    {
        PARAM *pam = pamList.at(row);
        PAMChart pamAttr = pamAttrList.at(row);

        QTableWidgetItem *nameItem = new QTableWidgetItem();
        nameItem->setText(pam->Name);

        ui->tableWidget->setItem(row, 0, nameItem);

        QCheckBox *checkBox = new QCheckBox(this);
        checkBox->setChecked(pamAttrList.at(row).visible);
        QWidget *w = new QWidget(this);
        QHBoxLayout *layout = new QHBoxLayout();
        layout->addWidget(checkBox);
        layout->setMargin(0);
        layout->setAlignment(checkBox, Qt::AlignCenter);
        w->setLayout(layout);

        ui->tableWidget->setCellWidget(row, 1, w);

        QTableWidgetItem *minYItem = new QTableWidgetItem();
        minYItem->setText(QString::number(pamAttr.minYAxis));

        QTableWidgetItem *maxYItem = new QTableWidgetItem();
        maxYItem->setText(QString::number(pamAttr.maxYAxis));

        ui->tableWidget->setItem(row, 2, minYItem);
        ui->tableWidget->setItem(row, 3, maxYItem);

        QCheckBox *cb_autoScale = new QCheckBox(this);
        cb_autoScale->setChecked(pamAttrList.at(row).autoScale);
        QWidget *w_autoScale = new QWidget(this);
        QHBoxLayout *layout_AutoScale = new QHBoxLayout();
        layout_AutoScale->addWidget(cb_autoScale);
        layout_AutoScale->setMargin(0);
        layout_AutoScale->setAlignment(cb_autoScale, Qt::AlignCenter);
        w_autoScale->setLayout(layout_AutoScale);

        ui->tableWidget->setCellWidget(row, 4, w_autoScale);
    }
}

void PamPropDlg::accept()
{
    saveChartProp();
    savePamAttr();

    emit chartPropUpdated(chartProp);
    //emit pamsRemoved(removePams);
    emit pamEditFinished(this->pamList);
    emit pamsAttrUpdated(pamAttrList);

    QDialog::accept();
}

void PamPropDlg::on_pb_Remove_clicked()
{
    //int curRow = ui->tableView->currentIndex().row();
    int curRow = ui->tableWidget->currentRow();

    int result = QMessageBox::warning(this,
                         tr("Pam delete"),
                         tr("Confirm to delete? \n"
                            ) + pamList.at(curRow)->Name,
                         QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::NoButton
                         );
    if (result == QMessageBox::Cancel)
    {

    }
    else
    {
        model->removeRow(curRow);
        ui->tableWidget->removeRow(curRow);

        this->removePams.append(pamList.at(curRow));
        this->pamList.removeAt(curRow);
        this->pamAttrList.removeAt(curRow);
    }
}

void PamPropDlg::saveChartProp()
{
    int timeGapValue = ui->sb_timegap->value();
    if(timeGapValue < 3)
    {
        timeGapValue = 3;
    }

    chartProp.timeGap = ui->sb_timegap->value();
    chartProp.style = ui->cb_style->currentIndex();
}

void PamPropDlg::savePamAttr()
{
    for(int i = 0; i < pamAttrList.count(); i++)
    {
        PAMChart pamChart = pamAttrList.at(i);

        QWidget *w = ui->tableWidget->cellWidget(i, 1);
        QLayoutItem *item = w->layout()->itemAt(0);
        QCheckBox *checkBox = (QCheckBox*)item->widget();
        if(checkBox)
            pamChart.visible = checkBox->isChecked();

        qreal minYAxis = ui->tableWidget->item(i, 2)->text().toDouble();
        qreal maxYAxis = ui->tableWidget->item(i, 3)->text().toDouble();

        QWidget *w_autoScale = ui->tableWidget->cellWidget(i, 4);
        QLayoutItem *item_autoScale = w_autoScale->layout()->itemAt(0);
        QCheckBox *cb_autoScale = (QCheckBox*)item_autoScale->widget();
        if(cb_autoScale)
            pamChart.autoScale = cb_autoScale->isChecked();

        //pamChart.visible = true;
        pamChart.minYAxis = minYAxis;
        pamChart.maxYAxis = maxYAxis;

        pamAttrList.replace(i, pamChart);

    }
}

QList<PAMChart> PamPropDlg::getPamAttrList() const
{
    return pamAttrList;
}

void PamPropDlg::setPamAttrList(const QList<PAMChart> &value)
{
    pamAttrList = value;
}

void PamPropDlg::setChartProp(const ChartProp &value)
{
    chartProp = value;

    ui->sb_timegap->setValue(chartProp.timeGap);
    ui->cb_style->setCurrentIndex(chartProp.style);
}
