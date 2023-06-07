#include <QGradient>
#include <QLayout>
#include <QPainter>
#include <QRadialGradient>
#include <QSize>
#include <QThreadPool>

#include "Components/AbstractComponent.h"
#include "Components/LEDComponent.h"
#include "qpainter.h"

using namespace rabbit_App::component;

class LEDComponentUpdater : public QRunnable {
public:
  LEDComponentUpdater(LEDRawComponent *led_component,
                      const QVector<bool> &input_data)
      : led_component_(led_component), input_data_(input_data) {}

private:  
  LEDRawComponent* led_component_;
  QVector<bool> input_data_;

  void run() override {
    led_component_->onSwitchLED(input_data_[2]);
  }
};

LEDRawComponent::LEDRawComponent(QWidget *parent)
    : AbstractRawComponent(parent) {
  initPorts();

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  led_picture_ = new QLabel(this);
  led_picture_->setPixmap(QPixmap(":/icons/icons/components/LED-icon.png"));

  led_picture_->setScaledContents(true);
  led_picture_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  auto layout = new QHBoxLayout(this);
  layout->addWidget(led_picture_);
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);
}

LEDRawComponent::~LEDRawComponent() {}

void LEDRawComponent::reset() {
  is_on_ = false;
  update();
}

void LEDRawComponent::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  QRadialGradient gradient(this->rect().center(), 20);
  gradient.setColorAt(0, is_on_ ? Qt::red : Qt::transparent);
  gradient.setColorAt(1, Qt::transparent);
  painter.setBrush(gradient);
  painter.setPen(Qt::transparent);
  painter.drawEllipse(this->rect());
}

void LEDRawComponent::initPorts() {
  auto out_port = ports::Port();
  out_port.name = "out";
  out_port.type = ports::PortType::Output;
  output_ports_[out_port.name] = out_port;
}

void LEDRawComponent::onSwitchLED(bool is_on) {
  is_on_ = is_on;
}

LEDComponent::LEDComponent(QWidget *parent) : AbstractComponent(parent) {
  raw_component_ = new LEDRawComponent(this);
  layout()->addWidget(raw_component_);
}

LEDComponent::~LEDComponent() {}

QSize LEDComponent::gridOccupation() const noexcept { return QSize(1, 1); }

void LEDComponent::contextMenuEvent(QContextMenuEvent *event) {
  AbstractComponent::contextMenuEvent(event);
}