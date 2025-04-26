#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

// WiFi Configuration
const char* ssid = "Mr Router 2";
const char* password = "SS%J44dNC%t6Nf!9&A";
const char* ap_ssid = "ESP8285_AP";
const char* ap_password = "password0123";

// Internet test configuration
const char* testUrls[] = {
  "http://www.google.com",
  "http://httpbin.org/get",
  "http://www.bing.com",
  "http://example.com",
  "http://httpforever.com/",
  "https://free-article-spinner.com/"
};
const int testUrlCount = 6;
int currentTestUrlIndex = 0;
WiFiClient wifiClient;
long checkInterval = 300000;//300000; // 5 minutes in milliseconds
int failedToConnectCount=0;
int resetAllPinCount=0;
const int analogPin = A0;  // The only analog pin on ESP8285 (labeled ADC on board)
int sensorValue = 0;       // Variable to store the analog value

ESP8266WebServer server(80);

const int routerControllerPin = 4;  
const int switchControllerPin = 5; 
bool routerState = true;
bool switchState = true;
bool internetAvailable = false;
unsigned long lastCheckTime = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(routerControllerPin, OUTPUT);
  pinMode(switchControllerPin, OUTPUT);
  digitalWrite(routerControllerPin, HIGH);
  digitalWrite(switchControllerPin, HIGH);

  // Start in both AP and STA mode
  WiFi.mode(WIFI_AP_STA);
  
  // Connect to WiFi network
  connectToWiFi();
  
  // Start Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("Access Point started");
  Serial.println("AP IP address: " + WiFi.softAPIP().toString());

  // Setup web server handlers
  server.on("/", handleRoot);
  server.on("/router-on", []() { setPinState(1, true); });
  server.on("/router-off", []() { setPinState(1, false); });
  server.on("/switch-on", []() { setPinState(2, true); });
  server.on("/switch-off", []() { setPinState(2, false); });
  server.on("/status", handleStatus);
  server.on("/check-internet", handleCheckInternet);
  server.onNotFound(handleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");

  // Initial internet check
  checkInternetConnectivity();
  lastCheckTime = millis();
}

void loop() {
  server.handleClient();
  sensorValue = analogRead(analogPin);
  Serial.print("Analog value: ");
  Serial.print(sensorValue);
  // Check internet connectivity periodically if sensorValue Which is a switch
  if(sensorValue>600){
     checkInternetActivity(); 
  }
 
}

void checkInternetActivity(){
   if (failedToConnectCount==0 && (millis() - lastCheckTime >= checkInterval)) {
    checkInternetConnectivity();
    lastCheckTime = millis();
    checkMessageAndUpdateFailedToConnectCount();
  }
  else if (failedToConnectCount==1 && (millis() - lastCheckTime >= (checkInterval/2))){
    checkInternetConnectivity();
    lastCheckTime = millis();
    checkMessageAndUpdateFailedToConnectCount();
  }
  else if (failedToConnectCount>1 && (millis() - lastCheckTime >= (checkInterval/3))){
    checkInternetConnectivity();
    lastCheckTime = millis();
    checkMessageAndUpdateFailedToConnectCount();
  }
}

void checkMessageAndUpdateFailedToConnectCount(){
  Serial.println("Check:"+String(lastCheckTime)+",Fail:"+String(failedToConnectCount));
  if(internetAvailable){
    failedToConnectCount=0;
    resetAllPinCount=0;
  }
  else if(!internetAvailable && failedToConnectCount<3){
    failedToConnectCount++;
  }
   
  if(failedToConnectCount>=3){
    if (WiFi.status() != WL_CONNECTED){
      
      connectToWiFi();
      if (WiFi.status() != WL_CONNECTED){
        resetAllPin();
      }
    }
    else{
       resetAllPin();
    }
  }
  
  if(resetAllPinCount>5){
     ESP.restart();
     resetAllPinCount=0;
  }
}

void resetAllPin(){
      setPinState(1,false);
      delay(2000);
      setPinState(1,true);
      
      setPinState(2,false);
      delay(2000);
      setPinState(2,true);

      resetAllPinCount++;
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.println("IP address: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nFailed to connect to WiFi");
    internetAvailable = false;
  }
}

