#include <QLayout>
#include <QPainter>
#include <QPalette>

#include "Common.h"
#include "Components/TextLCDComponent.h"

using namespace rabbit_App;
using namespace rabbit_App::component;

CharBlock::CharBlock(QWidget *parent) : QWidget(parent) {
  this->setFixedSize(15, 20);
}

CharBlock::~CharBlock() {}

void CharBlock::setCharCode(const quint8 &char_code) {
  char_code_ = char_code;
  this->update();
}

void CharBlock::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  // painter.setBrush(Qt::transparent);
  painter.setPen(color::ruri);
  painter.setFont(QFont("Arial", 13));
  painter.drawText(0, 0, 20, 20, Qt::AlignCenter,
                   QChar(uchar(char_code_ + 0x20)));
}

TextLCDRawComponent::TextLCDRawComponent(QWidget *parent)
    : AbstractRawComponent(parent) {
  auto layout = new QVBoxLayout(this);
  auto upper_layout = new QHBoxLayout(this);
  auto lower_layout = new QHBoxLayout(this);

  {
    int i = 0;
    CharBlock *char_block;
    while (i != 16) {
      char_block = new CharBlock(this);
      char_blocks_.push_back(char_block);
      upper_layout->addWidget(char_block);
      ++i;
    }

    while (i != 32) {
      char_block = new CharBlock(this);
      char_blocks_.push_back(char_block);
      lower_layout->addWidget(char_block);
      ++i;
    }
  }

  char_blocks_[2]->setCharCode(0x05);

  layout->addLayout(upper_layout);
  layout->addLayout(lower_layout);
  this->setLayout(layout);
}

TextLCDRawComponent::~TextLCDRawComponent() {}

void TextLCDRawComponent::reset() {
  for (auto &char_block : char_blocks_) {
    char_block->setCharCode(0x00);
  }
  cur_pos_ = 0;
}

void TextLCDRawComponent::setChar(const quint8 &char_code,
                                  const quint8 &char_pos) {
  char_blocks_[char_pos]->setCharCode(char_code);
}

void TextLCDRawComponent::setChar(const quint8 &char_code) {
  char_blocks_[cur_pos_]->setCharCode(char_code);
  moveCursorNext();
}

void TextLCDRawComponent::moveCursorNext() { cur_pos_ = (cur_pos_ + 1) % 32; }

void TextLCDRawComponent::setCursorPos(const int &pos) {
  assert(pos >= 0 && pos < 32);
  cur_pos_ = pos;
}

void TextLCDRawComponent::moveCursorHome() { cur_pos_ = 0; }

void TextLCDRawComponent::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setPen(Qt::gray);
  painter.setBrush(color::wasurenagusa);
  painter.drawRect(rect());
}

void TextLCDRawComponent::initPorts() {}

TextLCDComponent::TextLCDComponent(QWidget *parent)
    : AbstractComponent(parent) {
  raw_component_ = new TextLCDRawComponent(this);
  layout()->addWidget(raw_component_);
}

TextLCDComponent::~TextLCDComponent() {}

QSize TextLCDComponent::gridOccupation() const noexcept { return QSize(4, 1); }

void TextLCDComponent::contextMenuEvent(QContextMenuEvent *event) {
  AbstractComponent::contextMenuEvent(event);
}
