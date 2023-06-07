#include <QLayout>
#include <QMouseEvent>

#include "Components/AbstractComponent.h"
#include "Components/SwitchComponent.h"

using namespace rabbit_App::component;

SwitchRawComponent::SwitchRawComponent(QWidget *parent)
    : AbstractRawComponent(parent) {
  switch_picture_ = &SwitchRawComponent::switchOffPicture();
  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  switch_picture_label_ = new QLabel(this);
  switch_picture_label_->setPixmap(*switch_picture_);
  switch_picture_label_->setScaledContents(true);
  switch_picture_label_->setSizePolicy(QSizePolicy::Ignored,
                                       QSizePolicy::Ignored);
  layout->addWidget(switch_picture_label_);

  setLayout(layout);

  initConnections();
}

SwitchRawComponent::~SwitchRawComponent() {}

void SwitchRawComponent::reset() {}

void SwitchRawComponent::initConnections() {
  connect(this, &SwitchRawComponent::switchToggled, this,
          &SwitchRawComponent::onSwitchToggled);
}

void SwitchRawComponent::paintEvent(QPaintEvent *event) {}

void SwitchRawComponent::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit switchToggled();
  }
}

void SwitchRawComponent::initPorts() {}

void SwitchRawComponent::onSwitchToggled() {
  is_on_ = !is_on_;
  switch_picture_ = is_on_ ? &SwitchRawComponent::switchOffPicture()
                           : &SwitchRawComponent::switchOnPicture();
  switch_picture_label_->setPixmap(*switch_picture_);
  update();
}

SwitchComponent::SwitchComponent(QWidget *parent) : AbstractComponent(parent) {
  raw_component_ = new SwitchRawComponent(this);
  layout()->addWidget(raw_component_);
}

SwitchComponent::~SwitchComponent() {}

QSize SwitchComponent::gridOccupation() const noexcept { return QSize(1, 1); }

void SwitchComponent::contextMenuEvent(QContextMenuEvent *event) {
  AbstractComponent::contextMenuEvent(event);
}