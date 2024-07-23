#include "shrinkdialog.h"
#include "ui_shrinkdialog.h"

ShrinkDialog::ShrinkDialog(QWidget *parent, int PCT) :
    QDialog(parent),
    ui(new Ui::ShrinkDialog)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("Shrink Dialog"));
    this->InitFactorSliderProperty();
    this->InitFactorSpinBoxProperty();
    this->ConnectSignalSlots();

    ui->factorSpinBox->setValue(PCT);
}

ShrinkDialog::~ShrinkDialog()
{
    delete ui;
}

void ShrinkDialog::InitFactorSpinBoxProperty()
{
    ui->factorSpinBox->setRange(0, 100);
    ui->factorSpinBox->setSingleStep(10);
}

void ShrinkDialog::InitFactorSliderProperty()
{
    ui->factorSlider->setRange(0, 100);
    ui->factorSlider->setSingleStep(1);
    ui->factorSlider->setTickInterval(10);
    ui->factorSlider->setTickPosition(QSlider::TicksBelow);
}

void ShrinkDialog::ConnectSignalSlots()
{
    connect(ui->factorSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ShrinkDialog::slotSetFactorSliderValue);

    connect(ui->factorSlider, &QSlider::valueChanged,
            this, &ShrinkDialog::slotSetFactorSpinBoxValue);
}

void ShrinkDialog::slotSetFactorSpinBoxValue(int value)
{
    ui->factorSpinBox->setValue(value);
}

void ShrinkDialog::slotSetFactorSliderValue(int value)
{
    ui->factorSlider->setValue(value);
}

int ShrinkDialog::GetShrinkFactor() const
{
    return ui->factorSpinBox->value();
}

