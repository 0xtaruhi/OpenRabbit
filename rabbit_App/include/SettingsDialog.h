#pragma once
#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>

#include "ProjectManager.h"

namespace rabbit_App {

class SettingsDialog : public QDialog {
  Q_OBJECT

public:
  SettingsDialog(ProjectManager* project_manager, QWidget* parent);
  virtual ~SettingsDialog();

protected:
  /// @brief This method is called when the dialog shows. 
  void showEvent(QShowEvent* event) override;

private:
  ProjectManager* project_manager_;
};

}

#endif // SETTINGS_DIALOG_H
