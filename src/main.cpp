#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include "auth.h"
#include "AHT25.h"
// https://qiita.com/mine820/items/01d5b7dbf65296c6f6e8
// https://github.com/middle-river/weather_station

WebServer server(80); // ポート80番
#define PIN_SDA 21
#define PIN_SCL 22
String HTML_text = "";
unsigned long t = 0;
//  HTMLを組み立てる
String html1 = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>環境情報</title></head><body><p><h1>";
String html2 = "</h1></p><p><a href=\"/\"><h1>Reload</h1></a></p></body></html>";

// HTMLを構成
void handleHTML()
{
  String html = html1 + HTML_text + html2;
  server.send(200, "text/html", html);
}

// 存在しないアドレスが指定された時の処理
void handleNotFound(void)
{
  server.send(404, "text/plain", "Not Found.");
}

// 初期化
void setup()
{
  // シリアルポートの初期化
  Serial.begin(115200);

  // WiFiのアクセスポイントに接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  // ESP32のIPアドレスを出力
  Serial.println("\nWiFi Connected.");
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());

  // このアドレスにアクセスしたときに処理される関数を指定
  server.on("/", handleHTML);

  server.onNotFound(handleNotFound);
  // Webサーバーを起動
  server.begin();
}
String getTemp()
{
  AHT25 sensor(PIN_SDA, PIN_SCL);
  // Seosor Out
  float t = 22;
  float h = 51;
  float p = 1000; // Not Used
  sensor.begin();
  sensor.get(t, h, p);
  String text = "Temperature: " + String(t) + " C <br>" + "Humidity: " + String(h) + " %Rh";
  Serial.println("Temperature: " + String(t) + " C");
  Serial.println("Humidity: " + String(h) + " %");

  return text;
}

void loop()
{
  server.handleClient();

  if (millis() - t > 3000)
  {
    Serial.print("IP = ");
    Serial.println(WiFi.localIP());
    HTML_text = getTemp();
    t = millis();
  }
}