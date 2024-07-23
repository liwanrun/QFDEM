#include "trianglerigidbodymotionwidget.h"
#include "ui_trianglerigidbodymotionwidget.h"

TriangleRigidBodyMotionWidget::TriangleRigidBodyMotionWidget(QWidget *parent) :
    ConstitutiveModelWidget(parent),
    ui(new Ui::TriangleRigidBodyMotionWidget)
{
    ui->setupUi(this);

    ui->treeBrowser->setHeaderVisible(true);
    ui->treeBrowser->setEnabled(false);
}

TriangleRigidBodyMotionWidget::~TriangleRigidBodyMotionWidget()
{
    delete ui;
}
