#include <QAction>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <stdint.h>

#include "Components/PanelGuiUpdateController.h"
#include "FPGA/AsyncVLFDReadWrite.h"
#include "TabToolbar/Group.h"
#include "TabToolbar/Page.h"
#include "TabToolbar/SubGroup.h"
#include "TabToolbar/TabToolbar.h"

#include "FPGA/VLFDDeviceHandler.h"

#include "ProjectManager.h"

#include "Components/ComponentAction.h"
#include "Components/Components.h"
#include "MainWindow.h"

using namespace rabbit_App;
using namespace rabbit_App::component;

constexpr auto kMinWindowWidth = 800;
constexpr auto kMinWindowHeight = 600;
const auto kDefaultStyle = "Vienna";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  initMembers();
  initLayout();
  initActions();
  initConnections();
}

MainWindow::~MainWindow() {}

void MainWindow::initMembers() {
  builder_ = new tt::Builder(this);
  builder_->SetCustomWidgetCreator("pushButton",
                                   []() { return new QPushButton(); });
  tab_tool_bar_ = new tt::TabToolbar(this, 75, 3);
  status_bar_ = new QStatusBar(this);
  status_bar_->addPermanentWidget(
      new QLabel(tr("USB Disconnected"), status_bar_));

  components_panel_ = new ComponentsPanel(this);
  new_project_dialog_ = new NewProjectDialog(this);

  // fpga_manager_ = new FPGAManager(this);
  vlfd_device_handler_ = new fpga::VLFDDeviceHandler(this);
  project_manager_ = new ProjectManager(this);
  panel_gui_update_controller_ = new PanelGuiUpdateController(this);
  value_update_controller_ = new ValueUpdateController(components_panel_, this);

  settings_dialog_ = new SettingsDialog(project_manager_, this);

  project_manager_->setComponentsPanel(components_panel_);
}

