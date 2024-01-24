#define ROSSERIAL_ARDUINO_TCP
#include <ros.h>
#include <tortoisebot_mini/Diff.h>
#include <WiFi.h>

// WiFi Credentials
const char* ssid = "focaslab";
const char* password = "wifi@focaslab";
const uint16_t serverPort = 11411;  

// WiFi Client and Server IP Address
WiFiClient client;
IPAddress server(192, 168, 0, 187); // Replace with your server IP address
//ros::NodeHandle nh;

class WiFiHardware {
public:
  WiFiHardware() {};
  void init() {
    
    if (!client.connect(server, 11411)) { 
      Serial.println("Connection to ROS Server failed!");
     
    } else {
      Serial.println("Connected to ROS Server");
    }
  }
  int read() {
    return client.read();
  }
  void write(uint8_t* data, int length) {
    client.write(data, length);
  }
  unsigned long time() {
    return millis();
  }
};

// Motor 1 (existing motor) pins and encoder variables
const int encoderPinA1 = 18;
const int encoderPinB1 = 19;
const int motorPWMPin1 = 32; // Ensure this pin supports PWM
const int motorDirPin1_1 = 25; // First direction pin connected to motor 1
const int motorDirPin1_2 = 26; // Second direction pin connected to motor 1

volatile long encoderCount1 = 0;
long lastEncoderCount1 = 0;
unsigned long lastTime1 = 0;

// Motor 2 (new motor) pins and encoder variables
const int encoderPinA2 = 16;
const int encoderPinB2 = 17;
const int motorPWMPin2 = 33; // PWM pin connected to motor 2
const int motorDirPin2_1 = 13; // First direction pin connected to motor 2
const int motorDirPin2_2 = 27; // Second direction pin connected to motor 2

volatile long encoderCount2 = 0;
long lastEncoderCount2 = 0;
unsigned long lastTime2 = 0;

const float edgesPerRevolution = 285.0; // Same for both motors

// PID parameters - Assuming the same for both motors
float kp = 1.45;
float kd = 0.28;
float ki = 0.00;

// PID variables for both motors
float eprev1 = 0, eprev2 = 0;
float eintegral1 = 0, eintegral2 = 0;

// Target RPM for both motors
float targetRPM1 = 0, targetRPM2 = 0;

//ros::NodeHandle_<WiFiHardware> nh;
ros::NodeHandle  nh;
void updateEncoder1() {
  encoderCount1++;
}

void updateEncoder2() {
  encoderCount2++;
}

void setMotor(int dir, int pwmVal, int pwmPin) {
  analogWrite(pwmPin, pwmVal);
  
  Serial.print(", PWM: "); Serial.print(pwmVal);}
 

void controlMotor(int motorNum, unsigned long currentTime, unsigned long &lastTime,
                  long &lastEncoderCount, volatile long &encoderCount,
                  float &eprev, float &eintegral, int motorPWMPin, int motorDirPin1, int motorDirPin2) {
  float deltaT = (currentTime - lastTime) / 1000.0;
  if (deltaT >= 0.1) {
    long currentCount = encoderCount;
    long countDifference = currentCount - lastEncoderCount;
    
    float currentRPM = (countDifference / edgesPerRevolution) / deltaT * 60.0;
    float targetRPM = motorNum == 1 ? targetRPM1 : targetRPM2;
    float e = targetRPM - currentRPM;
    eintegral += e * deltaT;
    float ederivative = (e - eprev) / deltaT;
    float u = kp * e + ki * eintegral + kd * ederivative;
    
    eprev = e;

    int dir = u >= 0 ? 1 : 0;
    float pwr = min(fabs(u), 255.0f);
    setMotor(dir, (int)pwr, motorPWMPin);

    lastEncoderCount = currentCount;
    lastTime = currentTime;
  }
}

void diffCb(const tortoisebot_mini::Diff& msg) {
  targetRPM1 = msg.lrpm.data;
  targetRPM2 = msg.rrpm.data;
  
  // Direction data from the Diff message
  bool dir1 = msg.ldir.data; // Assuming 1 for forward, 0 for backward
  bool dir2 = msg.rdir.data; // Assuming 1 for forward, 0 for backward

  // Set motor direction based on the received direction data
  digitalWrite(motorDirPin1_1, dir1 ? HIGH : LOW);
  digitalWrite(motorDirPin1_2, dir1 ? LOW : HIGH);

  digitalWrite(motorDirPin2_1, dir2 ? HIGH : LOW);
  digitalWrite(motorDirPin2_2, dir2 ? LOW : HIGH);

  Serial.print("Received RPM1: ");
  Serial.print(targetRPM1);
  Serial.print(" RPM2: ");
  Serial.print(targetRPM2);
  Serial.print(" Dir1: ");
  Serial.print(dir1);
  Serial.print(" Dir2: ");
  Serial.println(dir2);
}


ros::Subscriber<tortoisebot_mini::Diff> sub_diff("diff", diffCb);

void setup() {
  Serial.begin(9600);
  while (!Serial) ; // Wait for Serial to be ready

  Serial.println("Initializing...");

  // Motor 1 setup
  pinMode(motorPWMPin1, OUTPUT);
  pinMode(motorDirPin1_1, OUTPUT);
  pinMode(motorDirPin1_2, OUTPUT);

  // Motor 2 setup
  pinMode(motorPWMPin2, OUTPUT);
  pinMode(motorDirPin2_1, OUTPUT);
  pinMode(motorDirPin2_2, OUTPUT);

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  nh.getHardware()->setConnection(server, serverPort);

  nh.initNode();
  nh.subscribe(sub_diff);
  Serial.println("Subscribed to diff");
}


void loop() {
  unsigned long currentTime = millis();

  // Check if connected to WiFi, if not, try to reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nReconnected to WiFi");
  }

  // Check if connected to ROS server, if not, try to reconnect
  if (!nh.connected()) {
    Serial.println("Reconnecting to ROS server...");

    // Reinitialize the WiFiHardware connection
   // nh.getHardware()->init();
    nh.getHardware()->setConnection(server, serverPort);
    // Reinitialize the node
    nh.initNode();

    // Wait for connection
    while (!nh.connected()) {
      delay(10);
      nh.spinOnce();
    }
    Serial.println("Reconnected to ROS server");
  }

  controlMotor(1, currentTime, lastTime1, lastEncoderCount1, encoderCount1, eprev1, eintegral1, motorPWMPin1, motorDirPin1_1, motorDirPin1_2);
  controlMotor(2, currentTime, lastTime2, lastEncoderCount2, encoderCount2, eprev2, eintegral2, motorPWMPin2, motorDirPin2_1, motorDirPin2_2);

  nh.spinOnce();
  delay(200);
}
