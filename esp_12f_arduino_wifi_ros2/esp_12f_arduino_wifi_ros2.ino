/*
  Nombre del Proyecto: esp_12f_arduino_wifi_ros2

  Descripción:
  codigo compatible con el software arduino para su programacion. 
  es un Puente de datos bidirrecional entre uart de la esp12f (esp8266) y MQTT,
  cualquier dispositivo que se conecte por el periferico del uart se 
  comunicara con la esp12f(esp8266),para se convierta el puente entre Mqtt y serial del dispositivo.

  Conceptos y secuencia del codigo:

  1. la esp12f se conecta a la red, con los parametros (SSID y password). 
  2. la esp12f se conecta al BROKER Mqtt, con el parametro (mqtt_server).
  3. si logra conectarse al BROKER, se subscribe al topico definido en topic_s.
  4. si el BROKER envia datos al esp 12f,esta llama al callback para responder con su instrucciones.
  5. si en la esp12f,recibe los datos por parte del uart esta publica los datos hacia el BROKER.

  Al implementar esto codigo, luego realiza la configuracion de parametros correcto, vera como la esp12f
  prende y apaga su led indicando que ya esta listo, para recibir y transmitir datos.

  Autor: jose heriberto marquez diaz

  Fecha de Creación: 15 de enero de 2024
*/

// bibliotecas incluidas
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// parametros principal de la red que se conectará
const char* ssid = "PcRobbie";
const char* password = "1234567890";
const char* mqtt_server = "192.168.134.74"; //misma ip del computador que le asigno la red
const char* topic_s = "mqtt_sub_1"; // topico que se va a subscribirse.
const char* topic_p = "mqtt_pub_1"; // topico donde publica los datos.

// uso de la libreria PubSubClient
WiFiClient espClient;
PubSubClient client(espClient);

// funcion que maneja la recepción de datos por parte del BROKER.
void callback(char* topic_s, byte* payload, unsigned int length) {

  char message[5]={0x00};
  uint8_t count =0;

  // lees todo los caracteres y te quedas con el mensaje real del BROKER.
  for(int i=0;i<length;i++){
    if (isPrintable(payload[i])){
      message[count]=(char)payload[i];
      count++;
    }
  }
  
  Serial.println (message);  // imprimes el mensaje completo al serial.
}

void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(2,OUTPUT);
  digitalWrite(2, HIGH);
  
  //espere hasta que logre conectarse a la red, con los parametros (SSID y password). 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    //Serial.println("Conectando a WiFi...");
  }

  //Serial.println("Conectado a la red WiFi");
  digitalWrite(2, LOW);
  
  client.setServer(mqtt_server, 1883); //define el puerto donde esta ubicado el servidor.
  client.setCallback(callback); // define la funcion que va llamar, cuando el servidor envia datos

  while (!client.connected()) {// espere hasta que logre conectarse al BROKER mqtt
   //Serial.println("Conectando al servidor MQTT...");
    if (client.connect("ESP8266Client")) {
    //  Serial.println("Conectado al servidor MQTT");
      client.subscribe(topic_s); //se subscribe al topico definido en topic_s.
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
    String mensaje = Serial.readStringUntil('\n'); // lee la cadena de caracteres hasta encontrar "\ņ"
    //Serial.print("Enviando mensaje al servidor MQTT: ");
    //Serial.println(mensaje);
    client.publish(topic_p, mensaje.c_str()); // publicas el mensaje al servidor mqtt con la variable topic_p.
  }
  
}
