# RoomTempDevice-IoT
Sends sensor data from a MXChip AZ3166 IoT Devkit to Azure IoT Hub at a defined interval

![](Docs/RoomTempDevice-IoT.png)

## Getting Started
* Run through [Get started - IoT DevKit guide](https://microsoft.github.io/azure-iot-developer-kit/docs/get-started/)
    * Prepare your development environment
    * Provision your device using the Azure DPS
    * Load your IoT Hub Device Connection String
    * Git clone && Upload Device Code

## Configuration

|| Description|
|---|---|
|`send_interval`|The interval at which the device will send the payload to [IoT Hub](https://azure.microsoft.com/en-au/services/iot-hub/).|
|`setup_delay` | The delay the setup messages will be displayed on the screen.|

> If running 24 hours a day - keep the `send_interval` greater than 10.8 seconds (10800ms) to stay within the [IoT Hub Standard tier free edition](https://azure.microsoft.com/en-us/pricing/details/iot-hub/) 8,000 messages per day

## Payload <a name="payload"></a>

An example of the payload that will be sent to [IoT Hub](https://azure.microsoft.com/en-au/services/iot-hub/)

```JSON
{
	"topic": "iot",
	"Temperature": 31.2999990000000,
	"Humidity": 47.0000000000000
}
```

## Realtime Dashboard

See [RoomTempDashboard](https://github.com/SeanoNET/RoomTempDashboard) for a realtime dashboard