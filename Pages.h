#ifndef PAGES_H
#define PAGES_H

#include "Constants.h"

String messageLog;
ESP8266WebServer server(80);

void handleMainPage() {
  String htmlResponse = 
        "<html><head>"
        "<title>Holiday Light Control</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; background-color: #1A1A1A; color: white; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }"
        ".container { text-align: center; width: 30%; }"
        ".switch { position: relative; display: inline-block; width: 60px; height: 34px; }"
        ".switch input { opacity: 0; width: 0; height: 0; }"
        ".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #B0B0B0; transition: .4s; border-radius: 34px; }"
        ".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }"
        "input:checked + .slider { background-color: #5a2d92; }"
        "input:checked + .slider:before { transform: translateX(26px); }"
        ".button { background-color: purple; color: white; border: none; border-radius: 5px; padding: 10px 20px; cursor: pointer; font-size: 16px; width: 100%; }"
        ".button:hover { background-color: #5a2d92; }"
        "</style>"
        "</head><body>"
        "<div class=\"container\">"
        "<h1>LED Control</h1>"
        "<label class=\"switch\"><input type=\"checkbox\" id=\"toggleBtn\" onchange=\"toggleLED()\">"
        "<span class=\"slider\"></span></label>"
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
                "<style>"
                "body { font-family: Arial, sans-serif; background-color: #1A1A1A; color: white; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }"
                ".container { text-align: center; width: 50%; }"
                "pre { white-space: pre-wrap; word-wrap: break-word; border-left: 5px solid purple; padding-left: 10px; }"
                "max-height: 300px; overflow-y: auto; padding-left: 5px; margin-left: 0; text-align: left; }"
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