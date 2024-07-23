#include "fractaldimensiondialog.h"
#include "ui_fractaldimensiondialog.h"
#include "fractaldimension.h"

#include <QDebug>
#include <QSharedPointer>

FractalDimensionDialog::FractalDimensionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FractalDimensionDialog)
{
    ui->setupUi(this);

    this->domain = QRectF(0.0, 0.0, 10.0, 10.0);
    this->InitFractalWidget();
    this->ConnectSignalSlot();
}

FractalDimensionDialog::~FractalDimensionDialog()
{
    delete ui;
}

void FractalDimensionDialog::InitFractalWidget()
{

    ui->correlateButton->setChecked(true);
    ui->rankSpinBox->setValue(5);

    QStringList headers;
    headers << "length" << "count";
    ui->tableWidget->setRowCount(5);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->setColumnWidth(0, 130);
    ui->tableWidget->setColumnWidth(1, 130);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDefault(true);
}

void FractalDimensionDialog::ConnectSignalSlot()
{
    connect(ui->xcoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FractalDimensionDialog::slotXcoordSpinBoxValueChanged);
    connect(ui->ycoordSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FractalDimensionDialog::slotYcoordSpinBoxValueChanged);
    connect(ui->widthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FractalDimensionDialog::slotWidthSpinBoxValueChanged);
    connect(ui->heightSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FractalDimensionDialog::slotHeightSpinBoxValueChanged);
    connect(ui->rankSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &FractalDimensionDialog::slotFractalRankBoxValueChanged);
    connect(ui->buttonBox, &QDialogButtonBox::clicked,
            this, &FractalDimensionDialog::slotButtonBoxClicked);
}

void FractalDimensionDialog::slotXcoordSpinBoxValueChanged(double value)
{
    this->domain.setX(value);
}

void FractalDimensionDialog::slotYcoordSpinBoxValueChanged(double value)
{
    this->domain.setY(value);
}

void FractalDimensionDialog::slotWidthSpinBoxValueChanged(double value)
{
    this->domain.setWidth(value);
}

void FractalDimensionDialog::slotHeightSpinBoxValueChanged(double value)
{
    this->domain.setHeight(value);
}

void FractalDimensionDialog::slotFractalRankBoxValueChanged(int value)
{
    ui->tableWidget->setRowCount(value);
}

void FractalDimensionDialog::slotButtonBoxClicked(QAbstractButton* button)
{
    switch (ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Apply:
    {
        if(ui->boxCountButton->isChecked())
        {
            QSharedPointer<FractalDimension> boxCountDim = QSharedPointer<FractalDimension>(new BoxCountingDimension);
            for(int i = 0; i < ui->rankSpinBox->value(); i++)
            {
                double cl = this->domain.width() / std::pow(2, i);
                int count = boxCountDim->Count(this->domain, cl);

                QTableWidgetItem *length = new QTableWidgetItem(QString::number(cl));
                ui->tableWidget->setItem(i, 0, length);
                QTableWidgetItem *number = new QTableWidgetItem(QString::number(count));
                ui->tableWidget->setItem(i, 1, number);
            }
        }
        else if(ui->correlateButton->isChecked())
        {
            QSharedPointer<FractalDimension> fractalDim = QSharedPointer<FractalDimension>(new CorrelationDimension);
            for(int i = 0; i < ui->rankSpinBox->value(); i++)
            {
                double cl = this->domain.width() / std::pow(2, i);
                int count = fractalDim->Count(this->domain, cl);

                QTableWidgetItem *length = new QTableWidgetItem(QString::number(cl));
                ui->tableWidget->setItem(i, 0, length);
                QTableWidgetItem *number = new QTableWidgetItem(QString::number(count));
                ui->tableWidget->setItem(i, 1, number);
            }
        }

        break;
    }
    case QDialogButtonBox::Ok:
    case QDialogButtonBox::Cancel:
    default:
        break;
    }
}
