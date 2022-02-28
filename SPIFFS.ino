#include "FS.h"
#include "ArduinoJson.h"

String readFile(fs::FS &fs, const char *path)
{
  // int errorValue;
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("[SPF] => Empty file or failed to open file");
    //  errorValue = 1;
    return "Arquivo nao encontrado";
  }
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  file.close();
  //  error = 0;
  return fileContent;
}

bool writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.println(path);
  File file = fs.open(path, "w");
  Serial.println(message);
  if (!file)
  {
    Serial.println("[SPF] => Failed to open file for writing");
    return 0;
  }
  if (file.print(message))
  {
    Serial.println("[SPF] => File written");
    return 1;
  }
  else
  {
    Serial.println("[SPF] => Write failed");
    return 0;
  }
  file.close();
}

//
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("[SPF] => Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("[SPF] => Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("[SPF] => Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

bool getFileDir(fs::FS &fs, const char *dirname, uint8_t levels, String findFile)
{
  Serial.printf("[SPF] => Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("[SPF] => Failed to open directory");
    return false;
  }
  if (!root.isDirectory())
  {
    Serial.println("[SPF] => Not a directory");
    return false;
  }

  File file = root.openNextFile();
  bool FLAG_FOUND = false;
  bool FLAG_END = false;
  while (file && !FLAG_FOUND)
  {
    if (file.isDirectory())
    {
    }
    else
    {
      if (String(file.name()) == String(findFile))
      {
        FLAG_FOUND = true;
      }
    }
    file = root.openNextFile();
  }
  return FLAG_FOUND;
}

bool deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path))
  {
    Serial.println("[SPF] => File deleted");
    return 1;
  }
  else
  {
    Serial.println("[SPF] => Delete failed");
    return 0;
  }
}

String SaveCustomWifi(fs::FS &fs, String ssid, String psk, String CONFIG_FILE)
{
  DynamicJsonDocument doc(1024);

  doc["WIFIPASS"] = psk;
  doc["WIFISSID"] = ssid;
  String output = doc.as<String>();

  return output;
}

struct wifiCredentials getwifiSSID(fs::FS &fs, String file)
{

  String value = readFile(SPIFFS, file.c_str());
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, value);
  String wifissid = doc["WIFISSID"];
  String wifipass = doc["WIFIPASS"];
  struct wifiCredentials myResponse;
  myResponse.WIFISSID = wifissid;
  myResponse.WIFIPASS = wifipass;
  return myResponse;
}
String readFileProperties(fs::FS &fs, const char *path, String propriedade)
{
  String leitura = readFile(SPIFFS, "/configFile");

  DynamicJsonDocument doc(1024);
  deserializeJson(doc, leitura);

  return !doc[propriedade].isNull() ? doc[propriedade].as<String>() : String();
}

//
