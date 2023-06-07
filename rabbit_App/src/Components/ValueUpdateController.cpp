#include <QApplication>

#include "Components/Components.h"
#include "Components/ComponentsPanel.h"
#include "Components/ValueUpdateController.h"

using namespace rabbit_App::component;

ValueUpdateController::ValueUpdateController(ComponentsPanel *components_panel,
                                             QObject *parent)
    : QObject(parent), components_panel_(components_panel) {
  update_thread_ = new QThread();
  update_timer_ = new QTimer(this);
  update_timer_->setInterval(1000 / 60);
  connect(update_timer_, &QTimer::timeout, this,
          &ValueUpdateController::process);
}

ValueUpdateController::~ValueUpdateController() {}

void ValueUpdateController::onStartUpdate() {
  update_thread_->start();
  update_timer_->start();
  this->moveToThread(update_thread_);
}

void ValueUpdateController::onStopUpdate() {
  this->moveToThread(QApplication::instance()->thread());
  update_timer_->stop();
  update_thread_->quit();
}

void ValueUpdateController::onReceiveData(const std::vector<uint16_t> &data) {
  assert(data.size() % 4 == 0);

  for (size_t i = 0; i != data.size(); i += 4) {
    uint64_t value = 0;
    for (size_t j = 0; j != 4; ++j) {
      value |= (uint64_t(data[i + j]) << (j * 16));
    }
    read_queue_.enqueue(value);
  }
}

void ValueUpdateController::process() {
  while (!read_queue_.isEmpty()) {
    uint64_t value = read_queue_.dequeue();
    for (auto component : components_panel_->components_) {
      if (auto led_component = dynamic_cast<LEDComponent *>(component)) {
        static_cast<LEDRawComponent *>(led_component->rawComponent())
            ->onSwitchLED((value >> 2) & 0x1);
      } else if (auto text_lcd_component =
                     dynamic_cast<TextLCDComponent *>(component)) {
        auto raw = static_cast<TextLCDRawComponent *>(
            text_lcd_component->rawComponent());

        auto en = (value >> 2) & 0x1;
        if (en == 0) {
          raw->setChar(static_cast<quint8>((value >> 3) & 0xff));
        }
      }
    }
  }
}
