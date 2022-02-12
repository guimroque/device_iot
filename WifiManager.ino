
extern String SaveCustomWifi(fs::FS &fs, String ssid, String psk, String CONFIG_FILE);
  
void setupWifiManager(){
WiFiManager wifiManager;
  pinMode(PIN_AP, INPUT);

  //callback que indica que o ESP entrou no modo AP
  wifiManager.setAPCallback(configModeCallback); 
  wifiManager.setSaveConfigCallback(saveConfigCallback); 
  wifiManager.autoConnect("ESP_AP","12345678"); //cria uma rede sem senha
}

void espRestartWifiManager(){
WiFiManager wifiManager;
      Serial.println("[WFM] => Reiniciando esp32 no modo servidor"); //tenta abrir o portal
      if(!wifiManager.startConfigPortal("ESP_AP", "12345678") ){
        Serial.println("[WFM] => Falha ao conectar");
        delay(2000);
        ESP.restart();
        delay(1000);
      }
}


void configModeCallback (WiFiManager *myWiFiManager) {  
    Serial.println("[WFM] => Entrou no modo de configuração");
    Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  }
  
  //callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  WiFiManager wifiManager;
    Serial.println("[WFM] => Configuração salva");
    Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  }
