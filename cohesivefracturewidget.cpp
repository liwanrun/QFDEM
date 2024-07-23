#include "cohesivefracturewidget.h"
#include "ui_cohesivefracturewidget.h"

CohesiveFractureWidget::CohesiveFractureWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::CohesiveFractureWidget)
{
    ui->setupUi(this);
    ui->treeBrowser->setHeaderVisible(true);
    ui->treeBrowser->setEnabled(false);
}

CohesiveFractureWidget::~CohesiveFractureWidget()
{
    delete ui;
}
