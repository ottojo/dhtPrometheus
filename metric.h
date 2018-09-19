#ifndef METRIC_H
#define METRIC_H

#include "Arduino.h"

enum MetricType : short {
  untyped = 0, counter, gauge, histogram, summary
};

class Metric {
  private:
    String name;
    String helpText;
    MetricType type;
    float value;
    int precision;
  public:
    String getString();
    Metric(MetricType type, String name, String helpText, int precision);
    void setValue(float val) {
      this->value = val;
    }
};

#endif
