#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

typedef struct {
  String name;
  unsigned long period = 0;
  unsigned long slept = 0;
  void (*handler)(void);

  void tick(unsigned long elapsedTime) {
    // Check if task has been initialized
    if (period == 0) {
      return;
    }
    // Update task status
    if (slept >= period) {
      slept = 0;
      handler();
    } else {
      slept += elapsedTime;
    }
  }
} task;

String weekdays[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
String months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

#endif