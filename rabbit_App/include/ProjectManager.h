#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include <QObject>
#include <QString>

#include "Components/ComponentsPanel.h"
#include "Ports/PortsFileReader.h"

namespace rabbit_App {

class MainWindow;

class ProjectManager : public QObject {
  Q_OBJECT

  friend class MainWindow;
  friend class SettingsDialog;

public:
  ProjectManager(QObject *parent = nullptr);
  virtual ~ProjectManager();

  auto getProjectName() const noexcept { return project_name_; }
  auto getProjectPath() const noexcept { return project_path_; }
  auto getConstraintPath() const noexcept { return constraint_path_; }
  auto getBitstreamPath() noexcept { return bitstream_path_; }

  void setProjectName(const QString &project_name) {
    project_name_ = project_name;
  }
  void setProjectPath(const QString &project_path) {
    project_path_ = project_path;
  }
  void setConstraintPath(const QString &constraint_path) {
    constraint_path_ = constraint_path;
  }
  void setBitstreamPath(const QString &bitstream_path) {
    bitstream_path_ = bitstream_path;
  }

  void readProjectFromFile(const QString &project_path);
  void createProject(const QString &project_name);
  void openProject();
  void saveProject();
  void saveAsProject();

  QString getBitstreamPath() const noexcept { return bitstream_path_; }

signals:
  void updateProjectName(const QString &project_name);

public slots:
  void onOpenProject() { openProject(); }
  void onSaveProject() { saveProject(); }
  void onSaveAsProject() { saveAsProject(); }

private:
  QString project_name_;
  QString project_path_;
  QString constraint_path_;
  QString bitstream_path_;

  component::ComponentsPanel *components_panel_;
  ports::PortsFileReader *ports_file_reader_;

  void setComponentsPanel(component::ComponentsPanel *components_panel) {
    components_panel_ = components_panel;
  }
};

} // namespace rabbit_App

#endif // PROJECT_MANAGER_H
