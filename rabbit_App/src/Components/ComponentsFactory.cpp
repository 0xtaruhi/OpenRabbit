#include "Components/ComponentsFactory.h"
#include "Components/Components.h"

using namespace rabbit_App::component;

AbstractComponent *ComponentsFactory::create(const QString &component_name,
                                             QWidget *parent) {
  AbstractComponent *new_component = nullptr;
  if (component_name == "LED") {
    new_component = new LEDComponent(parent);
  } else if (component_name == "TextLCD") {
    new_component = new TextLCDComponent(parent);
  } else if (component_name == "Switch") {
    new_component = new SwitchComponent(parent);
  } else {
    std::runtime_error("Unknown component type");
  }
  return new_component;
}
