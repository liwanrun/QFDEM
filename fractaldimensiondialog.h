#ifndef FRACTALDIMENSIONDIALOG_H
#define FRACTALDIMENSIONDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class FractalDimensionDialog;
}

class FractalDimensionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FractalDimensionDialog(QWidget *parent = nullptr);
    ~FractalDimensionDialog();

    void InitFractalWidget();

    void ConnectSignalSlot();

public slots:
    void slotXcoordSpinBoxValueChanged(double value);

    void slotYcoordSpinBoxValueChanged(double value);

    void slotWidthSpinBoxValueChanged(double value);

    void slotHeightSpinBoxValueChanged(double value);

    void slotFractalRankBoxValueChanged(int value);

    void slotButtonBoxClicked(QAbstractButton* button);

private:
    Ui::FractalDimensionDialog *ui;

    QRectF domain;
};

#endif // FRACTALDIMENSIONDIALOG_H
