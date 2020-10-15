#include<Arduino.h>

std::string BLE_NAME = "Dihambo's ESP32 BLE";

#define ECG_SERVICE_UUID                "0001181C-0000-1000-8000-00805F9B34FB" // UART service UUID
#define ECG_CHARACTERISTIC_UUID         "0003181C-0000-1000-8000-00805F9B34FB"
#define ECG_CHARACTERISTIC_UUID_CTR     "0002181C-0000-1000-8000-00805F9B34FB"
#define TEMP_SERVICE_UUID               "00042A1F-0000-1000-8000-00805F9B34FB" // UART service UUID
#define TEMP_CHARACTERISTIC_UUID        "00062A1F-0000-1000-8000-00805F9B34FB"
#define TEMP_CHARACTERISTIC_UUID_CTR    "00052A1F-0000-1000-8000-00805F9B34FB"
#define HRATE_SERVICE_UUID              "00072A37-0000-1000-8000-00805F9B34FB" // UART service UUID
#define HRate_CHARACTERISTIC_UUID       "00092A37-0000-1000-8000-00805F9B34FB"
#define HRate_CHARACTERISTIC_UUID_CTR   "00082A37-0000-1000-8000-00805F9B34FB"
#define SPO2_SERVICE_UUID               "00102A37-0000-1000-8000-00805F9B34FB" // UART service UUID
#define SPO2_CHARACTERISTIC_UUID        "00122A37-0000-1000-8000-00805F9B34FB"
#define SPO2_CHARACTERISTIC_UUID_CTR    "00112A37-0000-1000-8000-00805F9B34FB"