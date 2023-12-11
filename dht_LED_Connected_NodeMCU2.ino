#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

// Define LED pins
#define led1 D3
#define led2 D8

#define DHTPIN D7 //dht pin
#define DHTTYPE DHT11 //dht  type

// REPLACE WITH NodeMCU#1 MAC Address
uint8_t broadcastAddress[] = {0xE8, 0xDB, 0x84, 0xE1, 0x61, 0x33};

// Structure to declare variable to copy button values
typedef struct myStruct {
  bool a;
  bool b;
} myStruct;

myStruct structObj; // Create a myStruct object called structObj

float humidity;
float temperature;

// Structure to declare variable to copy Dht values
typedef struct dhtStruct {
  float temp;
  float humdt;
} dhtStruct;

dhtStruct dhtObj; //Create a dhtStruct objwct called dhtObj

DHT dht(DHTPIN, DHTTYPE); //Create a DHT object

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}


// Callback function
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&structObj, incomingData, sizeof(structObj));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("D1 button state: ");
  Serial.println(structObj.a);
  Serial.print("D3 button state: ");
  Serial.println(structObj.b);
  Serial.println();

  structObj.a ? digitalWrite(led1, HIGH) : digitalWrite(led1, LOW); //toggle led 1
  structObj.b ? digitalWrite(led2, HIGH) : digitalWrite(led2, LOW); //toggle led 2
}

void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor

  //set pinMode
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  WiFi.mode(WIFI_STA);  // Set device as a Wi-Fi Station

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);  // Set role

  // Add NodeMCU1 as a peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_CONTROLLER, 1, NULL, 0);

  esp_now_register_recv_cb(OnDataRecv);       // Set callback
  esp_now_register_send_cb(OnDataSent);       //set callback
  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  delay(2000); // Add delay 
 
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again)
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  }else{
    //copy dht values to dhtStruct object
    dhtObj.temp =  temperature;
    dhtObj.humdt =  humidity;

     // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &dhtObj, sizeof(dhtObj)); 
  }

}
