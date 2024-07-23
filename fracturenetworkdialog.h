#ifndef FRACTURENETWORKDIALOG_H
#define FRACTURENETWORKDIALOG_H

#include <QDialog>

namespace Ui {
class FractureNetworkDialog;
}

class QStandardItem;
class QItemSelection;
class QStandardItemModel;
class FractureNetworkDelegate;
class QtProperty;
class QtAbstractPropertyManager;
class QtVariantPropertyManager;
class QtVariantEditorFactory;
class QtTreePropertyBrowser;

class PDFInfo;
class FractureNetwork;
class PointTraceFractureSet;
class PointPointFractureSet;
class FractureNetworkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FractureNetworkDialog(QWidget *parent = nullptr);
    ~FractureNetworkDialog();

    void InitFractureTableViewGroup();

    void InitDomainOfInterestGroup();

    void InitFractureFunctionGroup();

    void ConnectSignalSlots();

    void UpdatePointTraceFractureBrowser(PointTraceFractureSet *fracSet);

    void UpdatePointPointFractureBrowser(PointPointFractureSet *fracSet);

private:    
    QList<QStandardItem *> CreateFractureItems(const QString& fractureId);

    void InitFractureDistributionBrowser(QtTreePropertyBrowser *browser, QtVariantPropertyManager* manager,
                                         const QString &title, const QStringList& names, QColor bkgColor);

    void UpdateFractureDistributionBrowser(QtVariantPropertyManager* manager, const PDFInfo &info);

private:
    void SetPropertyVisibility(QtTreePropertyBrowser *browser, QtProperty *property, bool visible);

    void UpdatePointSampleTreeWidget(QtTreePropertyBrowser* browser, QtProperty *property, int index);

    void UpdateValueSampleTreeWidget(QtTreePropertyBrowser* browser, QtProperty *property, int index);

    void UpdateFractureSetMetaInfo(const QString &id, const QVariant &value, PDFInfo& info);

public slots:
    void slotAppendButtonClicked();

    void slotRemoveButtonClicked();

    void slotClearButtonClicked();

    void slotTableModelItemChanged(QStandardItem *item);

    void slotSelectionChanged(const QItemSelection &selection, const QItemSelection &deselected);

public:
    void slotDomainOfInterestValueChanged(QtProperty *property, const QVariant &value);

    void slotPositionBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotTraceBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotDipBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotSourceBrowserValueChanged(QtProperty *property, const QVariant &value);

    void slotTargetBrowserValueChanged(QtProperty *property, const QVariant &value);

public:
    QRectF& GetDomainOfInterest();

    QSharedPointer<FractureNetwork> GetFractureNetwork();

private:
    Ui::FractureNetworkDialog *ui;

    QStandardItemModel *tableModel;
    FractureNetworkDelegate* delegate;

    QtVariantEditorFactory* factory;
    QtVariantPropertyManager *doiManager;
    QtVariantPropertyManager *posManager;
    QtVariantPropertyManager *modManager;
    QtVariantPropertyManager *dipManager;
    QtVariantPropertyManager *srcManager;
    QtVariantPropertyManager *tarManager;

    QRectF domainOfInterest;
    QSharedPointer<FractureNetwork> dfn;
};

#endif // FRACTURENETWORKDIALOG_H
