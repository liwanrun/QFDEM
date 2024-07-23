#ifndef TRIANGLERIGIDBODYMOTIONWIDGET_H
#define TRIANGLERIGIDBODYMOTIONWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class TriangleRigidBodyMotionWidget;
}

class RigidBodyMotionModel;
class TriangleRigidBodyMotionWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit TriangleRigidBodyMotionWidget(QWidget *parent = nullptr);
    ~TriangleRigidBodyMotionWidget();

    void CreateConstitutiveProperty(ConstitutiveModel *) override
    {

    };

private:
    Ui::TriangleRigidBodyMotionWidget *ui;

    RigidBodyMotionModel *model;
};

#endif // TRIANGLERIGIDBODYMOTIONWIDGET_H
