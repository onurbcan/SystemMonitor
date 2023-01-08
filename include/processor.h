#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long cachActTicks{0};
  long cachIdlTicks{0};
};

#endif