void checkInternetConnectivity() {
  if (WiFi.status() != WL_CONNECTED) {
    internetAvailable = false;
    Serial.println("WiFi not connected - can't check internet");
    connectToWiFi();
    return;
  }

  Serial.println("Checking internet connectivity...");
  
  HTTPClient http;
  http.setReuse(true); // Reuse connection
  http.setTimeout(3000); // 3 second timeout
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // Handle 301 redirects
  bool connectionSuccess = false;
  
  Serial.print("Testing connection to: ");
  Serial.println(testUrls[currentTestUrlIndex]);
  http.begin(wifiClient, testUrls[currentTestUrlIndex]);
  
  int httpCode = http.GET();
    
  if (httpCode > 0) {
    Serial.printf("Connection successful, HTTP code: %d\n", httpCode);
    connectionSuccess = true;
  } else {
    Serial.printf("Connection failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
    
  http.end();
  delay(500); // Short delay between tests
  
  internetAvailable = connectionSuccess;
  Serial.println(internetAvailable ? "Internet connection available" : "No internet connection");
  
  currentTestUrlIndex++;
  if(currentTestUrlIndex>5){
    currentTestUrlIndex=0;
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<title>Srijon Router Control</title>"
                "<meta name='viewport' content='width=device-width, initial-scale=1'>"
                "<style>"
                "body { font-family: Arial, sans-serif; text-align: center; margin: 0 auto; padding: 20px; max-width: 500px; }"
                ".btn { background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; "
                "text-decoration: none; display: inline-block; font-size: 16px; margin: 10px 5px; cursor: pointer; border-radius: 8px; }"
                ".off { background-color: #f44336; }"
                ".status { margin: 20px; padding: 10px; border-radius: 5px; }"
                ".on-status { background-color: #dff0d8; color: #3c763d; }"
                ".off-status { background-color: #f2dede; color: #a94442; }"
                ".router-container { border: 1px solid #ddd; border-radius: 8px; padding: 15px; margin: 15px 0; }"
                ".internet-status { margin: 20px; padding: 15px; border-radius: 5px; font-weight: bold; }"
                ".internet-up { background-color: #dff0d8; color: #3c763d; }"
                ".internet-down { background-color: #f2dede; color: #a94442; }"
                "</style>"
                "</head>"
                "<body>"
                "<h2>Srijon Router Control Panel</h2>"
                
                "<div class='internet-status " + String(internetAvailable ? "internet-up" : "internet-down") + "'>"
                "Internet: " + String(internetAvailable ? "AVAILABLE" : "UNAVAILABLE") +
                "</div>"
                "<button class='btn' onclick=\"checkInternet()\">Check Internet Now</button>"
                
                "<div class='router-container'>"
                "<h2>Router Control</h2>"
                "<div class='status " + String(routerState ? "on-status" : "off-status") + "'>"
                "Router is currently " + String(routerState ? "ON" : "OFF") +
                "</div>"
                "<a href='/router-on'><button class='btn'>Turn ON</button></a>"
                "<a href='/router-off'><button class='btn off'>Turn OFF</button></a>"
                "</div>"
                
                "<div class='router-container'>"
                "<h2>Switch Control</h2>"
                "<div class='status " + String(switchState ? "on-status" : "off-status") + "'>"
                "Switch is currently " + String(switchState ? "ON" : "OFF") +
                "</div>"
                "<a href='/switch-on'><button class='btn'>Turn ON</button></a>"
                "<a href='/switch-off'><button class='btn off'>Turn OFF</button></a>"
                "</div>"
                
                "<script>"
                "function checkInternet() {"
                "  var xhr = new XMLHttpRequest();"
                "  xhr.open('GET', '/check-internet', true);"
                "  xhr.send();"
                "  xhr.onload = function() {"
                "    if (xhr.status == 200) {"
                "      location.reload();"
                "    }"
                "  };"
                "}"
                "</script>"
                
                "<h3>Connection Info</h3>"
                "<p><strong>APP IP:</strong> " + WiFi.softAPIP().toString() + "</p>";
  
  if (WiFi.status() == WL_CONNECTED) {
    html += "<p><strong>Connected to WiFi:</strong> " + String(ssid) + "</p>";
    html += "<p><strong>STA IP:</strong> " + WiFi.localIP().toString() + "</p>";
  } else {
    html += "<p><strong>WiFi:</strong> Not connected</p>";
  }
  
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void setPinState(int pinNum, bool state) {
  if (pinNum == 1) {
    digitalWrite(routerControllerPin, state ? HIGH : LOW);
    routerState = state;
  } else if (pinNum == 2) {
    digitalWrite(switchControllerPin, state ? HIGH : LOW);
    switchState = state;
  }
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Redirecting...");
}

void handleStatus() {
  String status = "Router: " + String(routerState ? "ON" : "OFF") + "\n";
  status += "Switch: " + String(switchState ? "ON" : "OFF") + "\n";
  status += "Internet: " + String(internetAvailable ? "AVAILABLE" : "UNAVAILABLE");
  server.send(200, "text/plain", status);
}

void handleCheckInternet() {
  checkInternetConnectivity();
  server.send(200, "text/plain", internetAvailable ? "INTERNET_AVAILABLE" : "INTERNET_UNAVAILABLE");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}
