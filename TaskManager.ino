TaskHandle_t task_high;


String messageMQTT[2] = {"metodo", "value"};
String atuador[5] = {"nome", "pino", "repeats", "time", "diretorio"};


void vHigh(void *pvParameters);


void messageHandler(char* topic, byte* payload, unsigned int length) {
        Serial.print("[TMG] => Mensagem recebida no topico ");
        Serial.println(topic);
        
        DynamicJsonDocument doc(1024);     
        String message;
        
        for(int i=0; i<length; i++){
           message+=(char)payload[i];
        }
        deserializeJson(doc, String(message));
        String value = doc[messageMQTT[0]];
        Serial.print("[TMG] => Metodo acessado: ");
        Serial.println(value);
        if(value == "ADD"){
            int cont = 0;
            for(int j=0; j < 5; j++){ //VALIDAÇÃO PARA KEYS NULAS
              String valueElement = doc[messageMQTT[1]][String(atuador[j]).c_str()];
              doc[messageMQTT[1]][String(atuador[j]).c_str()].isNull()?cont++:cont=cont;
             }
             if(cont>0){
               FLAG_PENDING_RESPONSE = true;
               PENDING_MESSAGE_RESPONSE = "Revise o objeto enviado, todos as keys são obrigatórias";
               PENDING_MESSAGE_RESULT = "Falha";
               return;
             }
             
             String diretorio = doc[messageMQTT[1]][String(atuador[4]).c_str()];
             String gravar;
             serializeJson(doc[messageMQTT[1]], gravar);
             if(writeFile(SPIFFS, diretorio.c_str(), gravar.c_str())){
               FLAG_PENDING_RESPONSE = true;
               PENDING_MESSAGE_RESPONSE = "Arquivo adicionado!";
               PENDING_MESSAGE_RESULT = "Sucesso";
             }else{
               FLAG_PENDING_RESPONSE = true;
               PENDING_MESSAGE_RESPONSE = "Arquivo não pode ser adicionado.";
               PENDING_MESSAGE_RESULT = "Falha";
             }
             
        }else if(value == "REMOVE"){
            String diretorio = doc[messageMQTT[1]][String(atuador[4]).c_str()];
            if(deleteFile(SPIFFS, diretorio.c_str())){
              FLAG_PENDING_RESPONSE = true;
              PENDING_MESSAGE_RESPONSE = "Arquivo removido!";
              PENDING_MESSAGE_RESULT = "Sucesso";
            }else{
             FLAG_PENDING_RESPONSE = true;
             PENDING_MESSAGE_RESPONSE = "Não foi possivel remover o arquivo";
             PENDING_MESSAGE_RESULT = "Falha";
            }
        }
}


void makeTasks(){
  Serial.println("[PBS] => Na função makeTasks");
  xTaskCreatePinnedToCore(vHigh,"vHigh",10000,NULL,1,&task_high,0);
}


void vHigh(void *pvParameters){
  digitalWrite(2,HIGH);
  delay(1000);
  digitalWrite(2,LOW);
  vTaskDelete(NULL);
}




/* METODOS ACESSIVEIS POR ARDUINO_JSON
 *  ==================================================================================================================================================================================
 * createNestedObject -> cria um novo objeto 
 * ---> exemplo:
  StaticJsonDocument<256> doc;
  JsonObject root = doc.to<JsonObject>();
  root["city"] = "Paris";
  JsonObject weather = root.createNestedObject("weather");
  weather["temp"] = 14.2;
  weather["cond"] = "cloudy";
  serializeJsonPretty(root, Serial);
 * ---> saida:
  {
  "city": "Paris",
  "weather": {
    "temp": 14.20,
    "cond": "cloudy"
    }
  }
 * ==================================================================================================================================================================================
 * createNestedArray -> cria um novo array que pode ter os items adicionados ao utilizar o metodo .add() na variavel instanciada como array
 * ---> exemplo: 
  StaticJsonDocument<256> doc;
  JsonObject root = doc.to<JsonObject>();
  root["status"] = "on";
  JsonArray levels = root.createNestedArray("levels");
  levels.add(10);
  levels.add(20);
  serializeJsonPretty(root, Serial);
 * ---> saida:
  {
    "status": "on",
    "levels": [
      10,
      20
    ]
  }
 * ==================================================================================================================================================================================
 * begin e end -> retorna uma variavel do tipo iterator contendo o ultimo elemendo para o metodo .end() e o primeiro elemento para o metodo .begin()
 * ---> exemplo:
  char json[] = "[\"one\",\"two\",\"three\"]";
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, json);
  JsonArray arr = doc.as<JsonArray>();
  
  // using C++11 syntax (preferred):
  for (JsonVariant value : arr) {
      Serial.println(value.as<char*>());
  }
  
  // using C++98 syntax (for older compilers):
  for (JsonArray::iterator it=arr.begin(); it!=arr.end(); ++it) {
      Serial.println(it->as<char*>());
  }
 * --->saida:
  one
  two
  three
 * ==================================================================================================================================================================================
 * ---> METODOS QUE NAO PRECISAM DE EXEMPLO DISPONIVEIS
 * .remove(index) [SOMENTE EM MATRIZES]
 * .getElement(index) [SOMENTE MATRIZES]
 * .size()
 * .inNull()
 * 
 * ---> TIPANDO ELEMENTOS PARA O ARDUINO_JSON [Link: https://arduinojson.org/v6/api/jsonvariant/as/] = String as<String>() const;
 * --->exemplo:
  StaticJsonDocument<256> doc;
  deserializeJson(doc, "{\"answer\":42}");
  
  JsonVariant answer = doc["answer"];
  auto i = answer.as<int>();         // i == 42
  auto d = answer.as<double>();      // d == 42.0
  auto s = answer.as<const char*>();
 */
