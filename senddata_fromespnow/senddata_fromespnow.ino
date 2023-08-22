#include <esp_now.h>
#include <WiFi.h>


#define WIFI_SSID "Tien Thanh"
#define WIFI_PASSWORD "tienthanh2003"


#define LEDR 23       // đèn báo động đỏ - có trộm
#define LEDG 22       // đèn báo động xanh - an toàn


// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x0C, 0xB8, 0x15, 0x76, 0x11, 0x40};

// Define a data structure
typedef struct struct_message {
  int sr505;
  int ldr;
  float temp;
  float humid;
} struct_message;

// Create a structured object
struct_message myData;

esp_now_peer_info_t peerInfo;
//----------------------------hàm get tk và      mk trên firebase--------------------------------------
// void signIn(const char *email, const char *password)
// {

//     auth.user.email = email;
//     auth.user.password = password;

//     Firebase.reset(&config);
//     Firebase.begin(&config, &auth);
// }
//---------------------------------------------------------------------------------------------

// -----------hàm set up các thông số ----------------
void setup() {
  Serial.begin(115200);
 
  pinMode(LEDR,OUTPUT);
  pinMode(LEDG,OUTPUT);
  
  //Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("...");
  // }
  // Serial.println("WiFi connected");

  // Initilize ESP-NOW
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

 if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  } 
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}
//--------------hàm đọc các thông số của dht11 và firebase-----------------
void humid_temperature(){
  Serial.print("Temp: ");
  Serial.println(myData.temp);
  Serial.print("Humidity: ");
  Serial.println(myData.humid);
}
// ----------------hệ thống giám sát và firebase-----------------------------
void detection(){
  //che do troi sang
  if(myData.ldr == 0){
        Serial.println(F("\n trời sáng"));
     // Firebase.setString(firebaseData, "/EspNow_Test/DayTime","Ban ngày, trời sáng.");
        if (myData.sr505==0){
          Serial.println(F("\n không có trộm"));
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, HIGH);
          //  digitalWrite(Loa, LOW);
           // digitalWrite(Light, LOW);

        //  Firebase.setString(firebaseData, "/EspNow_Test/Detection","Không có khả nghi.");
        } else if (myData.sr505 == 1){
          Serial.println(F("\n có trộmmmmmm"));
            digitalWrite(LEDR, HIGH);
            digitalWrite(LEDG, LOW);
        //  Firebase.setString(firebaseData, "/EspNow_Test/Detection","Phát hiện khả nghi !!!");
        }
} else 
//che do troi toi
if(myData.ldr==1){
        Serial.println(F("\n trời tối"));
     // Firebase.setString(firebaseData, "/EspNow_Test/DayTime","Ban đêm, trời tối.");
        if (myData.sr505==0){
          Serial.println(F("\n không có trộm"));
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, HIGH);
          //  digitalWrite(Loa, LOW);
           // digitalWrite(Light, LOW);

       // Firebase.setString(firebaseData, "/EspNow_Test/Detection","Không có khả nghi.");
        } else if (myData.sr505 == 1){
          Serial.println(F("\n có trộmmmmmm"));
            digitalWrite(LEDR, HIGH);
            digitalWrite(LEDG, LOW);
        //    Firebase.setString(firebaseData, "/EspNow_Test/Detection","Phát hiện khả nghi !!!");
        }
}
}
//-------------------ham truyen du lieu espnow------------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//--------------------ham nhan thong tin tu esspnow-------------------------------
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  
   humid_temperature();
  //  detection();


  
}


void loop() {

   
  // if (result == ESP_OK) {
  //   Serial.println("Sending confirmed");
  // }
  // else {
  //   Serial.println("Sending error");
  // }
  // delay(2000);
}