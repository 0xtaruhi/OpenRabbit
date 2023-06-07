#pragma once
#include "qtmetamacros.h"
#ifndef LED_COMPONENT_H
#define LED_COMPONENT_H

#include <QLabel>

#include "Components/AbstractComponent.h"
#include "Components/ComponentMacro.h"

namespace rabbit_App::component {

COMPONENT_CLASS_DECLARATION(LED)

class LEDRawComponent final : public AbstractRawComponent {
  Q_OBJECT

public:
  LEDRawComponent(QWidget *parent = nullptr);
  virtual ~LEDRawComponent();

  void reset() override;

protected:
  void paintEvent(QPaintEvent *event) override;
  void initPorts() override;

public slots:
  void onSwitchLED(bool is_on);

private:
  QLabel *led_picture_;
  bool is_on_ = false;
};

} // namespace rabbit_App::component

#endif // LED_COMPONENT_H
