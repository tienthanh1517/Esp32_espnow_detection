#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTPin 5
#define DHTType DHT11
#define sensor1 17  // quang trở
#define sensor2 18  //sr505mini - cảm biến chuyển động nhiệt

DHT dht(DHTPin, DHTType);

//địa chỉ MAC có sẵn trên esp32 kia
uint8_t broadcastAddress[] = {0xA4, 0xE5, 0x7C, 0xF6, 0x6E, 0x1C};

// khai báo cấu trúc truyền-nhận biến số
typedef struct struct_message {

  int sr505;
  int ldr;
  float temp;
  float humid;
} struct_message;

// tạo cấu trúc data truyền nhận
struct_message myData;

esp_now_peer_info_t peerInfo;
//---------------hamf setup cac thông số------------------------
void setup() {
  Serial.begin(115200);
  dht.begin(); 

  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);

  // kiểm tra vị trí trạng thái wifi
  WiFi.mode(WIFI_STA);

  // thiết lập espnow
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // truy cập vào hàm truyền và nhận callback của espnow
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
}
//--------hàm truyền dữ liệu--callback-------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
// -----------hàm nhận dữ liệu---callback-------
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  // to do list

}
//------ cảm biến đọc nhiệt độ và gửi sang esp----------
void humid_temperature(){
  float temp = dht.readTemperature();
  delay(10);
  float humid = dht.readHumidity();
  delay(10);
  myData.temp = temp;
  myData.humid = humid;
 
  // gửi dữ lệu sang esp32 kia
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   delay(1000);
}
 //------------------------------------------------
 //------------hệ thống giám sát và gửi sang esp kia-----------
 void detection(int value1, int value2){
   // chế độ trời tối
   if (value1==1){
     if(value2==0){
       myData.ldr = 1;// troi toi
       myData.sr505 = 0;// khong co trom
     } else if(value2==1) {
      myData.ldr = 1; //troi toi
      myData.sr505 = 1;//co trom
     }
   } else
   //-- che do troi sang
   if (value1==0){
     if(value2==0){
       myData.ldr = 0;// troi sang
       myData.sr505 = 0;// khong co trom
     } else if(value2==1) {
      myData.ldr = 0; //troi sang
      myData.sr505 = 1;//co trom
     }
   }
    
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  delay(1000);
 }
 //------------------------------------------------------------
void loop() {
  humid_temperature();
  int value1  = digitalRead(sensor1);
  int value2 = digitalRead(sensor2);
  detection(value1,value2);
}