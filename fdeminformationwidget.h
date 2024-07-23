#ifndef FDEMINFORMATIONWIDGET_H
#define FDEMINFORMATIONWIDGET_H

#include <QWidget>

namespace Ui {
class FDEMInformationWidget;
}

class QMenu;
class QAction;

class FDEMInformationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FDEMInformationWidget(QWidget *parent = nullptr);
    ~FDEMInformationWidget();

private:
    void PrintHeaderLine(int index);

    void InitModuleNameList();

    void InitCustomContextMenu();

    void ConnectSignalSlots();

public slots:
    void ClearInfoContent();

    void ShowContextMenu(QPoint pos);

    void slotOutputModuleName(int index);

    void slotOutputPromptMessage(const QString msg);

private:
    Ui::FDEMInformationWidget *ui;
    QMenu *contextMenu;
    QAction *actionClear;

    int pageIndex;
    QList<QString> names;
};

#endif // FDEMINFORMATIONWIDGET_H