void MainWindow::initActions() {
  tt::Page *project_page = tab_tool_bar_->AddPage("Project");
  tt::Group *file_group = project_page->AddGroup(tr("File"));

  auto new_project_action = new QAction(tr("New"), this);
  new_project_action->setIcon(QIcon(":/icons/icons/icons8-add-file-94.png"));
  file_group->AddAction(QToolButton::DelayedPopup, new_project_action);
  connect(new_project_action, &QAction::triggered, this,
          &MainWindow::onNewProjectClicked);

  auto open_project_action = new QAction(tr("Open"), this);
  open_project_action->setIcon(QIcon(":/icons/icons/icons8-folder-94.png"));
  file_group->AddAction(QToolButton::DelayedPopup, open_project_action);
  connect(open_project_action, &QAction::triggered, this,
          &MainWindow::onOpenProjectClicked);

  auto save_project_action = new QAction(tr("Save"), this);
  save_project_action->setIcon(QIcon(":/icons/icons/icons8-save-94.png"));
  file_group->AddAction(QToolButton::DelayedPopup, save_project_action);
  connect(save_project_action, &QAction::triggered, this,
          &MainWindow::onSaveProjectClicked);

  auto save_as_project_action = new QAction(tr("Save As"), this);
  save_as_project_action->setIcon(QIcon(":/icons/icons/icons8-save-as-94.png"));
  file_group->AddAction(QToolButton::DelayedPopup, save_as_project_action);
  connect(save_as_project_action, &QAction::triggered, this,
          &MainWindow::onSaveProjectAsClicked);

  file_group->AddSeparator();

  auto settings_action = new QAction(tr("Settings"));
  settings_action->setIcon(QIcon(":/icons/icons/icons8-setting-94.png"));
  file_group->AddAction(QToolButton::DelayedPopup, settings_action);
  connect(settings_action, &QAction::triggered, this,
          &MainWindow::onSettingsClicked);

  tt::Group *bitstream_group = project_page->AddGroup(tr("Bitstream"));
  auto download_bitstrem_action = new QAction(tr("Download"), this);
  download_bitstrem_action->setIcon(
      QIcon(":/icons/icons/icons8-download-94.png"));
  bitstream_group->AddAction(QToolButton::DelayedPopup,
                             download_bitstrem_action);
  connect(download_bitstrem_action, &QAction::triggered, this,
          &MainWindow::onDownloadBitstreamClicked);

  tt::Group *running_group = project_page->AddGroup(tr("Running"));
  tt::SubGroup *frequency_group =
      running_group->AddSubGroup(tt::SubGroup::Align::No);
  frequency_group->AddWidget(new QLabel(tr("Frequency")));
  frequency_spin_box_ = new QSpinBox();
  frequency_spin_box_->setRange(1, 50000);
  frequency_spin_box_->setSuffix("Hz");
  vlfd_device_handler_->onFrequencyChanged(1);
  frequency_group->AddWidget(frequency_spin_box_);
  running_group->AddSeparator();
  run_action_ = new QAction(tr("Run"), this);
  run_action_->setIcon(QIcon(":/icons/icons/icons8-start-94.png"));
  running_group->AddAction(QToolButton::DelayedPopup, run_action_);
  connect(run_action_, &QAction::triggered, this,
          &MainWindow::onRunningStartClicked);

  stop_action_ = new QAction(tr("Stop"), this);
  stop_action_->setIcon(QIcon(":/icons/icons/icons8-cancel-94.png"));
  running_group->AddAction(QToolButton::DelayedPopup, stop_action_);
  connect(stop_action_, &QAction::triggered, this,
          &MainWindow::onRunningStopClicked);
  stop_action_->setDisabled(true);

  tt::Page *components_page = tab_tool_bar_->AddPage("Components");
  tt::Group *inputs_group = components_page->AddGroup("Inputs");
  tt::Group *outputs_group = components_page->AddGroup("Outputs");

  QList<ComponentAction *> output_component_actions;
  for (const auto &output_component : outputComponents()) {
    output_component_actions.push_back(
        ComponentActionFactory::create(output_component, this));
  }

  QList<ComponentAction *> input_component_actions;
  for (const auto &input_component : inputComponents()) {
    input_component_actions.push_back(
        ComponentActionFactory::create(input_component, this));
  }

  for (auto output : output_component_actions) {
    outputs_group->AddAction(QToolButton::InstantPopup, output);
    connect(output, &ComponentAction::triggered, components_panel_, [=]() {
      components_panel_->appendComponent(output->createComponent(), 3, 1);
    });
  }

  for (auto input : input_component_actions) {
    inputs_group->AddAction(QToolButton::InstantPopup, input);
    connect(input, &ComponentAction::triggered, components_panel_, [=]() {
      components_panel_->appendComponent(input->createComponent(), 2, 1);
    });
  }
}

void MainWindow::initLayout() {
  setMinimumSize(kMinWindowWidth, kMinWindowHeight);
  setWindowTitle("Rabbit");

  tab_tool_bar_->SetStyle(kDefaultStyle);
  addToolBar(Qt::TopToolBarArea, tab_tool_bar_);
  setStatusBar(status_bar_);

  setCentralWidget(components_panel_);
  components_panel_->appendComponent(new LEDComponent(), 1, 1);
}

