#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <Arduino.h>
#include <FastLED.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include "Config.h"

String header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

String html_1 = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
  <meta charset='utf-8'>
  <style>
    body    { font-size:120%;} 
    #main   { display: table; width: 300px; margin: auto;  padding: 10px 10px 10px 10px; border: 3px solid blue; border-radius: 10px; text-align:center;} 
    .button { width:200px; height:40px; font-size: 110%;  }
  </style>
  <title>Hodini Testing</title>
</head>
<body>
  <div id='main'>
    <h3>LED CONTROL</h3>
    <div id='content'>
      R: <input id="color_r" type="number" max="255" min="0" /><br />
      G: <input id="color_g" type="number" max="255" min="0" /><br />
      B: <input id="color_b" type="number" max="255" min="0" /><br />
      <button id='BTN_LED'class="button">Send</button>
    </div>
    <br />
   </div>
</body>
 
<script>
  var Socket;
  function init() 
  {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
  }
 
  document.getElementById('BTN_LED').addEventListener('click', buttonClicked);
  function buttonClicked()
  {   
    const buffer = new Uint8Array(3);
    buffer[0] = document.getElementById('color_r').value;
    buffer[1] = document.getElementById('color_g').value;
    buffer[2] = document.getElementById('color_b').value;
    Socket.send(buffer);
  }
 
  window.onload = function(e)
  { 
    init();
  }
</script>
</html>
)=====";

WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

CRGB leds[NUM_LEDS];
uint8_t currentTargetColor[3];

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{
  if ((type == WStype_BIN || type == WStype_TEXT) && length == 3)
  {
    memcpy(currentTargetColor, payload, length);
    // for (int i = 0; i < length; i++)
    // {
    //   Serial.print(payload[i]);
    //   Serial.print(":");
    // }
    // Serial.println();
  }
  else
  {
    Serial.print("WStype = ");
    Serial.println(type);
    Serial.print("WS payload = ");
    for (int i = 0; i < length; i++)
    {
      Serial.print((char)payload[i]);
      Serial.print(":");
    }
    Serial.println();
  }
}

void initLeds()
{
  FastLED.setBrightness(255);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
}

void setup()
{
  initLeds();

  Serial.begin(115200);
  Serial.println();
  Serial.println("Serial started at 115200");
  Serial.println();

  WiFiManager wifiManager;
  // wifiManager.resetSettings();
  wifiManager.autoConnect("HodiniAP");

  Serial.println("");
  Serial.println(F("[CONNECTED]"));
  Serial.print("[IP ");
  Serial.print(WiFi.localIP());
  Serial.println("]");

  // start a server
  server.begin();
  Serial.println("Server started");

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void ledTick()
{
  if(currentTargetColor[0] > leds[0].r) {
    leds[0].r += 1;
  }
  else if(currentTargetColor[0] < leds[0].r) {
    leds[0].r -= 1;
  }

  if(currentTargetColor[1] > leds[0].g) {
    leds[0].g += 1;
  }
  else if(currentTargetColor[1] < leds[0].g) {
    leds[0].g -= 1;
  }

  if(currentTargetColor[2] > leds[0].b) {
    leds[0].b += 1;
  }
  else if(currentTargetColor[2] < leds[0].b) {
    leds[0].b -= 1;
  }

  for (uint8_t i = 1; i < NUM_LEDS; i++)
  {
    leds[i].r = leds[0].r;
    leds[i].g = leds[0].g;
    leds[i].b = leds[0].b;
  }
  FastLED.show();
}

void loop()
{
  webSocket.loop();
  if(millis() % 50 == 0) {
    ledTick();
  }

  WiFiClient client = server.available(); // Check if a client has connected
  if (!client)
  {
    return;
  }

  client.flush();
  client.print(header);
  client.print(html_1);

  delay(5);
}