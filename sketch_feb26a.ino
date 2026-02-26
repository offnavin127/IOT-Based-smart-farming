#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11
#define RELAY D5   // Motor control pin

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Vivo T4x 5G";
const char* password = "Navin1535";

WiFiServer server(80);

bool motorState = false;

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);  // Motor OFF initially

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {

  WiFiClient client = server.available();
  if (!client) return;

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int soilValue = analogRead(A0);

  String request = client.readStringUntil('\r');
  client.flush();

  // Manual Motor Control
  if (request.indexOf("/MOTOR=ON") != -1) {
    digitalWrite(RELAY, HIGH);
    motorState = true;
  }
  if (request.indexOf("/MOTOR=OFF") != -1) {
    digitalWrite(RELAY, LOW);
    motorState = false;
  }

  // Webpage
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><meta http-equiv='refresh' content='5'></head>");
  client.println("<body style='font-family:Arial;text-align:center;'>");
  client.println("<h1>IOT Based Smart Farming</h1>");
  client.println("<p>Temperature: " + String(t) + " C</p>");
  client.println("<p>Humidity: " + String(h) + " %</p>");
  client.println("<p>Soil Moisture: " + String(soilValue) + "</p>");

  if (motorState) {
    client.println("<p><b>Motor Status: ON</b></p>");
  } else {
    client.println("<p><b>Motor Status: OFF</b></p>");
  }

  client.println("<br>");
  client.println("<a href=\"/MOTOR=ON\"><button style='padding:10px 20px;'>Motor ON</button></a>");
  client.println("<a href=\"/MOTOR=OFF\"><button style='padding:10px 20px;'>Motor OFF</button></a>");
  client.println("</body></html>");

  delay(1);
}