#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "PcRobbie";
const char* password = "1234567890";
const char* mqtt_server = "192.168.134.74";
const char* topic_s = "Vel_motor1";
const char* topic_p = "msg_robot1";

WiFiClient espClient;
PubSubClient client(espClient);


void callback(char* topic_s, byte* payload, unsigned int length) {

  char message[5]={0x00};
  uint8_t count =0;

  for(int i=0;i<length;i++){
    if (isPrintable(payload[i])){
      message[count]=(char)payload[i];
      count++;
    }
  }
  
  Serial.println (message);  
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(2,OUTPUT);
  digitalWrite(2, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.println("Conectando a WiFi...");
  }

  //Serial.println("Conectado a la red WiFi");
  digitalWrite(2, LOW);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
   //Serial.println("Conectando al servidor MQTT...");
    if (client.connect("ESP8266Client")) {
    //  Serial.println("Conectado al servidor MQTT");
      client.subscribe(topic_s);
      digitalWrite(2, HIGH);
    } else {
     // Serial.println("Error de conexión, reintento en 5 segundos");
      delay(5000);
    }
  }
}

void loop() {
  client.loop();

  // Si hay datos disponibles en el puerto serial, léelos y publícalos en el servidor MQTT
  if (Serial.available() > 0) 
  {
    String mensaje = Serial.readStringUntil('\n');
    //Serial.print("Enviando mensaje al servidor MQTT: ");
    //Serial.println(mensaje);
    client.publish(topic_p, mensaje.c_str());
  }
  
}
