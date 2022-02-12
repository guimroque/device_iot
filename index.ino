
#include "SPIFFS.h"
#include "ArduinoJson.h"
#include "secrets.h"
#include "rom/gpio.h"

#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

//==================================================================================ConstsToSystem=====================================================================

const char CONFIG_FILE[] = "/wifiConection.txt";
const char CONFIG_WIFIPASS[] = "WIFIPASS";
const char CONFIG_WIFISSID[] = "WIFISSID";

struct wifiCredentials{
  String WIFISSID;
  String WIFIPASS;
};

bool FLAG_PENDING_RESPONSE = false;
String PENDING_MESSAGE_RESPONSE;
String PENDING_MESSAGE_RESULT;
//==================================================================================ConstsToSystem=====================================================================

//==================================================================================ExternFunctions====================================================================

//=======>SPIFFS
extern String readFile(fs::FS &fs, const char * path);
extern bool writeFile(fs::FS &fs, const char * path, const char * message);
extern void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
extern bool deleteFile(fs::FS &fs, const char * path);
extern String SaveCustomWifi(fs::FS &fs, String ssid, String psk, String CONFIG_FILE);
extern bool getFileDir(fs::FS &fs, const char * dirname, uint8_t levels, String findFile);

//=======>WIFIMANAGER
extern void setupManager();
extern struct wifiCredentials getwifiSSID(fs::FS &fs, String file);

//=======>PUBSUBCLIENT
void messageHandler(char* topic, byte* payload, unsigned int length);

//==================================================================================ExternFunctions====================================================================

//==================================================================================Imports===========================================================================
#include <WiFi.h>   
#include <WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
//==================================================================================Imports===========================================================================

//==================================================================================Flagts============================================================================

//flag para indicar se foi salva uma nova configuração de rede

bool shouldSaveConfig = false;
const int PIN_AP = 5; //FLAG DE RESET DE SENHA
const int PIN_LED = 2;
bool existFile;
//==================================================================================Flags==============================================================================


void setup() {
  SPIFFS.begin(true);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_AP, INPUT);
  Serial.begin(9600);
  existFile = getFileDir(SPIFFS, "/", 0, CONFIG_FILE);
  
//============================== LOGICA PARA CONEXÃO DE SERVIÇO WEB [FINALIZADO] =============================================================
 if(existFile == 1){
  struct wifiCredentials myWifiCredentials= getwifiSSID(SPIFFS, CONFIG_FILE);
  WiFiServer server(80);
  WiFi.begin(myWifiCredentials.WIFISSID.c_str(), myWifiCredentials.WIFIPASS.c_str());

  server.begin();
  int timeConection = 0;
  while ((timeConection<10)) {
      delay(500);
      Serial.print(".");
      timeConection++;
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("[INDEX] => Falha ao tentar conectar ao WiFi"); 
  }else{
    Serial.print("[INDEX] => WiFi connected, host:");
    Serial.println(WiFi.localIP());
      
  }
//=========CONECTANDO A AWS IOT CORE================  
    WiFiClientSecure net;    
    PubSubClient client(net);
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);
  
    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.setServer(AWS_IOT_ENDPOINT, 8883);
    
    client.connect(THINGNAME);
    
    // Create a message handler
    client.setCallback(messageHandler);
//=========CONECTANDO A AWS IOT CORE================

//=========PRENDENDO SETUP EM UM LOOP QUE VERIFICA MENSAGENS DA AWS================================================================    
    if (!client.connected()) {
      Serial.println("[INDEX] => Tempo de conexao com AWSIot acabou, nao conectado");
    }else{
      Serial.println("[INDEX] => Conexao com AWSIot estabelecida");
      while(1){
        if(FLAG_PENDING_RESPONSE){
              StaticJsonDocument<200> doc;
              doc["result"] = PENDING_MESSAGE_RESULT;
              doc["response"] = PENDING_MESSAGE_RESPONSE;
              char jsonBuffer[512];
              serializeJson(doc, jsonBuffer); // print to client
              
              client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
              Serial.print("[INDEX] => Flag de response ativa: ");
              Serial.println(PENDING_MESSAGE_RESPONSE);
              FLAG_PENDING_RESPONSE = false;
              PENDING_MESSAGE_RESPONSE="";
        }
        //listDir(SPIFFS, "/", 0);
        client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
        //Serial.println(client.connected());
        client.loop();
        delay(500);
      }
    }
    
  }else{
    Serial.println("[INDEX] => Arquivo de credenciais de conexao nao encontrado");
    setupWifiManager();
  }  
//=========PRENDENDO SETUP EM UM LOOP QUE VERIFICA MENSAGENS DA AWS================================================================

//============================== LOGICA PARA CONEXÃO DE SERVIÇO WEB [FINALIZADO] =============================================================

}
// GROUP FUNCTIONS

//---------------->TODO configurar leitura do pino de reset como interrupção externa, e deixar preso em um loop apos configurar conexão com aws
// A partir disso agendar tarefas a serem executadas na função de callback
// [FLUXO]=>preso no loop->recebe uma mensagem no topico<nomeTopico>->agenda tarefas pra serem executadas no nucleo 2->tarefas são criadas->volta pro loop->executa as tarefas em paralelo
// !IMPORTANTE -> todas as tarefas referentes ao loop() e setup() são executadas no núcleo 1, deixando o nucleo 0 livre, alocar tarefas somente no nucleo 2
void loop() {
  if(existFile == 0){
    Serial.println("[INDEX] => Iniciando gravacao do arquivo de credenciais de conexao");
    WiFiManager wifiManager;
    String conteudo = SaveCustomWifi(SPIFFS, String(wifiManager.getWiFiSSID()),String(wifiManager.getWiFiPass()), String(CONFIG_FILE));
    writeFile(SPIFFS, CONFIG_FILE, conteudo.c_str());
    Serial.println("[INDEX] => Configuracao salva");
    Serial.println("[INDEX] => Resetando em 5s");
    delay(5000);
    ESP.restart();
  }
  if(digitalRead(PIN_AP) == HIGH){//TODO -> mover if da interrupção pra um outro lugar, de maneira que fique mais organizado
      Serial.println("[INDEX] => Intererrupcao reset de senha");
      deleteFile(SPIFFS, CONFIG_FILE);
      Serial.println("[INDEX] => Resetando em 5s");
      delay(5000);
      ESP.restart();
 }

}
