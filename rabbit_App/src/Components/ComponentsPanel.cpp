#include <cassert>

#include <QDrag>
#include <QDropEvent>
#include <QEvent>
#include <QMimeData>
#include <QUuid>

#include "Components/AbstractComponent.h"
#include "Components/ComponentsPanel.h"

using namespace rabbit_App::component;

ComponentsPanel::ComponentsPanel(QWidget *parent) : QWidget(parent) {
  setAcceptDrops(true);
  initConnections();
}

ComponentsPanel::~ComponentsPanel() {}

void ComponentsPanel::resetAllComponents() {
  for (auto component : components_) {
    component->reset();
  }
}

void ComponentsPanel::onUpdateGui() {
  for (auto component : components_) {
    component->update();
  }
}

void ComponentsPanel::dragEnterEvent(QDragEnterEvent *event) {
  if (event->mimeData()->hasFormat("application/x-abstractcomponent")) {
    event->acceptProposedAction();
  }
}

void ComponentsPanel::dragMoveEvent(QDragMoveEvent *event) {
  event->acceptProposedAction();
}

void ComponentsPanel::dropEvent(QDropEvent *event) {
  if (event->mimeData()->hasFormat("application/x-abstractcomponent")) {
    QByteArray itemData =
        event->mimeData()->data("application/x-abstractcomponent");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QString uuid;
    QPoint drag_start_pos;
    dataStream >> uuid >> drag_start_pos;

    auto component = components_.value(QUuid(uuid));
    assert(component != nullptr);

    // Get the nearest grid position and set the component's position to it.
    auto mouse_pos = event->position().toPoint();
    auto grid_row = mouse_pos.y() / grid_height_;
    auto grid_col = mouse_pos.x() / grid_width_;
    component->setGeometry(grid_width_ * grid_col, grid_height_ * grid_row,
                           component->width(), component->height());

    event->setDropAction(Qt::MoveAction);
    event->accept();
  } else {
    event->ignore();
  }
}

void ComponentsPanel::initConnections() {
  connect(this, &ComponentsPanel::gridSizeChanged, this,
          &ComponentsPanel::onGridSizeChanged);
}

void ComponentsPanel::onGridSizeChanged(int grid_width, int grid_height) {
  for (const auto &component : components_) {
    auto component_grid_size = component->gridOccupation();
    auto component_width = component_grid_size.width() * grid_width;
    auto component_height = component_grid_size.height() * grid_height;
    component->setFixedSize(component_width, component_height);

    auto component_grid_pos = componentGridCoords(component);
    component->setGeometry(grid_width * component_grid_pos.x(),
                           grid_height * component_grid_pos.y(),
                           component_width, component_height);
  }
  grid_width_ = grid_width;
  grid_height_ = grid_height;
}

void ComponentsPanel::appendComponent(AbstractComponent *component,
                                      int gird_row, int grid_col) {
  component->setParent(this);
  components_.insert(component->uuid(), component);

  auto component_grid_size = component->gridOccupation();
  auto component_width = component_grid_size.width() * grid_width_;
  auto component_height = component_grid_size.height() * grid_height_;
  component->setFixedSize(component_width, component_height);

  component->setGeometry(grid_width_ * grid_col, grid_height_ * gird_row,
                         component_width, component_height);

  component->show();
  connect(component, &AbstractComponent::componentRemoved, this,
          &ComponentsPanel::removeComponent);
}

void ComponentsPanel::appendComponent(AbstractComponent *component) {
  return appendComponent(component, 0, 0);
}

void ComponentsPanel::removeComponent(AbstractComponent *component) {
  components_.erase(components_.find(component->uuid()));
  component->deleteLater();
}
