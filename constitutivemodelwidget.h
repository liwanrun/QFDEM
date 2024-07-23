#ifndef CONSTITUTIVEMODELWIDGET_H
#define CONSTITUTIVEMODELWIDGET_H

#include <QWidget>

class QtProperty;
class ConstitutiveModel;
class ConstitutiveModelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConstitutiveModelWidget(QWidget *parent = nullptr);
    virtual ~ConstitutiveModelWidget();

    virtual void CreateConstitutiveProperty(ConstitutiveModel *model) = 0;
};

#endif // CONSTITUTIVEMODELWIDGET_H
