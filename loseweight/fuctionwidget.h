#ifndef FUCTIONWIDGET_H
#define FUCTIONWIDGET_H

#include <QWidget>
#include "tool_button.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

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

protected:
   void resizeEvent(QResizeEvent *event);
};

#endif // FUCTIONWIDGET_H
