#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QSpinBox>
#include <QStatusBar>

#include "TabToolbar/Builder.h"

#include "Components/ComponentsPanel.h"
#include "Components/PanelGuiUpdateController.h"
#include "Components/ValueUpdateController.h"
#include "FPGA/VLFDDeviceHandler.h"
#include "NewProjectDialog.h"
#include "ProjectManager.h"
#include "SettingsDialog.h"

namespace rabbit_App {
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  virtual ~MainWindow();

private:
  tt::Builder *builder_;
  tt::TabToolbar *tab_tool_bar_;
  QStatusBar *status_bar_;
  component::ComponentsPanel *components_panel_;
  QSpinBox *frequency_spin_box_;

  // dialog handlers
  NewProjectDialog *new_project_dialog_;
  SettingsDialog *settings_dialog_;

  rabbit_App::fpga::VLFDDeviceHandler *vlfd_device_handler_;
  ProjectManager *project_manager_;
  component::PanelGuiUpdateController *panel_gui_update_controller_;
  component::ValueUpdateController *value_update_controller_;

  QAction *run_action_;
  QAction *stop_action_;

  void initMembers();

  /// @brief Initializes the actions for the toolbar.
  /// It is called by initMembers(). Do not call it directly.
  void initActions();

  void initLayout();
  void initConnections();

signals:
  void downloadBitstreamClicked(const QString &bitstream_path);
  void frequencyChanged(int frequency);

private slots:
  void onNewProjectClicked();
  void onOpenProjectClicked();
  void onSaveProjectClicked();
  void onSaveProjectAsClicked();

  void onSettingsClicked();

  void onDownloadBitstreamClicked();
  void onRunningStartClicked();
  void onRunningStopClicked();

  void onDownloadBitstreamSuccess();
  void onDownloadBitstreamFailure(const QString &error_message);

  void onReadWriteDone(const std::vector<uint16_t> &read_data);
  void onReadWriteError(const QString &error_message);

  void onProjectNameUpdated(const QString &project_name);
};
} // namespace rabbit_App

#endif // MAINWINDOW_H
