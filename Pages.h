#ifndef PAGES_H
#define PAGES_H

#include "Constants.h"
#include "Helpers.h"

ESP8266WebServer server(80);

void handleMainPage() {
  String scheduleState = schedActive_ ? "ACTIVE" : "INACTIVE";
  String scheduleStateClass = schedActive_ ? "active" : "";
  String relayOneStateClass = relayOneActive_ ? " active" : "";
  String relayTwoStateClass = relayTwoActive_ ? " active" : "";
  String matrixStateClass = showMatrix_ ? " active" : "";
  String scheduleStartTime = formatTime(TIME_ON);
  String scheduleEndTime = formatTime(TIME_OFF);
  String scheduleStartAngle = String((TIME_ON % 1440) * 0.25, 1);
  String sunriseAngle = String((TIME_SUNRISE % 1440) * 0.25, 1);
  String sunsetAngle = String((TIME_SUNSET % 1440) * 0.25, 1);
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentTime = currentHour * 60 + currentMinute;
  String hourHandAngle = String(currentTime * 0.25, 1);
  String minuteHandAngle = String(currentMinute * 6.0, 1);
  String minuteHandColor = schedActive_ ? "#C07D4D" : "rgba(185,185,185,.8)";
  const int transitionMinutes = 24;
  const int dayRed = 47;
  const int dayGreen = 129;
  const int dayBlue = 212;
  const int nightRed = 106;
  const int nightGreen = 91;
  const int nightBlue = 166;
  int handRed = nightRed;
  int handGreen = nightGreen;
  int handBlue = nightBlue;
  int sunriseStart = (TIME_SUNRISE - transitionMinutes + 1440) % 1440;
  int sunsetStart = (TIME_SUNSET - transitionMinutes + 1440) % 1440;
  int sunriseElapsed = (currentTime - sunriseStart + 1440) % 1440;
  int sunsetElapsed = (currentTime - sunsetStart + 1440) % 1440;

  if (sunriseElapsed < transitionMinutes * 2) {
    float blend = sunriseElapsed / (float)(transitionMinutes * 2);
    handRed = nightRed + (dayRed - nightRed) * blend;
    handGreen = nightGreen + (dayGreen - nightGreen) * blend;
    handBlue = nightBlue + (dayBlue - nightBlue) * blend;
  } else if (sunsetElapsed < transitionMinutes * 2) {
    float blend = 1.0 - sunsetElapsed / (float)(transitionMinutes * 2);
    handRed = nightRed + (dayRed - nightRed) * blend;
    handGreen = nightGreen + (dayGreen - nightGreen) * blend;
    handBlue = nightBlue + (dayBlue - nightBlue) * blend;
  } else {
    int dayStart = (TIME_SUNRISE + transitionMinutes) % 1440;
    int dayEnd = (TIME_SUNSET - transitionMinutes + 1440) % 1440;
    bool isDay = dayStart < dayEnd
                     ? currentTime >= dayStart && currentTime < dayEnd
                     : currentTime >= dayStart || currentTime < dayEnd;
    if (isDay) {
      handRed = dayRed;
      handGreen = dayGreen;
      handBlue = dayBlue;
    }
  }
  String dayColor = "rgba(" + String(dayRed) + "," + String(dayGreen) + "," + String(dayBlue) + ",.55)";
  String nightColor = "rgba(" + String(nightRed) + "," + String(nightGreen) + "," + String(nightBlue) + ",.55)";
  String hourHandColor = "rgba(" + String(handRed) + "," + String(handGreen) + "," + String(handBlue) + ",.55)";
  int scheduleDuration = TIME_OFF - TIME_ON;
  if (scheduleDuration <= 0) scheduleDuration += 1440;
  float scheduleSweepDegrees = scheduleDuration * 0.25;
  if (scheduleSweepDegrees > 360.0) scheduleSweepDegrees = 360.0;
  String scheduleSweep = String(scheduleSweepDegrees, 1);

  String htmlResponse =
        "<html><head>"
        "<title>Holiday Light Control</title>"
        "<style>"
        "body { --panel-diameter: min(72vw, 90vh, 320px); font-family: Arial, sans-serif; background-color: #1A1A1A; color: white; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }"
        ".container { position: relative; z-index: 1; display: flex; flex-direction: column; align-items: center; justify-content: center; box-sizing: border-box; width: min(90vw, 360px); text-align: center; }"
        ".container h1, .schedule-text { box-sizing: border-box; max-width: 100%; padding: 2px 6px; background: #1a1a1ada; border-radius: 8px; }"
        ".container h1 { margin: 0 0 8px; }"
        ".switch { position: relative; display: inline-block; width: 60px; height: 34px; margin:20px 0;}"
        ".switch input { opacity: 0; width: 0; height: 0; }"
        ".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #B0B0B0; transition: .4s; border-radius: 34px; }"
        ".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }"
        "input:checked + .slider { background-color: #5a2d92; }"
        "input:checked + .slider:before { transform: translateX(26px); }"
        ".button { background-color: purple; color: white; border: none; border-radius: 5px; padding: 10px 20px; cursor: pointer; font-size: 16px; width: 100%; }"
        ".button:hover { background-color: #5a2d92; }"
        ".clock-face { --panel-diameter: min(72vw, 90vh, 320px); position: fixed; z-index: 0; left: 50%; top: 50%; transform: translate(-50%, -50%); width: min(100vw, 100vh, max(70vmin, calc(var(--panel-diameter) + 72px))); height: min(100vw, 100vh, max(70vmin, calc(var(--panel-diameter) + 72px))); border-radius: 50%; pointer-events: none; opacity: .7; }"
        ".clock-face:before { content: ''; position: absolute; inset: 4%; border: 2px solid rgba(255,255,255,.16); border-radius: 50%; }"
        ".clock-ticks, .day-night-band, .schedule-arc, .schedule-marks { position: absolute; inset: 0; border-radius: 50%; }"
        ".clock-ticks { -webkit-mask: radial-gradient(circle closest-side, transparent 0 89%, #000 89.5% 91%, transparent 91.5%); mask: radial-gradient(circle closest-side, transparent 0 89%, #000 89.5% 91%, transparent 91.5%); }"
        ".clock-ticks { background: repeating-conic-gradient(from -0.225deg, rgba(255,255,255,.32) 0deg .45deg, transparent .45deg 15deg); }"
        ".day-night-band { background: conic-gradient(from 0deg, var(--night-color) 0deg, var(--night-color) calc(var(--sunrise-angle) - 6deg), var(--day-color) calc(var(--sunrise-angle) + 6deg), var(--day-color) calc(var(--sunset-angle) - 6deg), var(--night-color) calc(var(--sunset-angle) + 6deg), var(--night-color) 360deg); -webkit-mask: radial-gradient(circle closest-side, transparent 0 84%, #000 84.5% 86%, transparent 86.5%); mask: radial-gradient(circle closest-side, transparent 0 84%, #000 84.5% 86%, transparent 86.5%); }"
        ".schedule-arc { background: conic-gradient(from var(--schedule-start), #C07D4D 0deg var(--schedule-sweep), transparent var(--schedule-sweep) 360deg); -webkit-mask: radial-gradient(circle closest-side, transparent 0 91.5%, #000 91.7% 92.5%, transparent 92.7%); mask: radial-gradient(circle closest-side, transparent 0 91.5%, #000 91.7% 92.5%, transparent 92.7%); }"
        ".schedule-marks { background: conic-gradient(from calc(var(--schedule-start) - .35deg), #C07D4D 0deg .7deg, transparent .7deg var(--schedule-sweep), #C07D4D var(--schedule-sweep) calc(var(--schedule-sweep) + .7deg), transparent calc(var(--schedule-sweep) + .7deg) 360deg); -webkit-mask: radial-gradient(circle closest-side, transparent 0 87%, #000 88% 94%, transparent 95%); mask: radial-gradient(circle closest-side, transparent 0 87%, #000 88% 94%, transparent 95%); }"
        ".clock-label { position: absolute; z-index: 2; color: rgba(255,255,255,.65); font-size: 11px; font-variant-numeric: tabular-nums; transform: translate(-50%, -50%); }"
        ".label-00 { left: 50%; top: 1%; } .label-03 { left: 84%; top: 16%; } .label-06 { left: 99%; top: 50%; } .label-09 { left: 84%; top: 84%; }"
        ".label-12 { left: 50%; top: 99%; } .label-15 { left: 16%; top: 84%; } .label-18 { left: 1%; top: 50%; } .label-21 { left: 16%; top: 16%; }"
        ".clock-links { position: fixed; z-index: 2; left: 50%; top: 50%; transform: translate(-50%, -50%); width: min(100vw, 100vh, max(70vmin, calc(var(--panel-diameter) + 72px))); height: min(100vw, 100vh, max(70vmin, calc(var(--panel-diameter) + 72px))); pointer-events: none; }"
        ".clock-link { position: absolute; display: flex; align-items: center; justify-content: center; width: 32px; height: 32px; box-sizing: border-box; border: 1px solid rgba(255,255,255,.35); border-radius: 50%; background: #1A1A1A; color: #A0A0A0; text-decoration: none; pointer-events: auto; transform: translate(-50%, -50%); }"
        ".clock-link:hover, .clock-link:focus-visible, .clock-link:active { background: #5a2d92; color: white; outline: 2px solid white; outline-offset: 2px; }"
        ".clock-link svg { position: absolute; left: 50%; top: 50%; display: block; width: 17px; height: 17px; transform: translate(-50%, -50%); }"
        ".clock-link-update { left: 74.5%; top: 92.4%; } .clock-link-log { left: 62.7%; top: 97.3%; }"
        ".clock-link-relay-one { left: 92.4%; top: 25.5%; } .clock-link-relay-two { left: 97.3%; top: 37.3%; }"
        ".clock-link-matrix { left: 74.5%; top: 7.6%; }"
        ".clock-link.active { background: #5a2d92; color: white; }"
        ".clock-hand { position: absolute; z-index: 1; left: 50%; bottom: 50%; transform-origin: 50% 100%; border-radius: 4px; }"
        ".hour-hand { width: 6px; height: calc(var(--panel-diameter) / 2 + 20px); background: var(--hour-color); transform: translateX(-50%) rotate(var(--hour-angle)); }"
        ".minute-hand { width: 3px; height: calc(var(--panel-diameter) / 2 + 67px); background: var(--minute-color); transform: translateX(-50%) rotate(var(--minute-angle)); }"
        ".clock-pin { position: absolute; z-index: 2; left: 50%; top: 50%; width: 10px; height: 10px; border-radius: 50%; background: #a56de2; transform: translate(-50%, -50%); }"
        ".schedule-text { color: #888; font-size: 14px; margin-top: 6px; }"
        ".schedule-text .active { color: #a56de2; }"
        "</style>"
        "</head><body>"
        "<div class=\"clock-face\" aria-hidden=\"true\" style=\"--hour-angle: " + hourHandAngle + "deg; --minute-angle: " + minuteHandAngle + "deg; --hour-color: " + hourHandColor + "; --minute-color: " + minuteHandColor + "; --day-color: " + dayColor + "; --night-color: " + nightColor + ";\"><span class=\"clock-ticks\"></span><span class=\"day-night-band\" style=\"--sunrise-angle: "
        + sunriseAngle + "deg; --sunset-angle: " + sunsetAngle + "deg;\"></span><span class=\"clock-label label-00\">00</span><span class=\"clock-label label-03\">03</span><span class=\"clock-label label-06\">06</span><span class=\"clock-label label-09\">09</span><span class=\"clock-label label-12\">12</span><span class=\"clock-label label-15\">15</span><span class=\"clock-label label-18\">18</span><span class=\"clock-label label-21\">21</span><span class=\"schedule-arc\" style=\"--schedule-start: "
        + scheduleStartAngle + "deg; --schedule-sweep: " + scheduleSweep
        + "deg;\"></span><span class=\"schedule-marks\" style=\"--schedule-start: "
        + scheduleStartAngle + "deg; --schedule-sweep: " + scheduleSweep
        + "deg;\"></span><span class=\"clock-hand hour-hand\"></span><span class=\"clock-hand minute-hand\"></span><span class=\"clock-pin\"></span></div>"
        "<nav class=\"clock-links\" aria-label=\"Device pages\">"
        "<a class=\"clock-link clock-link-update\" href=\"/update\" aria-label=\"Firmware Update\" title=\"Firmware Update\">"
        "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" aria-hidden=\"true\"><path d=\"M12 15V3m0 0L7 8m5-5 5 5M5 14v5a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2v-5\"/></svg>"
        "</a>"
        "<a class=\"clock-link clock-link-log\" href=\"/log\" aria-label=\"Device Log\" title=\"Device Log\">"
        "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" aria-hidden=\"true\"><path d=\"M8 6h13M8 12h13M8 18h13M3 6h.01M3 12h.01M3 18h.01\"/></svg>"
        "</a>"
        "<a class=\"clock-link clock-link-relay-one" + relayOneStateClass + "\" href=\""
        + String(relayOneActive_ ? "/relay1/off" : "/relay1/on")
        + "\" aria-label=\"Relay 1\" title=\"Relay 1\">1</a>"
        + "<a class=\"clock-link clock-link-relay-two" + relayTwoStateClass + "\" href=\""
        + String(relayTwoActive_ ? "/relay2/off" : "/relay2/on")
        + "\" aria-label=\"Relay 2\" title=\"Relay 2\">2</a>"
        + "<a class=\"clock-link clock-link-matrix" + matrixStateClass + "\" href=\""
        + String(showMatrix_ ? "/matrix/off" : "/matrix/on")
        + "\" aria-label=\"LED Matrix\" title=\"LED Matrix\">"
        "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" aria-hidden=\"true\"><rect x=\"4\" y=\"4\" width=\"6\" height=\"6\"/><rect x=\"14\" y=\"4\" width=\"6\" height=\"6\"/><rect x=\"4\" y=\"14\" width=\"6\" height=\"6\"/><rect x=\"14\" y=\"14\" width=\"6\" height=\"6\"/></svg>"
        "</a>"
        "</nav>"
        "<div class=\"container\">"
        "<h1>LED Control</h1>"
        "<label class=\"switch\"><input type=\"checkbox\" id=\"toggleBtn\" onchange=\"toggleLED()\">"
        "<span class=\"slider\"></span></label>"
        "<div class=\"schedule-text\">Schedule is <span class=\""
        + scheduleStateClass + "\">" + scheduleState + "</span></div>"
        "<div class=\"schedule-text\">Active between "
        + scheduleStartTime + " and " + scheduleEndTime + "</div>"
        "<script>"
        "function toggleLED() {"
        "var toggle = document.getElementById('toggleBtn');"
        "if (toggle.checked) {"
        "location.href='/on';"
        "} else {"
        "location.href='/off';"
        "}"
        "}"
        "</script>"
        "</div>"
        "</body></html>";

  if (showLeds_) {
    htmlResponse.replace("id=\"toggleBtn\"", "id=\"toggleBtn\" checked");
  }
  server.send(200, "text/html", htmlResponse);
}

void handleLogPage() {
  String html = "<html><head>"
                "<title>Message Log</title>"
                "<meta http-equiv='refresh' content='10'>"
                "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<style>"
                "body { font-family: Arial, sans-serif; background-color: #1A1A1A; color: white; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; overflow: hidden; }"
                ".container { display: flex; flex-direction: column; text-align: center; width: 90vw; max-width: 800px; height: 80vh; }"
                ".container h1 { flex: 0 0 auto; }"
                "pre { box-sizing: border-box; flex: 1 1 auto; min-height: 0; overflow-y: auto; white-space: pre-wrap; overflow-wrap: anywhere; border-left: 5px solid purple; padding: 10px; margin: 0; text-align: left; }"
                "</style>"
                "</head><body>"
                "<div class=\"container\">"
                "<h1>Message Log</h1><pre>"
                + messageLog + "</pre>"
                               "</div>"
                               "</body></html>";

  server.send(200, "text/html", html);
}

#endif