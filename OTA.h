#ifndef OTA_H
#define OTA_H

void onOTAStart() {
  Serial.println("OTA update started!");
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  if (success) {
    logMsg("OTA update finished successfully!\n");
  } else {
    logMsg("There was an error during OTA update!\n");
  }
}

void setupOTACallbacks() {
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  // ElegantOTA.setID("HolidayLights");
  // ElegantOTA.setFWVersion("1.0.0");
}

#endif // OTA_H
