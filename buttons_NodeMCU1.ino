// add libraries
#include <ESP8266WiFi.h>
#include <espnow.h>

// Define Button pins
#define btn1 D0
#define btn2 D4

bool btn1State = false;
bool btn2State = false;

// REPLACE WITH NodeMCU#2 MAC Address
uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x6B, 0x07, 0xAF};

// Structure to declare variable to copy button values
typedef struct myStruct {
  bool a;
  bool b;
} myStruct;

myStruct structObj; // Create a myStruct object called structObj

// Structure to declare variable to copy Dht values
typedef struct dhtStruct {
  float temp;
  float humdt;
} dhtStruct;

dhtStruct dhtObj; //Create a dhtStruct objwct called dhtObj

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&dhtObj, incomingData, sizeof(dhtObj));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Temperature : ");
  Serial.println(dhtObj.temp);
  Serial.print("Humidity : ");
  Serial.println(dhtObj.humdt);
  Serial.println();
}

void setup() {
  Serial.begin(115200); // Init Serial Monitor
  
  //set pinMode
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);

  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); //set role
  esp_now_register_send_cb(OnDataSent); //set callback for data sent
  esp_now_register_recv_cb(OnDataRecv);  //set callback for data received
 
  // add NodeMCU 2 as a peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}
 
void loop() {
    delay(2000); // Add delay 

    // copy button values to myStruct object
    structObj.a = btn1State; 
    structObj.b = btn2State; 

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &structObj, sizeof(structObj));
 
   //read buttons 1
   if (digitalRead(btn1) == LOW) {
    btn1State = !btn1State;
    delay(50);
    while (digitalRead(btn1) == LOW);
  }
   //read buttons 2
    if (digitalRead(btn2) == LOW) {
    btn2State = !btn2State;
    delay(50); 
    while (digitalRead(btn2) == LOW);
  }
}
