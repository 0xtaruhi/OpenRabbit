#pragma once
#include "qwidget.h"
#ifndef ABSTRACT_COMPONENT_H
#define ABSTRACT_COMPONENT_H

#include <QPoint>
#include <QSize>
#include <QUuid>
#include <QWidget>
#include <QBitArray>

#include "Components/ComponentTitleBar.h"
#include "Ports/Port.h"

namespace rabbit_App::component {

/// @brief Abstract class for all raw components.
/// The raw component is the true component that accepts inputs and displays
/// outputs.
class AbstractRawComponent : public QWidget {
  Q_OBJECT

public:
  AbstractRawComponent(QWidget *parent = nullptr);
  virtual ~AbstractRawComponent();

  virtual void reset() = 0;

  const auto& inputPorts() const noexcept { return input_ports_; }
  const auto& outputPorts() const noexcept { return output_ports_; }
  auto& inputPorts() noexcept { return input_ports_; }
  auto& outputPorts() noexcept { return output_ports_; }

protected:
  void paintEvent(QPaintEvent *event) override = 0;
  virtual void initPorts() = 0;

protected:
  QMap<QString, ports::Port> input_ports_;
  QMap<QString, ports::Port> output_ports_;
};

/// @brief Abstract class for all components. Do not instantiate.
class AbstractComponent : public QWidget {
  Q_OBJECT

public:
  AbstractComponent(QWidget *parent = nullptr);
  virtual ~AbstractComponent();

  constexpr const QUuid &uuid() const noexcept { return uuid_; }

  /// @brief Returns the size of the grid occupied by the component.
  /// @return The size of the grid occupied by the component.
  virtual QSize gridOccupation() const noexcept = 0;

  /// @brief Returns the raw component. The raw component is the true component
  /// that accepts inputs and displays outputs. The classes that inherit from
  /// AbstractComponent are only wrappers for the raw component.
  /// @return The raw component.
  auto rawComponent() noexcept { return raw_component_; }

  virtual QString componentType() const noexcept = 0;

  const auto& inputPorts() const noexcept { return raw_component_->inputPorts(); }
  const auto& outputPorts() const noexcept { return raw_component_->outputPorts(); }

  void reset() { raw_component_->reset(); }

protected:
  void paintEvent(QPaintEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

signals:
  void componentRemoved(AbstractComponent *component);

public slots:
  // void onUpdateComponent(const QVector<bool>& data);
  void onUpdateGui() { update(); }

private:
  QUuid uuid_;
  QPoint drag_start_position_;
  ComponentTitleBar *title_bar_;

  void initMembers();
  void initLayout();
  void initConnections();

  void askForRemove();

protected:
  AbstractRawComponent *raw_component_;
};

} // namespace rabbit_App::component

#endif // ABSRACT_COMPONENT_H
