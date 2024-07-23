#ifndef SHRINKDIALOG_H
#define SHRINKDIALOG_H

#include <QDialog>

namespace Ui {
class ShrinkDialog;
}

class ShrinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShrinkDialog(QWidget *parent = nullptr, int PCT = 80);
    ~ShrinkDialog();

    void InitFactorSpinBoxProperty();

    void InitFactorSliderProperty();

    void ConnectSignalSlots();

    int GetShrinkFactor() const;

public slots:
    void slotSetFactorSpinBoxValue(int value);

    void slotSetFactorSliderValue(int value);

private:
    Ui::ShrinkDialog *ui;
};

#endif // SHRINKDIALOG_H
