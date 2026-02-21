#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <LiquidCrystal.h>

// ---------- LCD (Parallel, 20x4, 4-bit) ----------
// RS, E, D4, D5, D6, D7
const int LCD_RS = 23;
const int LCD_E  = 22;
const int LCD_D4 = 21;
const int LCD_D5 = 19;
const int LCD_D6 = 18;
const int LCD_D7 = 5;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// ---------- WiFi ----------
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// ---------- OpenWeather ----------
String apiKey = "8145513da15c12112f0d49df05fa6f37";
String countryCode = "US";
String units = "imperial"; // Fahrenheit

// URL-encoded city names
const char* cities[] = { "San%20Antonio", "Austin", "Dallas", "Houston" };
const char* citiesDisplay[] = { "San Antonio", "Austin", "Dallas", "Houston" };
const int NUM_CITIES = sizeof(cities) / sizeof(cities[0]);
int cityIdx = 0;

// ---------- Buttons ----------
const int BTN_NEXT = 32;     
const int BTN_REFRESH = 33;  

// Debounce settings
unsigned long lastPressNext = 0;
unsigned long lastPressRefresh = 0;
const unsigned long debounceMs = 250;

// Print fixed 20-character line on LCD
void lcdPrintFixed(int col, int row, String text) {
  if (text.length() > 20) text = text.substring(0, 20);
  while (text.length() < 20) text += " ";
  lcd.setCursor(col, row);
  lcd.print(text);
}

void showLCDStatus(String line0, String line1 = "", String line2 = "", String line3 = "") {
  lcd.clear();
  lcdPrintFixed(0, 0, line0);
  lcdPrintFixed(0, 1, line1);
  lcdPrintFixed(0, 2, line2);
  lcdPrintFixed(0, 3, line3);
}

void connectWiFi() {
  Serial.print("Connecting to WiFi");
  showLCDStatus("Connecting WiFi...", "Please wait...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  showLCDStatus("WiFi Connected!", "Ready");
  delay(700);
}

// Global variables for weather display
float gTemp = 0.0;
int gHumidity = 0;
String gDesc = "";

bool getWeatherForCity(const String& cityEncoded) {
  if (WiFi.status() != WL_CONNECTED) return false;

  HTTPClient http;

  String url = "http://api.openweathermap.org/data/2.5/weather?q="
               + cityEncoded + "," + countryCode
               + "&appid=" + apiKey
               + "&units=" + units;

  http.begin(url);
  int httpCode = http.GET();

  Serial.print("HTTP code: ");
  Serial.println(httpCode);

  if (httpCode != 200) {
    Serial.println("Request failed or API error:");
    Serial.println(http.getString());
    showLCDStatus("HTTP Error: " + String(httpCode),
                  "Check URL/API key");
    http.end();
    return false;
  }

  String payload = http.getString();
  http.end();

  JSONVar data = JSON.parse(payload);
  if (JSON.typeof(data) == "undefined") {
    Serial.println("JSON parse failed");
    showLCDStatus("JSON parse failed");
    return false;
  }

  gTemp = (float)(double)data["main"]["temp"];
  gHumidity = (int)data["main"]["humidity"];
  gDesc = (const char*)data["weather"][0]["description"];

  Serial.println("====== Weather Data ======");
  Serial.print("City: "); Serial.println(cityEncoded);
  Serial.print("Temperature (F): "); Serial.println(gTemp);
  Serial.print("Humidity (%): "); Serial.println(gHumidity);
  Serial.print("Condition: "); Serial.println(gDesc);
  Serial.println();

  return true;
}

// Display weather on LCD
void renderLCDWeather() {
  String cityLine = "City: " + String(citiesDisplay[cityIdx]);
  String tempLine = "Temp: " + String(gTemp, 1) + " F";
  String humLine  = "Humidity: " + String(gHumidity) + "%";
  String condLine = "Cond: " + gDesc;

  if (condLine.length() > 20) condLine = condLine.substring(0, 20);

  lcd.clear();
  lcdPrintFixed(0, 0, cityLine);
  lcdPrintFixed(0, 1, tempLine);
  lcdPrintFixed(0, 2, humLine);
  lcdPrintFixed(0, 3, condLine);
}

// Show updating screen
void showUpdating(String reason) {
  showLCDStatus("Updating...", reason, String(citiesDisplay[cityIdx]), "");
}

void setup() {
  Serial.begin(115200);

  lcd.begin(20, 4);
  showLCDStatus("LCD Ready");

  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_REFRESH, INPUT_PULLUP);

  connectWiFi();

  showUpdating("Startup");
  if (getWeatherForCity(cities[cityIdx])) {
    renderLCDWeather();
  }
}

void loop() {
  bool nextPressed = (digitalRead(BTN_NEXT) == LOW);
  bool refreshPressed = (digitalRead(BTN_REFRESH) == LOW);

  unsigned long now = millis();

  // NEXT button: change city and update
  if (nextPressed && (now - lastPressNext > debounceMs)) {
    lastPressNext = now;

    cityIdx = (cityIdx + 1) % NUM_CITIES;

    Serial.print(">> City changed to: ");
    Serial.println(cities[cityIdx]);

    showUpdating("NEXT pressed");
    if (getWeatherForCity(cities[cityIdx])) {
      renderLCDWeather();
    }
  }

  // REFRESH button: update current city
  if (refreshPressed && (now - lastPressRefresh > debounceMs)) {
    lastPressRefresh = now;

    Serial.print(">> Refreshing current city: ");
    Serial.println(cities[cityIdx]);

    showUpdating("REFRESH pressed");
    if (getWeatherForCity(cities[cityIdx])) {
      renderLCDWeather();
    }
  }

  delay(10);
}