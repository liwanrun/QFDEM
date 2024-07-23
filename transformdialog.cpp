#include "transformdialog.h"
#include "ui_transformdialog.h"
#include "fdemvtkrenderwidget.h"

#include <QAbstractButton>

TransformDialog::TransformDialog(int type, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransformDialog),
    m_type(type)
{
    ui->setupUi(this);

    ui->box_X->setDecimals(6);
    ui->box_X->setMinimum(-9.9e+10);
    ui->box_Y->setDecimals(6);
    ui->box_Y->setMinimum(-9.9e+10);
    ui->box_Z->setDecimals(6);
    ui->box_Z->setMinimum(-9.9e+10);
    ui->box_W->setDecimals(6);
    ui->box_W->setRange(-360.0, 360.0);

    this->dofs[0] = 0.0;
    this->dofs[1] = 0.0;
    this->dofs[2] = 0.0;
    this->dofs[3] = 0.0;

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDefault(true);

    this->ConnectSignalSlot();
}

TransformDialog::~TransformDialog()
{
    delete ui;
}

void TransformDialog::ConnectSignalSlot()
{
    connect(ui->box_X, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TransformDialog::slotBoxXValueChanged);
    connect(ui->box_Y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TransformDialog::slotBoxYValueChanged);
    connect(ui->box_Z, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TransformDialog::slotBoxZValueChanged);
    connect(ui->box_W, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TransformDialog::slotBoxWValueChanged);

    connect(ui->buttonBox, &QDialogButtonBox::clicked,
            this, &TransformDialog::slotDialogButtonBoxClicked);
}

void TransformDialog::slotBoxXValueChanged(double x)
{
    this->dofs[0] = x;
}

void TransformDialog::slotBoxYValueChanged(double y)
{
    this->dofs[1] = y;
}

void TransformDialog::slotBoxZValueChanged(double z)
{
    this->dofs[2] = z;
}

void TransformDialog::slotBoxWValueChanged(double w)
{
    this->dofs[3] = w;
}

void TransformDialog::slotDialogButtonBoxClicked(QAbstractButton *button)
{
    if(0 == m_type)
    {
        this->ProcessTranslation(button);
    }
    else if(1 == m_type)
    {
        this->ProcessRotation(button);
    }
    else if(2 == m_type)
    {
        this->ProcessScale(button);
    }
}

void TransformDialog::ProcessTranslation(QAbstractButton *button)
{
    if(FDEMVTKRenderWidget *parent = static_cast<FDEMVTKRenderWidget*>(this->parentWidget()))
    {
        switch (ui->buttonBox->standardButton(button))
        {
        case QDialogButtonBox::Ok:
        {
            parent->TranslateTriangulation(this->dofs);
            parent->slotCancelSelection();
            break;
        }
        case QDialogButtonBox::Cancel:
        {
            parent->slotCancelSelection();
            break;
        }
        case QDialogButtonBox::Apply:
        {
            parent->TranslateTriangulation(this->dofs);
            break;
        }
        default:
            break;
        }
    }
}

void TransformDialog::ProcessRotation(QAbstractButton *button)
{
    if(FDEMVTKRenderWidget *parent = static_cast<FDEMVTKRenderWidget*>(this->parentWidget()))
    {
        switch (ui->buttonBox->standardButton(button))
        {
        case QDialogButtonBox::Ok:
        {
            parent->RotateTriangulation(this->dofs);
            parent->slotCancelSelection();
            break;
        }
        case QDialogButtonBox::Cancel:
        {
            parent->slotCancelSelection();
            break;
        }
        case QDialogButtonBox::Apply:
        {
            parent->RotateTriangulation(this->dofs);
            break;
        }
        default:
            break;
        }
    }
}

void TransformDialog::ProcessScale(QAbstractButton *button)
{
    if(FDEMVTKRenderWidget *parent = static_cast<FDEMVTKRenderWidget*>(this->parentWidget()))
    {
        switch (ui->buttonBox->standardButton(button))
        {
        case QDialogButtonBox::Ok:
        {
            parent->ScaleTriangulation(this->dofs);
            parent->slotCancelSelection();
            break;
        }
        case QDialogButtonBox::Cancel:
        {
            parent->slotCancelSelection();
            break;
        }
        case QDialogButtonBox::Apply:
        {
            parent->ScaleTriangulation(this->dofs);
            break;
        }
        default:
            break;
        }
    }
}
