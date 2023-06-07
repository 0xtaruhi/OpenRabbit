#include "Ports/PinInfo.h"
#include "Ports/Port.h"
#include <QHash>

using namespace rabbit_App::ports;

static const QHash<QString, int> input_decl_index_map{
    {"P151", 0},  {"P148", 1},  {"P150", 2},  {"P152", 3},  {"P160", 4},
    {"P161", 5},  {"P162", 6},  {"P163", 7},  {"P164", 8},  {"P165", 9},
    {"P166", 10}, {"P169", 11}, {"P173", 12}, {"P174", 13}, {"P175", 14},
    {"P191", 15}, {"P120", 16}, {"P116", 17}, {"P115", 18}, {"P114", 19},
    {"P113", 20}, {"P112", 21}, {"P111", 22}, {"P108", 23}, {"P102", 24},
    {"P101", 25}, {"P100", 26}, {"P97", 27},  {"P96", 28},  {"P95", 29},
    {"P89", 30},  {"P88", 31},  {"P87", 32},  {"P86", 33},  {"P81", 34},
    {"P75", 35},  {"P74", 36},  {"P70", 37},  {"P69", 38},  {"P68", 39},
    {"P64", 40},  {"P62", 41},  {"P61", 42},  {"P58", 43},  {"P57", 44},
    {"P49", 45},  {"P47", 46},  {"P48", 47},  {"P192", 48}, {"P193", 49},
    {"P199", 50}, {"P200", 51}, {"P201", 52}, {"P202", 53}};

static const QHash<QString, int> output_decl_index_map{
    {"P7", 1},    {"P6", 2},    {"P5", 3},    {"P4", 4},    {"P9", 5},
    {"P8", 6},    {"P16", 6},   {"P15", 7},   {"P11", 8},   {"P10", 9},
    {"P20", 10},  {"P18", 11},  {"P17", 12},  {"P22", 13},  {"P21", 14},
    {"P23", 15},  {"P44", 16},  {"P45", 17},  {"P46", 18},  {"P43", 19},
    {"P40", 20},  {"P41", 21},  {"P42", 22},  {"P33", 23},  {"P34", 24},
    {"P35", 25},  {"P36", 26},  {"P30", 27},  {"P31", 28},  {"P24", 29},
    {"P27", 30},  {"P29", 31},  {"P110", 32}, {"P109", 33}, {"P99", 34},
    {"P98", 35},  {"P94", 36},  {"P93", 37},  {"P84", 38},  {"P83", 39},
    {"P82", 40},  {"P73", 41},  {"P71", 42},  {"P63", 43},  {"P60", 44},
    {"P59", 45},  {"P56", 46},  {"P55", 47},  {"P167", 48}, {"P168", 49},
    {"P176", 50}, {"P187", 51}, {"P189", 52}, {"P194", 53}};

static const QString clock_decl = "";

auto rabbit_App::ports::inputDeclIndexMap(const QString &pin_name) -> int {
  if (auto iter = input_decl_index_map.find(pin_name);
      iter != input_decl_index_map.end()) {
    return iter.value();
  }
  return -1;
}

auto rabbit_App::ports::outputDeclIndexMap(const QString &pin_name) -> int {
  if (auto iter = output_decl_index_map.find(pin_name);
      iter != output_decl_index_map.end()) {
    return iter.value();
  }
  return -1;
}

auto rabbit_App::ports::getClockPinDecl() -> QString { return clock_decl; }

auto rabbit_App::ports::declIndexMap(const QString &pin_name)
    -> QPair<PortType, int> {
  if (pin_name == clock_decl) {
    return {PortType::Clock, 0};
  }
  if (auto iter = input_decl_index_map.find(pin_name);
      iter != input_decl_index_map.end()) {
    return {PortType::Input, iter.value()};
  }
  if (auto iter = output_decl_index_map.find(pin_name);
      iter != output_decl_index_map.end()) {
    return {PortType::Output, iter.value()};
  }
  return {PortType::Undefined, -1};
}
