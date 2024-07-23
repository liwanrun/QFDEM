#ifndef TRANSLATEDIALOG_H
#define TRANSLATEDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
class TransformDialog;
}

class TransformDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransformDialog(int type, QWidget *parent = nullptr);
    ~TransformDialog();

    void ConnectSignalSlot();

public slots:
    void slotBoxXValueChanged(double x);

    void slotBoxYValueChanged(double y);

    void slotBoxZValueChanged(double z);

    void slotBoxWValueChanged(double w);

    void slotDialogButtonBoxClicked(QAbstractButton *button);

private:
    void ProcessTranslation(QAbstractButton *button);

    void ProcessRotation(QAbstractButton *button);

    void ProcessScale(QAbstractButton *button);

private:
    Ui::TransformDialog *ui;

    int m_type;
    double dofs[4];
};

#endif // TRANSLATEDIALOG_H
