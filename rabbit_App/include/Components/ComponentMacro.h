#pragma once
#ifndef COMPONENT_MACRO_H
#define COMPONENT_MACRO_h

#define COMPONENT_CLASS_DECLARATION(component_name)                            \
  class component_name##Component : public AbstractComponent {                 \
    Q_OBJECT                                                                   \
  public:                                                                      \
    component_name##Component(QWidget *parent = nullptr);                      \
    virtual ~component_name##Component();                                      \
                                                                               \
    QSize gridOccupation() const noexcept override;                            \
    QString componentType() const noexcept override {                          \
      return QString(#component_name);                                         \
    }                                                                          \
    static QIcon componentIcon() noexcept {                                    \
      return QIcon(":/icons/icons/components/" #component_name "-icon.png");   \
    }                                                                          \
                                                                               \
  protected:                                                                   \
    void contextMenuEvent(QContextMenuEvent *event) override;                  \
  };

#endif // COMPONENT_MACRO_H
