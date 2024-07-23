#ifndef COHESIVEFRACTUREWIDGET_H
#define COHESIVEFRACTUREWIDGET_H

#include "constitutivemodelwidget.h"

namespace Ui {
class CohesiveFractureWidget;
}

class CohesiveFractureModel;
class CohesiveFractureWidget : public ConstitutiveModelWidget
{
    Q_OBJECT

public:
    explicit CohesiveFractureWidget(QWidget *parent = nullptr);
    ~CohesiveFractureWidget();

     void CreateConstitutiveProperty(ConstitutiveModel *) override
     {

     };

private:
    Ui::CohesiveFractureWidget *ui;

    CohesiveFractureModel *model;
};

#endif // COHESIVEFRACTUREWIDGET_H
