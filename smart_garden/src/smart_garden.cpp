/* Smart Garden Project
** https://github.com/carlkid1499/Smart-Garden  
** 2 Relay switches to control 2 pumps
** Adafruit HUZZAH32 – ESP32 Feather Board [Master]
** Arduino Uno Rev 3 [Slave]
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
#define debug false

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char *ssid = "";
const char *password = "";

// Create a json object to hold our json in RAM
StaticJsonDocument<512> output_doc;

// Place to store our config file
const char *path = "/config.json";

// HTML web page to handle input fields (time, radio buttons)
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

// A func to handle pages that don't exist
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// A func to read from a file, given a path
String readFile(fs::FS &fs, const char *path)
{
#if debug
  Serial.printf("Reading file: %s\r\n", path);
#endif

  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
#if debug
    Serial.println("- empty file or failed to open file");
#endif

    return String();
  }

#if debug
  Serial.println("- read from file:");
#endif

  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  file.close();

#if debug
  Serial.println(fileContent);
#endif

  return fileContent;
}

// A func to write to a file, given a path and message
void writeFile(fs::FS &fs, const char *path, const char *message)
{
#if debug
  Serial.printf("Writing file: %s\r\n", path);
#endif

  File file = fs.open(path, "w");
  if (!file)
  {
#if debug
    Serial.println("- failed to open file for writing");
#endif
    return;
  }
  if (file.print(message))
  {
#if debug
    Serial.println("- file written");
#endif
  }
  else
  {
#if debug
    Serial.println("- write failed");
#endif
  }
  file.close();
}

// A func to write json values to disk
void write_json_values(const char *filename, char *contents)
{
  // Write to the file
  writeFile(SPIFFS, path, contents);

#if debug
  Serial.println(contents);
#endif
}

String read_json_values(const char *filename)
{
  // Get the contents of the file
  String input = readFile(SPIFFS, filename);

#if debug
  Serial.print(input);
#endif

  // Return the results
  return input;
}

void setup()
{
  Serial.begin(115200);

  // Init to some default values
  output_doc["SetLightTimeOn"] = "";
  output_doc["SetLightTimeOff"] = "";
  output_doc["SetWaterTimeOn"] = "";
  output_doc["SetWaterTimeOff"] = "";
  output_doc["BTN_WaterOn"] = false;
  output_doc["BTN_LightsOn"] = false;

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
#if debug
    Serial.println("An Error has occurred while mounting SPIFFS");
#endif

    return;
  }

  if (!SPIFFS.exists(path))
  {
    Serial.println("No file, setting defaults!");
    // Create items to store in our json object. Init to some default values
    output_doc["SetLightTimeOn"] = "2021-08-01T12:00";
    output_doc["SetLightTimeOff"] = "2021-08-01T12:00";
    output_doc["SetWaterTimeOn"] = "2021-08-01T12:00";
    output_doc["SetWaterTimeOff"] = "2021-08-01T12:00";
    output_doc["BTN_WaterOn"] = false;
    output_doc["BTN_LightsOn"] = false;

    char output[250] = "";
    serializeJson(output_doc, output);
    // Lastly, we write those out to a file
    write_json_values(path, output);
  }
  else
  {
#if debug
    Serial.println("Trying to read from file!");
#endif

    StaticJsonDocument<512> input_doc;
    String input = read_json_values(path);

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(input_doc, input);

    // Check for an Error
    if (error)
    {
#if debug
      Serial.println("Couldn't read from file using the defaults!");
#endif

      // use the defaults
      output_doc["SetLightTimeOn"] = "2021-08-01T12:00";
      output_doc["SetLightTimeOff"] = "2021-08-01T12:00";
      output_doc["SetWaterTimeOn"] = "2021-08-01T12:00";
      output_doc["SetWaterTimeOff"] = "2021-08-01T12:00";
      output_doc["BTN_WaterOn"] = false;
      output_doc["BTN_LightsOn"] = false;

      char output[250] = "";
      serializeJson(output_doc, output);
      // Lastly, we write those out to a file
      write_json_values(path, output);
    }
    else
    {
#if debug
      Serial.println("Successfully read from the file!");
#endif

      // read the values from the file.
      output_doc["SetLightTimeOn"] = input_doc["SetLightTimeOn"];
      output_doc["SetLightTimeOff"] = input_doc["SetLightTimeOff"];
      output_doc["SetWaterTimeOn"] = input_doc["SetWaterTimeOn"];
      output_doc["SetWaterTimeOff"] = input_doc["SetWaterTimeOff"];
      output_doc["BTN_WaterOn"] = input_doc["BTN_WaterOn"];
      output_doc["BTN_LightsOn"] = input_doc["BTN_LightsOn"];
    }
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }

#if debug
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
#endif

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  // Send a GET request to <ESP_IP>/get?inputString=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String inputMessage;
              // GET inputString value on <ESP_IP>/get?inputString=<inputMessage>
              if (request->hasParam("SetLightTimeOn"))
              {
                inputMessage = request->getParam("SetLightTimeOn")->value();
                output_doc["SetLightTimeOn"] = inputMessage;
              }
              // GET inputInt value on <ESP_IP>/get?inputInt=<inputMessage>
              else if (request->hasParam("SetLightTimeOff"))
              {
                inputMessage = request->getParam("SetLightTimeOff")->value();
                output_doc["SetLightTimeOff"] = inputMessage;
              }
              // GET inputFloat value on <ESP_IP>/get?inputFloat=<inputMessage>
              else if (request->hasParam("SetWaterTimeOn"))
              {
                inputMessage = request->getParam("SetWaterTimeOn")->value();
                output_doc["SetWaterTimeOn"] = inputMessage;
              }
              else if (request->hasParam("SetWaterTimeOff"))
              {
                inputMessage = request->getParam("SetWaterTimeOff")->value();
                output_doc["SetWaterTimeOff"] = inputMessage;
              }
              else if (request->hasParam("BTN_LightsOn"))
              {
                inputMessage = request->getParam("BTN_LightsOn")->value();
                output_doc["BTN_LightsOn"] = true;
              }
              else if (request->hasParam("BTN_LightsOff"))
              {
                inputMessage = request->getParam("BTN_LightsOff")->value();
                output_doc["BTN_LightsOn"] = false;
              }
              else if (request->hasParam("BTN_WaterOn"))
              {
                inputMessage = request->getParam("BTN_WaterOn")->value();
                output_doc["BTN_WaterOn"] = true;
              }
              else if (request->hasParam("BTN_WaterOff"))
              {
                inputMessage = request->getParam("BTN_WaterOff")->value();
                output_doc["BTN_WaterOn"] = false;
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

  delay(5000);
  char output[250] = "";
  serializeJson(output_doc, output);
  // Lastly, we write those out to a file
  write_json_values(path, output);
}