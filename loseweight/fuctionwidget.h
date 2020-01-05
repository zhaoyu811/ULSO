#ifndef FUCTIONWIDGET_H
#define FUCTIONWIDGET_H

#include <QWidget>
#include "tool_button.h"
#include <QVBoxLayout>
#include <QLabel>

class FuctionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FuctionWidget(QWidget *parent = nullptr);

private:
   ToolButton *userButton;
   ToolButton *collectInfButton;
   ToolButton *forecastButton;

Q_SIGNALS:
   void ToolButtonClicked(int index);

};

#endif // FUCTIONWIDGET_H
