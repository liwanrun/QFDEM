#ifndef BACKGROUNDGRIDDIALOG_H
#define BACKGROUNDGRIDDIALOG_H

#include <QDialog>
#include <QPushButton>
#include "backgroundgrid.h"

namespace Ui {
class BackgroundGridDialog;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QAbstractButton;
class BackgroundGridDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BackgroundGridDialog(QWidget *parent = nullptr);
    ~BackgroundGridDialog();
    
    void InitPropertyBrowser();

    void ConnectSignalToSlot();

    void slotCartesianCoordClicked(bool checked);

    void slotPolarCoordClicked(bool checked);

    void slotPropertyBrowserValueChanged(QtProperty *property, const QVariant& value);

    void slotDialogButtonBoxClicked(QAbstractButton *button);

private:
    Ui::BackgroundGridDialog *ui;
    
    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    QSharedPointer<BackgroundGrid> bkgGrid;
};

#endif // BACKGROUNDGRIDDIALOG_H
