#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "RGB_LED.h"
#include <HTS221Sensor.h>

#define RGB_LED_BRIGHTNESS 32

// Screen lines
#define HEADER 0
#define LINE_1 1
#define LINE_2 2
#define LINE_3 3

#define send_interval 15000
#define setup_delay 2000

static const float DEFAULT_TEMP = -1000;
static const float DEFAULT_HUMID = -1000;

DevI2C *i2c;
HTS221Sensor *sensor;

float humidity = 0;
float temperature = 0;

static bool hasWifi = false;
static bool hasIoTHub = false;
static bool hasSensors = false;
static RGB_LED rgbLed;

///// Init
static void InitWifi(){
  Screen.print(LINE_1, "Connecting WiFi...");

  if (WiFi.begin() == WL_CONNECTED)
  {
    Screen.print(LINE_2, "Connected...\r\n");   
    hasWifi = true;
    IPAddress ip = WiFi.localIP();
    Screen.print(LINE_3, ip.get_address());
  }
  else
  {
    hasWifi = false;
    Screen.print(LINE_2, "No Wi-Fi");
  }
}

static void InitIotHub(){
  Screen.print(LINE_1, "Connecting To IoTHub...",true);

  if (DevKitMQTTClient_Init())
  {
    Screen.print(LINE_2, "Connected...\r\n");
    hasIoTHub = true;
    // TODO: Register call backs
    DevKitMQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
    return;
  }

}

static void InitSensors(){
  Screen.print(LINE_1, "Init Sensors...",true);

  // Setup sensor
  i2c = new DevI2C(D14, D15);
  sensor = new HTS221Sensor(*i2c);

  // init sensor
  sensor->init(NULL);

  humidity = DEFAULT_HUMID;
  temperature = DEFAULT_TEMP;

  hasSensors = true;
}

//// MQTT Call backs

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result){
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    // TODO: Increment message sent count?
    blinkLED();
  }
}

//// Util

static void SetLEDError(){
  rgbLed.setColor(RGB_LED_BRIGHTNESS,0, 0);
}

static void blinkLED(){
  rgbLed.turnOff();
  rgbLed.setColor(0,0, RGB_LED_BRIGHTNESS);
  delay(500);
  rgbLed.turnOff();
}

float ReadTemperature(){
  sensor->reset();

  float temperature = DEFAULT_TEMP;
  sensor->getTemperature(&temperature);
  return temperature;
}

float ReadHumidity(){
  sensor->reset();

  float humidity = DEFAULT_HUMID;
  sensor->getHumidity(&humidity);
  return humidity;
}

void setup() {
  Screen.print(HEADER, "Setup...");

  hasIoTHub = false;
  hasWifi = false;

  // Check for Wifi connection
  InitWifi();

  if(!hasWifi){
    return;
  }
  
  // Check for Azure connection
  InitIotHub();
  if(!hasIoTHub){
    return;
  }

  // Init Sensors
  InitSensors();
  if(!hasSensors){
    return;
  }

  Serial.print("Sending interval at ");
  Serial.println(send_interval);

  delay(setup_delay);

  Screen.clean();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (hasIoTHub && hasWifi && hasSensors)
  {
    Screen.print(HEADER, "Running...");

    float t = ReadTemperature();
    float h = ReadHumidity();

    char buff[128];

    // Azure IoTHub payload
    snprintf(buff, 128, "{\"topic\":\"iot\",\"Temperature\":%f,\"Humidity\":%f}", t, h);
    
    if (DevKitMQTTClient_SendEvent(buff))
    {  
      // Update Screen with sensor data
      char sensorLine[20];
      sprintf(sensorLine, "T:%.2f H:%.2f", t, h);
      Screen.print(LINE_3, sensorLine);
      
    }
    else
    {
      Screen.print(LINE_1, "Failed to send payload...", true);
      SetLEDError();
    }
  }else{
    Screen.print(LINE_1, "Error...");
    SetLEDError();
  }

  delay(send_interval);
}
