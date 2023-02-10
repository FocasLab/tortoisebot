// lpwm - left PWM Pin
// lfwd - Left Forward Pin
// lback - Left Backward Pin

// lpwm - Right PWM Pin
// lfwd - Right Forward Pin
// lback - Right Backward Pin

#include <ros.h>
#include <tortoisebot/Diff.h>
#include <WiFi.h>
#include<analogWrite.h>

#define DEBUG 0

#define lpwmPin 32 
#define lfwdPin 25  
#define lbackPin 26 

#define rpwmPin 33 
#define rfwdPin 13 
#define rbackPin 27 

//add your wifi ssid & password
const char* ssid = "focaslab";
const char* password = "wifi@focaslab";

WiFiClient client;
//add host device IP address
// to find host IP in cmd : hostname -I
IPAddress server(192,168,0,203);

class WiFiHardware {
  public:
    WiFiHardware() {};
    void init() {
      client.connect(server, 11411);
    }
    int read() {
      return client.read();
    }
    void write(uint8_t* data, int length) {
      for (int i = 0; i < length ; i++)
        client.write(data[i]);
    }
    unsigned long time() {
      return millis();
    }
};

void setupWiFi() {
    
    WiFi.begin(ssid, password);
    Serial.print("\nConnecting to "); Serial.println(ssid);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) delay(500);
    if (i == 21) {
      Serial.print("Could not connect to"); Serial.println(ssid);
      while (1) delay(500);
    }
    Serial.print("Ready! Use ");
    Serial.print(WiFi.localIP());
    Serial.println(" to access client");
  }

int rigpwm = 0;
int lefpwm = 0;

ros::NodeHandle_<WiFiHardware> nh;

void diffCb(const tortoisebot::Diff& toggle_msg){

  Serial.print("toggle_msg.rpwm.data: ");
  Serial.println(toggle_msg.rpwm.data);
  
  analogWrite(rpwmPin, toggle_msg.rpwm.data);
  analogWrite(lpwmPin, toggle_msg.lpwm.data);

  digitalWrite(lfwdPin, !(toggle_msg.ldir.data));
  digitalWrite(lbackPin, toggle_msg.ldir.data);

  Serial.print("toggle_msg.rdir.data: ");
  Serial.println(toggle_msg.rdir.data);
  
  digitalWrite(rfwdPin, toggle_msg.rdir.data);
  digitalWrite(rbackPin, !(toggle_msg.rdir.data));
  
}

ros::Subscriber<tortoisebot::Diff> sub_diff("diff", diffCb );

void setup() {
  
  pinMode (lpwmPin, OUTPUT);
  pinMode (rpwmPin, OUTPUT);
  pinMode (lfwdPin, OUTPUT);
  pinMode (lbackPin, OUTPUT);
  pinMode (rfwdPin, OUTPUT);
  pinMode (rbackPin, OUTPUT);

  Serial.begin(115200);
  
  setupWiFi();

  analogWrite(rpwmPin, 0);
  analogWrite(lpwmPin, 0);
  
  nh.initNode();
  nh.subscribe(sub_diff);
  
}

void loop() {

  nh.spinOnce();
  delay(10);

}
