
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <PubSubClient.h>
#include <ArduinoJson.h>


//OLED
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#ifndef APSSID
#define APSSID "HSU_Students" // existing Wifi network
#define APPSK  "dhhs12cnvch"
// #define APSSID "DoanTin"  // existing Wifi network
// #define APPSK "doantin135"
#endif

/* Set these to your desired credentials. */
const char* ssid = APSSID;
const char* password = APPSK;
ESP8266WebServer server(80);
MDNSResponder mdns;

// Thông tin về MQTT Broker
#define mqtt_server "broker.emqx.io"
const uint16_t mqtt_port = 1883;  //Port của MQTT broker
#define mqtt_topic_pub "tin/led"
#define mqtt_topic_sub "tin/led"
WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonDocument<256> doc;  //PubSubClient limits the message size to 256 bytes (includingheader)
char dhtstatus[32] = "on";

//#define DHTPIN D5     // Digital pin connected to the DHT sensor
const int DHTPIN = D5;  //Chân Out của cảm biến nối chân số 5 Arduino
//stutus OLED

String data;
// Uncomment the type of sensor in use:
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  
  Serial.begin(115200);
  // hàm thực hiện chức năng kết nối Wifi và in ra địa chỉ IP của ESP8266
  setup_wifi();
  // cài đặt server eclispe mosquitto / mqttx và lắng nghe client ở port 1883
  client.setServer(mqtt_server, mqtt_port);
  // gọi hàm callback để thực hiện các chức năng publish/subcribe
  client.setCallback(callback);
  // gọi hàm reconnect() để thực hiện kết nối lại với server khi bị mất kết nối
  reconnect();
  // Bat tat OLED
  
 
  server.onNotFound(handleNotFound);

  server.enableCORS(true);

  server.begin();
  Serial.println("HTTP server started");

  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

//MQTT
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // kết nối đến mạng Wifi
  WiFi.begin(ssid, password);
  // in ra dấu . nếu chưa kết nối được đến mạng Wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // in ra thông báo đã kết nối và địa chỉ IP của ESP8266
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void callback(char* topic, byte* payload, unsigned int length) {

  //in ra tên của topic và nội dung nhận được từ kênh MQTT lens đã publish
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  // kiểm tra nếu dữ liệu nhận được từ topic wemos/led/status là chuỗi "on"
  // sẽ bậtled D4, nếu là chuỗi "off" sẽ tắt led D4
  
  Serial.println();
}
void reconnect() {
  // lặp cho đến khi được kết nối trở lại
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // hàm connect có đối số thứ 1 là id đại diện cho mqtt client phải duy nhất,
    // đối số thứ 2 là username và đối số thứ 3 là password nếu có
    if (client.connect("tin_thcntt3")) {
      Serial.println("connected");
      // Đọc nhiệt độ và độ ẩm từ cảm biến DHT11
      

      // Kiểm tra xem việc đọc dữ liệu từ cảm biến có thành công không
      
      
      // Tạo một JSON object để chứa dữ liệu nhiệt độ và độ ẩm
      StaticJsonDocument<200> doc;
       char buffer[256];
      size_t n = serializeJson(doc, buffer);
      
      // float t = dht.readTemperature();
      // float h = dht.readHumidity();
      //  doc["id"] = "1";
      // doc["device"] = "dht11";
      // doc["temperature"] = t;
      // doc["humidity"] = h;
      n=serializeJson(doc, buffer);
      client.publish(mqtt_topic_pub, buffer, n);
      // Chuyển đổi JSON object thành chuỗi và gửi lên MQTT
     

      // Đăng ký nhận gói tin tại topic esp/test
      client.subscribe(mqtt_topic_sub);
    } else {
      // in ra màn hình trạng thái của client khi không kết nối được với MQTT broker
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // delay 5s trước khi thử lại
      delay(5000);
    }
  }
}

void loop() {
  delay(5000);
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  
  // Create a JSON object
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["temperature"] = t;
  jsonDoc["humidity"] = h;

  // Convert JSON object to string
  char buffer[256];
  size_t n = serializeJson(jsonDoc, buffer);
  
  // Publish JSON string
  client.publish(mqtt_topic_pub, buffer, n);

  // clear display
  display.clearDisplay();

  // display temperature
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setCursor(90, 0);
  display.print("Status: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  // display humidity
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(" %");
  //bat tat DHT11
  server.handleClient();

  delay(500);
  display.display();
}





void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}