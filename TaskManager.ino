

TaskHandle_t task_high;


String messageMQTT[2] = {"metodo", "value"};
String atuador[5] = {"nome", "pino", "repeats", "timeDelay", "diretorio"};

struct task{
  int pino;
  int timeDelay;
};


void vHigh(void *pvParameters);
void makeTaskJsonObject( String dispositivo);

// TODO ao adicionar um arquivo, enviar uma flag que diz se o dispositivo passa a valer no periodo de 24 horas ja ativo, ou se espera o proximo
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

void makeTasks(fs::FS &fs){
  String sensores[MAX_DISP];
  String atuadores[MAX_DISP];

  Serial.println("[TMG] => Iniciando agendamento de tasks");

    File root = fs.open("/");
    if(!root){
        Serial.println("[TMG] => Diretorio nao encontrado");
    }
    if(!root.isDirectory()){
        Serial.println("[TMG] => Diretorio nao encontrado");
    }

    File file = root.openNextFile();
    int contA = 0;
    int contS = 0;
    while(file){
        if(file.isDirectory()){
        } else {
            if(String(file.name()).indexOf(String(DISP_ACAO)) !=-1 ){
          Serial.print("RESULTADO LOGICO");
          Serial.println(String(file.name()).indexOf(String(DISP_ACAO)));
                atuadores[contA] = file.name();
                contA++;
                Serial.print("ATUADOR: ");
                Serial.println(file.name());
                makeTaskJsonObject(String(file.name()), contA+contS);
            }else if(String(file.name()).indexOf(String(DISP_LEITURA)) != -1 ){
          Serial.print("RESULTADO LOGICO");
          Serial.println(String(file.name()).indexOf(String(DISP_ACAO)));
                atuadores[contS] = file.name();
                contS++;
                Serial.print("SENSOR: ");
                Serial.println(file.name());
            }
        }
        file = root.openNextFile();
    }
}


void makeTaskJsonObject( String dispositivo, int posicao){
  Serial.print("[TMG] => Dispositivo buscado");
  String value = readFile(SPIFFS, dispositivo.c_str());
  Serial.println(value);
  DynamicJsonDocument doc(1024);     
  deserializeJson(doc, String(value));

  //answer.as<int>()
  
  Serial.println("JSON MONTADO");
  String nome = doc[atuador[0]];
  int pino = doc[atuador[1]].as<int>();
  int timeDelay = doc[atuador[3]].as<int>();
  int repeats = doc[atuador[2]].as<int>();
  struct task myTask;
  myTask.pino = pino;
  myTask.timeDelay = timeDelay;
  Serial.println("[TMG] => Informacoes do dispositivo:");
  Serial.println(nome);
  Serial.println(pino);
  Serial.println(repeats);
  Serial.println(timeDelay);
  pinMode(pino, OUTPUT);
  //todas as tarefas devem ser executadas no nucleo livre, o nucleo 0
  xTaskCreatePinnedToCore(taskExec,nome.c_str(),10000,(void*)&myTask, 1,NULL,0);
  xTaskCreatePinnedToCore(vHigh,"SERIAL",10000,NULL, 1,NULL,0);
  return;
}
//void makeTasks(){
//  Serial.println("[PBS] => Na função makeTasks");
//  xTaskCreatePinnedToCore(vHigh,"vHigh",10000,NULL,1,&task_high,0);
//}

void taskExec(void * parameter){
  struct task myTask = *((struct task*)parameter);
  //int pino = *((int*)parameter);
  //int timeDelay*((int*)parameter)
  int pino = myTask.pino;
  int timeDelay = myTask.timeDelay;
  TickType_t xDelay = timeDelay / portTICK_PERIOD_MS;
  Serial.print("[TMG] => Executando instrução do pino: ");
  Serial.println(myTask.pino);
  Serial.println(myTask.timeDelay);
  for(;;){
    Serial.println("[LED] => Debug LED!");
    digitalWrite(myTask.pino, !digitalRead(myTask.pino));
    vTaskDelay(xDelay);  
  }
  
}


void vHigh(void *pvParameters){
  TickType_t xDelay = 3000 / portTICK_PERIOD_MS;
  for(;;){
    Serial.println("[DEBUG] => PRINT");
    vTaskDelay(xDelay);  
  }
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
