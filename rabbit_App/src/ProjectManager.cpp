#include <QFile>
#include <QFileDialog>
#include <QObject>
#include <QXmlStreamWriter>

#include "Components/ComponentsFactory.h"

#include "ProjectManager.h"

using namespace rabbit_App;

ProjectManager::ProjectManager(QObject *parent) : QObject(parent) {
  ports_file_reader_ = new ports::PortsFileReader(this);
  bitstream_path_ = QString("/home/taruhi/Project/Rabbit/Name_fde_yosys.bit");
}

ProjectManager::~ProjectManager() {}

void ProjectManager::readProjectFromFile(const QString &project_path) {
  QXmlStreamReader xml_reader;
  QFile file(project_path);
  file.open(QIODevice::ReadOnly);
  if (!file.isOpen()) {
    throw std::runtime_error("Failed to open file.");
  }
  xml_reader.setDevice(&file);
  while (!xml_reader.atEnd()) {
    xml_reader.readNext();
    auto name = xml_reader.name().toString();
    if (xml_reader.isStartElement()) {
      if (name == "project") {
        project_name_ = xml_reader.attributes().value("name").toString();
        constraint_path_ =
            xml_reader.attributes().value("constraint").toString();
        bitstream_path_ = xml_reader.attributes().value("bitstream").toString();
        ports_file_reader_->readFromFile(constraint_path_);
      } else if (name == "panel") {
        auto grid_width = xml_reader.attributes().value("grid_width").toInt();
        auto grid_height = xml_reader.attributes().value("grid_height").toInt();
        components_panel_->setGridSize(grid_width, grid_height);
      } else if (name == "components") {
        auto typeStr = xml_reader.attributes().value("type").toString();
        auto new_component = component::ComponentsFactory::create(typeStr);
        components_panel_->appendComponent(new_component);

        xml_reader.readNext();
        if (xml_reader.isStartElement()) {
          if (xml_reader.name().toString() == "inputs") {
            xml_reader.readNext();
            const auto &inputs = ports_file_reader_->inputs();
            while (xml_reader.isStartElement() &&
                   xml_reader.name().toString() == "input") {
              auto pin = xml_reader.attributes().value("pin").toString();
              auto port = xml_reader.attributes().value("port").toString();
              
              auto input = inputs.find(port);
              if (input != inputs.end()) {
                new_component->inputPorts()[pin] = *input;
              }
              xml_reader.readNext();
            }
          }
        }
        if (xml_reader.isStartElement()) {
          if (xml_reader.name().toString() == "outputs") {
            xml_reader.readNext();
            while (xml_reader.isStartElement() &&
                   xml_reader.name().toString() == "output") {
              auto pin = xml_reader.attributes().value("pin").toString();
              auto port = xml_reader.attributes().value("port").toString();

              auto output = ports_file_reader_->outputs().find(port);
              if (output != ports_file_reader_->outputs().end()) {
                new_component->outputPorts()[pin] = *output;
              }
              xml_reader.readNext();
            }
          }
        }
      }
    }
  }

  if (xml_reader.hasError()) {
    throw std::runtime_error(xml_reader.errorString().toLocal8Bit());
  }
  file.close();

  project_path_ = project_path;
  emit updateProjectName(project_name_);
}

void ProjectManager::createProject(const QString &project_name) {}

void ProjectManager::openProject() {
  QString default_rbtprj_dir = QDir::homePath();

  QString project_path = QFileDialog::getOpenFileName(
      nullptr, tr("Open Project"), default_rbtprj_dir,
      tr("Rabbit Project (*.rbtprj)"));
  if (project_path.isEmpty()) {
    return;
  }

  try {
    readProjectFromFile(project_path);
  } catch (std::runtime_error &e) {
    throw e;
  }
}

void ProjectManager::saveProject() {
  if (project_path_.isEmpty()) {
    saveAsProject();
    return;
  }

  QXmlStreamWriter xml_writer;
  QFile file(project_path_);
  file.open(QIODevice::WriteOnly);
  if (!file.isOpen()) {
    throw std::runtime_error("Failed to open file.");
  }
  xml_writer.setDevice(&file);
  xml_writer.setAutoFormatting(true);
  xml_writer.writeStartDocument();
  xml_writer.writeComment("Rabbit Project File");

  // Project Information
  xml_writer.writeStartElement("project");
  xml_writer.writeAttribute("name", project_name_);
  xml_writer.writeAttribute("constraint", constraint_path_);
  xml_writer.writeAttribute("bitstream", bitstream_path_);
  // xml_writer.writeEndElement();

  // Components Information
  xml_writer.writeStartElement("panel");
  xml_writer.writeAttribute("grid_width",
                            QString::number(components_panel_->gridWidth()));
  xml_writer.writeAttribute("grid_height",
                            QString::number(components_panel_->gridHeight()));
  xml_writer.writeEndElement();

  xml_writer.writeStartElement("components");
  for (const auto component : components_panel_->components()) {
    auto component_grid_pos = components_panel_->componentGridCoords(component);
    xml_writer.writeStartElement("component");
    xml_writer.writeAttribute("type", component->componentType());
    xml_writer.writeAttribute("grid_x",
                              QString::number(component_grid_pos.x()));
    xml_writer.writeAttribute("grid_y",
                              QString::number(component_grid_pos.y()));

    // Inputs
    xml_writer.writeStartElement("inputs");
    auto input_keys = component->inputPorts().keys();
    for (const auto &key : input_keys) {
      const auto port = component->inputPorts()[key];
      xml_writer.writeStartElement("input");
      xml_writer.writeAttribute("pin", key);
      xml_writer.writeAttribute("port", port.name);
      xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement();

    // Outputs
    xml_writer.writeStartElement("outputs");
    auto output_keys = component->outputPorts().keys();
    for (const auto &key : output_keys) {
      const auto port = component->outputPorts()[key];
      xml_writer.writeStartElement("output");
      xml_writer.writeAttribute("pin", key);
      xml_writer.writeAttribute("port", port.name);
      xml_writer.writeEndElement();
    }
    xml_writer.writeEndElement();

    xml_writer.writeEndElement();

    xml_writer.writeEndElement();
  }
  xml_writer.writeEndElement();

  xml_writer.writeEndElement();
  xml_writer.writeEndDocument();
  file.close();
}

void ProjectManager::saveAsProject() {
  QString default_rbtprj_dir = QDir::homePath();

  QString project_path = QFileDialog::getSaveFileName(
      nullptr, tr("Save Project"), default_rbtprj_dir,
      tr("Rabbit Project (*.rbtprj)"));
  if (project_path.isEmpty()) {
    return;
  }

  if (!project_path.endsWith(".rbtprj")) {
    project_path.append(".rbtprj");
  }

  project_path_ = project_path;
  saveProject();
}
