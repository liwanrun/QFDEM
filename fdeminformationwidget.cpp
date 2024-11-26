#include "fdeminformationwidget.h"
#include "ui_fdeminformationwidget.h"

#include <QDateTime>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QDebug>

FDEMInformationWidget::FDEMInformationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FDEMInformationWidget),
    pageIndex(1)
{
    ui->setupUi(this);
    this->InitModuleNameList();
    this->PrintHeaderLine(pageIndex);
    this->InitCustomContextMenu();
    this->ConnectSignalSlots();
}

FDEMInformationWidget::~FDEMInformationWidget()
{
    delete ui;
}

void FDEMInformationWidget::InitModuleNameList()
{
    names.push_back(QString("\n[PREFERENCE]"));
    names.push_back(QString("\n[ SOLUTION ]"));
    names.push_back(QString("\n[ GEOMETRY ]"));
    names.push_back(QString("\n[ MESHING  ]"));
    names.push_back(QString("\n[ GROUPS   ]"));
    names.push_back(QString("\n[ CONTACT  ]"));
    names.push_back(QString("\n[ MATERIAL ]"));
    names.push_back(QString("\n[ BOUNDARY ]"));
    names.push_back(QString("\n[ TEMPORAL ]"));
    names.push_back(QString("\n[ HISTORY  ]"));
}

void FDEMInformationWidget::PrintHeaderLine(int index)
{
//    QString logo = QString(" _____ ____  _____ __  __       ____  _   _ _____\n"
//                           "|  ___|  _ \\| ____|  \\/  |     |  _ \\| | | |_   _|\n"
//                           "| |_  | | | |  _| | |\\/| |_____| | | | | | | | |\n"
//                           "|  _| | |_| | |___| |  | |_____| |_| | |_| | | |\n"
//                           "|_|   |____/|_____|_|  |_|     |____/ \\___/  |_|");

    QLocale locale = QLocale::English;
    QString datetime = locale.toString(QDateTime::currentDateTime(),tr("MMMM dd, yyyy(dddd) | HH:mm:ss"));
    QString logo = QString("==================== Q F D E M ===================\n"
                           "Info: A proprietary preprocessor for YFDEM solver \n"
                           "Info: Version 3.25.33-LTS, Copyright(C) 2018-2025 \n"
                           "Info: %1                                          \n"
                           "==================================================").arg(datetime);

    ui->infoPanel->setReadOnly(true);
    ui->infoPanel->setWordWrapMode(QTextOption::NoWrap);
    slotOutputPromptMessage(logo);
    slotOutputModuleName(index);
}

void FDEMInformationWidget::ClearInfoContent()
{
    ui->infoPanel->clear();
    this->PrintHeaderLine(pageIndex);
}

void FDEMInformationWidget::ShowContextMenu(QPoint pos)
{
    contextMenu->move(ui->infoPanel->mapToGlobal(pos));
    contextMenu->show();
}

void FDEMInformationWidget::slotOutputModuleName(int index)
{
    ui->infoPanel->setAlignment(Qt::AlignJustify);
    ui->infoPanel->setFontWeight(QFont::Bold);
    ui->infoPanel->setTextColor(Qt::darkRed);
    ui->infoPanel->append(names[index]);
    this->pageIndex = index;
}

void FDEMInformationWidget::slotOutputPromptMessage(const QString msg)
{
    ui->infoPanel->setAlignment(Qt::AlignJustify);
    ui->infoPanel->setFontWeight(QFont::Normal);
    ui->infoPanel->setTextColor(Qt::black);
    ui->infoPanel->append(msg);
}

void FDEMInformationWidget::ConnectSignalSlots()
{
    connect(actionClear, &QAction::triggered,
            this, &FDEMInformationWidget::ClearInfoContent);
    connect(ui->infoPanel, &QTextEdit::customContextMenuRequested,
            this, &FDEMInformationWidget::ShowContextMenu);
}

void FDEMInformationWidget::InitCustomContextMenu()
{
    ui->infoPanel->setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = ui->infoPanel->createStandardContextMenu();
    actionClear = new QAction(QString("Clear"), ui->infoPanel);
    contextMenu->addAction(actionClear);
}
