#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "msgMotorPWM.h"

const char *ssid = "DriJaguar";
const char *pass = "drrobotdrrobot";

IPAddress ip(192,168,0,104);
IPAddress gateway(192,168,0,245);
IPAddress subnet(255,255,255,0);

WiFiUDP Udp;
unsigned int localUdpPort = 10001;
int pin = D7;

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin, OUTPUT);
  
  Serial.begin(9600);
  delay(10);
  
  WiFi.begin(ssid, pass);
  WiFi.config(ip, gateway, subnet);
   
  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  digitalWrite(LED_BUILTIN, LOW);
  
  while( WiFi.status() != WL_CONNECTED ) {
    Serial.print(".");
    delay(500);
  }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("");
  Serial.print("WiFi connected, using IP address: ");
  Serial.println(WiFi.localIP());

  askTCP();
  delay( 1000 );
   
  Serial.println("\nStarting connection to server...");
  Udp.begin( localUdpPort );
  
  Serial.print("Listening on port ");
  Serial.println( localUdpPort );

  askTCP();
  delay( 1000 );
}

void udpSend(char *buff, IPAddress ip)
{
  unsigned int hostUdpPort = 10001;
  
  digitalWrite(LED_BUILTIN, LOW);
  Serial.printf("write to IP %s, UDP port %d\n", ip.toString().c_str(), hostUdpPort);
  msg_did5_println( buff );
  
  Udp.beginPacket(ip, hostUdpPort);
  Udp.write(buff, 12);
  Udp.endPacket();

  digitalWrite(LED_BUILTIN, HIGH);
}

void askTCP( void )
{
const uint16_t port = 10001;
const char * host = "192.168.0.61"; // ip or dns

  Serial.println("");
  Serial.print("TCP connecting to ");
  Serial.println(host);

  WiFiClient client;  // Use WiFiClient class to create TCP connections

  if( !client.connect(host, port) ) {
    Serial.println("connection failed");
    return;
  }

  //read back one line from server
  String line = client.readStringUntil('\r');
  Serial.println(line);
    
  Serial.println("closing connection");
  client.stop();
}

void loop()
{
char buff[12], ex1113[12] = {94,2,1,0,5,3,4,160,15,86,94,13};
IPAddress jaguarIP(192,168,0,60);
IPAddress echoIP(192,168,0,106);

  digitalWrite(pin, HIGH);
  unsigned int  verty=analogRead(0);
  digitalWrite(pin, LOW);
  unsigned int  vertx=analogRead(0);

  int vertA =vertx-verty;
  unsigned int vertB =vertx+verty;  

  if ((vertx>400&&vertx<600)&&(verty>400&&verty<600)){// uz vietas
    msg_did5_encode(buff, 3, 16000); //pwm-atrums
    udpSend(buff, jaguarIP);
    msg_did5_encode(buff, 4, 16000);
    udpSend(buff, jaguarIP);
    delay(100);
  }
  else if (vertA <0 && vertB > 1024){ //atpakal
    unsigned int pwm= map(verty, 600, 1024, 16000, 0);
    msg_did5_encode(buff, 3, pwm); //pwm-atrums
    udpSend(buff, jaguarIP);
    delay(100);
  }
  else if (vertA >0 && vertB < 1024){ //uz prieksu
    unsigned int pwm= map(verty, 0, 400, 32000, 16000);
    msg_did5_encode(buff, 3, pwm);
    udpSend(buff, jaguarIP);
    delay(100);
  }
  else if (vertA <0 && vertB < 1024){ //kreisi
    unsigned int owm= map(vertx, 0, 400, 32000, 16000);
    msg_did5_encode(buff, 4, owm);
    udpSend(buff, jaguarIP);   
    delay(100);
  }
  else if (vertA >0 && vertB > 1024){ //labi
    unsigned int owm= map(vertx, 600, 1024, 16000, 0);
    msg_did5_encode(buff, 4, owm);
    udpSend(buff, jaguarIP);    
    delay(100);
  }
}
