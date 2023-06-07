#ifndef VALUE_UPDATE_CONTROLLER_H
#define VALUE_UPDATE_CONTROLLER_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QTimer>

namespace rabbit_App::component {

class ComponentsPanel;
class ValueUpdateController final : public QObject {

  Q_OBJECT
public:
  ValueUpdateController(ComponentsPanel *components_panel,
                        QObject *parent = nullptr);
  virtual ~ValueUpdateController();

public slots:
  void onStartUpdate();
  void onStopUpdate();
  void onReceiveData(const std::vector<uint16_t> &data);

private slots:
  void process();

private:
  QTimer* update_timer_;
  ComponentsPanel* components_panel_;
  QThread *update_thread_;
  QQueue<uint64_t> read_queue_;
};

} // namespace rabbit_App::component

#endif // VALUE_UPDATE_CONTROLLER_H
