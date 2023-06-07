#include "Components/ComponentAction.h"
#include "Components/Components.h"
#include "Components/ComponentsFactory.h"

using namespace rabbit_App::component;

ComponentAction *ComponentActionFactory::create(const QString &text,
                                                QObject *parent) {
  auto action = new ComponentAction(parent);
  action->setText(text);

  action->component_factory_ = [=]() {
    return ComponentsFactory::create(text);
  };

  if (text == "LED") {
    action->setIcon(LEDComponent::componentIcon());
  } else if (text == "TextLCD") {
    action->setIcon(TextLCDComponent::componentIcon());
  } else if (text == "Switch") {
    action->setIcon(SwitchComponent::componentIcon());
  } else {
    std::runtime_error("Unknown component type");
  }
  
  return action;
}
