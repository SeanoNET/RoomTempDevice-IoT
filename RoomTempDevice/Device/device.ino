#include "AZ3166WiFi.h"
#include "DevKitMQTTClient.h"
#include "RGB_LED.h"

#define RGB_LED_BRIGHTNESS 32

// Screen lines
#define HEADER 0
#define LINE_1 1
#define LINE_2 2
#define LINE_3 3

#define send_interval 15000
#define setup_delay 2000

static bool hasWifi = false;
static bool hasIoTHub = false;
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


//// MQTT Call backs

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result){
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    // TODO: Increment message sent count?
    blinkLED();
  }
}

//// Util

static void blinkLED(){
  rgbLed.turnOff();
  rgbLed.setColor(0,0, RGB_LED_BRIGHTNESS);
  delay(500);
  rgbLed.turnOff();
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

  // TODO: Prep Sensors

  Serial.print("Sending interval at ");
  Serial.println(send_interval);

  delay(setup_delay);
  Screen.clean();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (hasIoTHub && hasWifi)
  {
    char buff[128];

    // replace the following line with your data sent to Azure IoTHub
    snprintf(buff, 128, "{\"topic\":\"iot\",\"temperature\":30.000000,\"humidity\":29.000000}");
    
    if (DevKitMQTTClient_SendEvent(buff))
    {
      Screen.print(LINE_1, "Sending...");
    }
    else
    {
      Screen.print(LINE_1, "Failure...");
    }
    delay(send_interval);
  }else{
    Screen.print(LINE_1, "Error...");
  }
}
