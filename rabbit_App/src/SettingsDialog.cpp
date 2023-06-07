#include <QMessageBox>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

#include "SettingsDialog.h"

using namespace rabbit_App;

SettingsDialog::SettingsDialog(ProjectManager *project_manager, QWidget *parent)
    : QDialog(parent), project_manager_(project_manager) {
  QLabel* project_name_label = new QLabel(tr("Project Name"), this);
  QLineEdit* project_name_line_edit = new QLineEdit(this);
  project_name_line_edit->setText(project_manager_->getProjectName());

  QLabel* project_path_label = new QLabel(tr("Project Path"), this);
  QLineEdit* project_path_line_edit = new QLineEdit(this);
  project_path_line_edit->setText(project_manager_->getProjectPath());
  // Browse button
  QPushButton* browse_button = new QPushButton(tr("Browse"), this);
  connect(browse_button, &QPushButton::clicked, this, [=]() {
    QString project_path = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"), project_manager_->getProjectPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    project_path_line_edit->setText(project_path);
  });

  QLabel* constraint_path_label = new QLabel(tr("Constraint Path"), this);
  QLineEdit* constraint_path_line_edit = new QLineEdit(this);
  constraint_path_line_edit->setText(project_manager_->getConstraintPath());
  // Browse button
  QPushButton* constraint_browse_button = new QPushButton(tr("Browse"), this);
  connect(constraint_browse_button, &QPushButton::clicked, this, [=]() {
    QString constraint_path = QFileDialog::getOpenFileName(
        this, tr("Open File"), project_manager_->getConstraintPath(),
        tr("Constraint Files (*.xml)"));
    constraint_path_line_edit->setText(constraint_path);
  });

  QLabel* bitstream_path_label = new QLabel(tr("Bitstream Path"), this);
  QLineEdit* bitstream_path_line_edit = new QLineEdit(this);
  bitstream_path_line_edit->setText(project_manager_->getBitstreamPath());
  // Browse button
  QPushButton* bitstream_browse_button = new QPushButton(tr("Browse"), this);
  connect(bitstream_browse_button, &QPushButton::clicked, this, [=]() {
    QString bitstream_path = QFileDialog::getOpenFileName(
        this, tr("Open File"), project_manager_->getBitstreamPath(),
        tr("Bitstream Files (*.bit)"));
    bitstream_path_line_edit->setText(bitstream_path);
  });

  QPushButton* ok_button = new QPushButton(tr("OK"), this);
  connect(ok_button, &QPushButton::clicked, this, [=]() {
    project_manager_->setProjectName(project_name_line_edit->text());
    project_manager_->setProjectPath(project_path_line_edit->text());
    project_manager_->setConstraintPath(constraint_path_line_edit->text());
    project_manager_->setBitstreamPath(bitstream_path_line_edit->text());
    accept();
  });

  QPushButton* cancel_button = new QPushButton(tr("Cancel"), this);
  connect(cancel_button, &QPushButton::clicked, this, [=]() { reject(); });

  QGridLayout* layout = new QGridLayout(this);
  layout->addWidget(project_name_label, 0, 0);
  layout->addWidget(project_name_line_edit, 0, 1);
  layout->addWidget(project_path_label, 1, 0);
  layout->addWidget(project_path_line_edit, 1, 1);
  layout->addWidget(browse_button, 1, 2);
  layout->addWidget(constraint_path_label, 2, 0);
  layout->addWidget(constraint_path_line_edit, 2, 1);
  layout->addWidget(constraint_browse_button, 2, 2);
  layout->addWidget(bitstream_path_label, 3, 0);
  layout->addWidget(bitstream_path_line_edit, 3, 1);
  layout->addWidget(bitstream_browse_button, 3, 2);
  layout->addWidget(ok_button, 4, 1);
  layout->addWidget(cancel_button, 4, 2);
  setLayout(layout);
}

SettingsDialog::~SettingsDialog() {}

void SettingsDialog::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
}
