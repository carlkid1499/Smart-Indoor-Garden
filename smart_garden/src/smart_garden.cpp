/* Smart Garden Project
** https://github.com/carlkid1499/Smart-Garden  
** 2 Relay switches to control 2 pumps
** Adafruit HUZZAH32 – ESP32 Feather Board
*/

/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "";
const char *password = "";

// Create a json object to hold our json in RAM
StaticJsonDocument<10> output_doc;

// HTML web page to handle 3 input fields (inputString, inputInt, inputFloat)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Smart Garden Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script>
    function submitMessage() {
      alert("Saving value to ESP SPIFFS");
      setTimeout(function(){ document.location.reload(false); }, 500);   
    }
  </script></head><body>
  <form action="/get" target="hidden-form">
    Set Light Time On: <input type="datetime-local" name="SetLightTimeOn" value="2021-08-01T12:00">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    Set Light Time Off: <input type="datetime-local" name="SetLightTimeOff" value="2021-08-01T12:00">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    Set Water Time On: <input type="datetime-local" name="SetWaterTimeOn" value="2021-08-01T12:00">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    Set Water Time Off: <input type="datetime-local" name="SetWaterTimeOff" value="2021-08-01T12:00">
    <input type="submit" value="Submit" onclick="submitMessage()">
  </form><br>
  <form action="/get" target="hidden-form">
    <input type="submit" name="BTN_WaterOn" value="Water On">
    <input type="submit" name="BTN_WaterOff" value="Water Off">
  </form><br>
  <form action="/get" target="hidden-form">
    <input type="submit" name="BTN_LightsOn" value="Lights On">
    <input type="submit" name="BTN_LightsOff" value="Lights Off">
  </form><br>
  <iframe style="display:none" name="hidden-form"></iframe>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}

// Replaces placeholder with stored values
String processor(const String &var)
{
  Serial.println(var);
  if (var == "SetLightTimeOn")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "SetLightTimeOff")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "SetWaterTimeOn")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "SetWaterTimeOff")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "BTN_WaterOn")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "BTN_WaterOff")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "BTN_LightsOn")
  {
    return readFile(SPIFFS, "/config.json");
  }
  else if (var == "BTN_LightsOff")
  {
    return readFile(SPIFFS, "/config.json");
  }
  return String();
}

void write_json_values(const char *filename, JsonDocument &json_doc)
{
  // Open the file for writting
  File myfile = SPIFFS.open(filename, "w");

  // Access our json values & serialize them
  char output[50] = "";
  serializeJson(json_doc, output);

  // Write to the file
  myfile.write(output, 50);

  // Close the file
  myfile.close();
}

int read_json_values(const char *filename, JsonDocument &json_doc)
{
  uint val = 0;
  // Get the contents of the file
  char input[50] = readFile(SPIFFS, file); // PICK UP WORK HERE

  // Access our json values & deserialize them
  deserializeJson(json_doc, input);

  // Return 0 for sucess and 1 for failure
  return val;
}

void setup()
{
  Serial.begin(115200);

// Initialize SPIFFS
#ifdef ESP32
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
#else
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
#endif

  if (!SPIFFS.exists("/config.json"))
  {
    Serial.println("No file, setting defaults!");
    // Create items to store in our json object. Init to some default values
    output_doc["SetLightTimeOn"] = "2021-08-01T12:00";
    output_doc["SetLightTimeOff"] = "2021-08-01T12:00";
    output_doc["SetWaterTimeOn"] = "2021-08-01T12:00";
    output_doc["SetWaterTimeOff"] = "2021-08-01T12:00";
    output_doc["BTN_Water"] = false;
    output_doc["BTN_Lights"] = false;
  }
  else
  {
    Serial.println("Trying to read from file!");
    StaticJsonDocument<10> input_doc;
    File myfile = SPIFFS.open("/config.json", "r");

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(input_doc, myfile);

    // Check for an Error
    if (error)
    {
      Serial.println("Couldn't read from file using the defaults!");
      // use the defaults
      output_doc["SetLightTimeOn"] = "2021-08-01T12:00";
      output_doc["SetLightTimeOff"] = "2021-08-01T12:00";
      output_doc["SetWaterTimeOn"] = "2021-08-01T12:00";
      output_doc["SetWaterTimeOff"] = "2021-08-01T12:00";
      output_doc["BTN_Water"] = false;
      output_doc["BTN_Lights"] = false;
    }
    else
    {
      Serial.println("Successfully read from the file!");
      // read the values from the file.
      output_doc["SetLightTimeOn"] = input_doc["SetLightTimeOn"];
      output_doc["SetLightTimeOff"] = input_doc["SetLightTimeOff"];
      output_doc["SetWaterTimeOn"] = input_doc["SetWaterTimeOn"];
      output_doc["SetWaterTimeOff"] = input_doc["SetWaterTimeOff"];
      output_doc["BTN_Water"] = input_doc["BTN_Water"];
      output_doc["BTN_Lights"] = input_doc["BTN_Lights"];
    }
  }

WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
if (WiFi.waitForConnectResult() != WL_CONNECTED)
{
  Serial.println("WiFi Failed!");
  return;
}
Serial.println();
Serial.print("IP Address: ");
Serial.println(WiFi.localIP());

// Send web page with input fields to client
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
          { request->send_P(200, "text/html", index_html, processor); });

// Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
          {
            String inputMessage;
            // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
            if (request->hasParam("SetLightTimeOn"))
            {
              inputMessage = request->getParam("SetLightTimeOn")->value();
              writeFile(SPIFFS, "/config.json", inputMessage.c_str());
            }
            // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
            else if (request->hasParam("SetLightTimeOff"))
            {
              inputMessage = request->getParam("SetLightTimeOff")->value();
              writeFile(SPIFFS, "/config.json", inputMessage.c_str());
            }
            // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
            else if (request->hasParam("SetWaterTimeOn"))
            {
              inputMessage = request->getParam("SetWaterTimeOn")->value();
              writeFile(SPIFFS, "/config.json", inputMessage.c_str());
            }
            else if (request->hasParam("SetWaterTimeOff"))
            {
              inputMessage = request->getParam("SetWaterTimeOff")->value();
              writeFile(SPIFFS, "/config.json", inputMessage.c_str());
            }
            else if (request->hasParam("BTN_LightsOn"))
            {
              inputMessage = request->getParam("BTN_LightsOn")->value();
              writeFile(SPIFFS, "/config.json", "True");
            }
            else if (request->hasParam("BTN_LightsOff"))
            {
              inputMessage = request->getParam("BTN_LightsOff")->value();
              writeFile(SPIFFS, "/config.json", "False");
            }
            else if (request->hasParam("BTN_WaterOn"))
            {
              inputMessage = request->getParam("BTN_WaterOn")->value();
              writeFile(SPIFFS, "/config.json", "True");
            }
            else if (request->hasParam("BTN_WaterOff"))
            {
              inputMessage = request->getParam("BTN_WaterOff")->value();
              writeFile(SPIFFS, "/config.json", "False");
            }
            else
            {
              inputMessage = "No message sent";
            }
            request->send(200, "text/text", inputMessage);
          });
server.onNotFound(notFound);
server.begin();
}

void loop()
{
  // To access your stored values on inputString, inputInt, inputFloat
  //String yourInputString = readFile(SPIFFS, "/config.json");
  //Serial.print("*** Your inputString: ");
  // Serial.println(yourInputString);

  delay(5000);
}