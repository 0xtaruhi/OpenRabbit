#pragma once
#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <QList>
#include <QString>

/*
 * This file is used to include all the components.
 * The `IWYU pragma: export' is used to tell IWYU to export the included
 * headers. I declared the pragma to disable the warning.
 */

#include "LEDComponent.h"     // IWYU pragma: export
#include "SwitchComponent.h"  // IWYU pragma: export
#include "TextLCDComponent.h" // IWYU pragma: export

inline QList<QString> inputComponents() { return QList<QString>{"Switch"}; }

inline QList<QString> outputComponents() {
  return QList<QString>{"LED", "TextLCD"};
}

#endif // COMPONENTS_H