void MainWindow::initConnections() {
  connect(vlfd_device_handler_,
          &fpga::VLFDDeviceHandler::downloadBitstreamSuccess, this,
          &MainWindow::onDownloadBitstreamSuccess);
  connect(vlfd_device_handler_,
          &fpga::VLFDDeviceHandler::downloadBitstreamFailure, this,
          &MainWindow::onDownloadBitstreamFailure);

  connect(frequency_spin_box_, QOverload<int>::of(&QSpinBox::valueChanged),
          vlfd_device_handler_, &fpga::VLFDDeviceHandler::onFrequencyChanged);

  connect(vlfd_device_handler_, &fpga::VLFDDeviceHandler::readWriteDone, this,
          &MainWindow::onReadWriteDone);
  connect(vlfd_device_handler_, &fpga::VLFDDeviceHandler::readWriteError, this,
          &MainWindow::onReadWriteError);

  // connect(vlfd_device_handler_->ayncVLFDReadWriteHandler(),
  //         &fpga::AsyncVLFDReadWrite::readWriteDone, panel_update_controller_,
  //         &component::PanelUpdateController::onReadData);
  // connect(panel_update_controller_,
  //         &component::PanelUpdateController::updatePanel, components_panel_,
  //         &component::ComponentsPanel::onUpdatePanel);
  connect(vlfd_device_handler_->ayncVLFDReadWriteHandler(),
          &fpga::AsyncVLFDReadWrite::readWriteDone, value_update_controller_,
          &component::ValueUpdateController::onReceiveData);
  connect(panel_gui_update_controller_,
          &component::PanelGuiUpdateController::updateGui, components_panel_,
          &component::ComponentsPanel::onUpdateGui);

  connect(project_manager_, &ProjectManager::updateProjectName, this,
          &MainWindow::onProjectNameUpdated);
}

void MainWindow::onNewProjectClicked() { new_project_dialog_->exec(); }

void MainWindow::onOpenProjectClicked() {
  try {
    project_manager_->onOpenProject();
  } catch (const std::exception &e) {
    QMessageBox msg;
    msg.setText(tr("Open project failed."));
    msg.setIcon(QMessageBox::Critical);
    msg.setWindowTitle(tr("Open project"));
    msg.setDetailedText(e.what());
    msg.exec();
  }
}

void MainWindow::onSaveProjectClicked() { project_manager_->onSaveProject(); }

void MainWindow::onSaveProjectAsClicked() {
  project_manager_->onSaveAsProject();
}

void MainWindow::onSettingsClicked() { settings_dialog_->exec(); }

void MainWindow::onDownloadBitstreamClicked() {
  vlfd_device_handler_->program(project_manager_->getBitstreamPath());
}

void MainWindow::onRunningStartClicked() {
  run_action_->setDisabled(true);
  stop_action_->setEnabled(true);
  vlfd_device_handler_->onStartRunning();
  panel_gui_update_controller_->onStartUpdate();
  value_update_controller_->onStartUpdate();
  components_panel_->resetAllComponents();
}

void MainWindow::onRunningStopClicked() {
  run_action_->setEnabled(true);
  stop_action_->setDisabled(true);
  vlfd_device_handler_->onStopRunning();
  panel_gui_update_controller_->onStopUpdate();
  value_update_controller_->onStopUpdate();
  components_panel_->resetAllComponents();
}

void MainWindow::onDownloadBitstreamSuccess() {
  QMessageBox msg;
  msg.setText(tr("Download bitstream success."));
  msg.setIcon(QMessageBox::Information);
  msg.setWindowTitle(tr("Download Bitstream"));
  msg.exec();
}

void MainWindow::onDownloadBitstreamFailure(const QString &error_message) {
  QMessageBox msg;
  msg.setText(error_message);
  msg.setIcon(QMessageBox::Critical);
  msg.setWindowTitle(tr("Download Bitstream"));
  msg.exec();
}

void MainWindow::onReadWriteDone(const std::vector<uint16_t> &read_data) {
  static long long time = 0;
  for (size_t i = 0; i < read_data.size(); i += 4) {
    qDebug() << time << " " << read_data[i] << " " << read_data[i + 1] << " "
             << read_data[i + 2] << " " << read_data[i + 3];
    time += 1;
  }
}

void MainWindow::onReadWriteError(const QString &error_message) {
  onRunningStopClicked();
  QMessageBox msg;
  msg.setText(error_message);
  msg.setIcon(QMessageBox::Critical);
  msg.setWindowTitle(tr("Read/Write"));
  msg.exec();
}

void MainWindow::onProjectNameUpdated(const QString &project_name) {
  setWindowTitle(project_name + " - Rabbit");
}
