#ifndef HELPERS_H
#define HELPERS_H

void logMsg(const char* format, ...) {
  char msgBuff[MAX_LOG_BUFF_SIZE];
  va_list args;
  va_start(args, format);
  vsnprintf(msgBuff, sizeof(msgBuff), format, args);
  va_end(args);

  messageLog += String(msgBuff) + "\n";

  while (messageLog.length() > maxLogSize) {
    int truncateIdx = messageLog.indexOf('\n') + 1;
    if (truncateIdx > 0) {
      messageLog.remove(0, truncateIdx);
    } else {
      messageLog = "";
    }
  }

#ifdef SERIAL_PRINT
  Serial.print(msgBuff);
#endif
}

void printTime() {
  unsigned long epochTime = timeClient.getEpochTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);

#ifdef DEBUG_ON
  logMsg("Current time: %s\n", timeClient.getFormattedTime());
#endif
}

#endif