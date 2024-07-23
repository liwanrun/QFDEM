#ifndef FDEMCONTACTWIDGET_H
#define FDEMCONTACTWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMContactWidget;
}

class QtProperty;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class Contact;

class FDEMContactWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMContactWidget(QWidget *parent = nullptr);
    ~FDEMContactWidget();

    void InitPropertyBrowser();

    void ConnectSignalSlots();

private:
    void CreateOptionProperty();

    void CreateParameterProperty();

    void CreateTimestepProperty();

    void CreatePreferenceProperty();

    void CreateUtilityProperty();

signals:
    void signalOutputPromptMessage(const QString msg);

    void signalAnalysisModuleChanged(const QVariant& value);

public slots:
    void slotUpdateButtonClicked();

    void slotTreeBrowserValueChanged(QtProperty *property, const QVariant &value);

private:
    Ui::FDEMContactWidget *ui;

    QtVariantPropertyManager *manager;
    QtVariantEditorFactory   *factory;

    QSharedPointer<Contact> contact;
};

#endif // FDEMCONTACTWIDGET_H
