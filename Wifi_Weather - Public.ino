#include <JSON_Decoder.h>
#include <OpenWeatherMod.h>
#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include <Time.h>
#include <WiFi.h>
#include <cstring>
#define ARDUINOJSON_USE_LONG_LONG 1
#define ARDUINOJSON_USE_DOUBLE 0
#include <ArduinoJson.h>
#include <WiFi.h>

// =====================================================
// ========= User configured stuff starts here =========
// Further configuration settings can be found in the
// OpenWeather library "User_Setup.h" file

// Just using this library for unix time conversion

#define TIME_OFFSET -25,200,000 // UTC + 0 hour

  String api_key = "Not today hacker person";
  String language = "en";
  String ssid;
  String pass = "hello";
  byte Nnet;
  int Snet;
  int i;
  const char *Nname = ssid.c_str();
  const char *Npass = pass.c_str();
  char server[] = "geocoding.geo.census.gov";
  const char*  host = "geocoding.geo.census.gov";
  String url = "https://geocoding.geo.census.gov/geocoder/geographies/address?street=4600+Silver+Hill+Rd&city=Washington&state=DC&benchmark=Public_AR_Census2020&vintage=Census2020_Census2020&layers=10&format=json";
  bool printWebData = true;
  unsigned long beginMicros, endMicros;
  unsigned long byteCount = 0;
  DynamicJsonDocument doc(6144);
  WiFiClientSecure client;
  DeserializationError error;
  int x = 0; 
  char lf = 10;
  String readString, readString1;
  OW_Weather ow;
  OW_Weather geocoder;
  String units = "imperial";
  String latitude;
  String longitude;
  String address;
  



void setup() {
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
  ScanNetwork();
  LoginQuery();
  Login();
  Serial.println (" WiFi Connection Established ");
  AddressQuery();
  DataGet();
  DataProcess();
  }
}


void loop() {
  WeatherPull();
  delay(5 * 60 * 1000); 
    }



void ScanNetwork()
{
  byte Nnet = 0;
  Nnet = WiFi.scanNetworks();
  Serial.printf("Number of Networks : %d \n", Nnet);
    for (int Snet = 0; Snet<Nnet; Snet++) {
    Serial.print(Snet);
    Serial.print(") Network: ");
    Serial.println(WiFi.SSID(Snet));
   }
  }

void LoginQuery()
{
  Serial.println("Choose network number :");

  while(Serial.available() == 0){}
  Snet = Serial.parseInt();
  
  Serial.println(Snet);
  ssid = (WiFi.SSID(Snet));
  Serial.printf("You have selected : %s \n", ssid);
  Serial.printf("What is the password for the %s network? \n", ssid);
  delay(1000);
  
  while(Serial.available() == 0){}
  pass = Serial.readStringUntil('\n');
  Npass = pass.c_str();
  Nname = ssid.c_str();
}
  

void Login(){

  WiFi.begin (Nname, Npass);
    for ( i = 0; i<10; i++ ) {
      Serial.print("Connecting");
      delay(300);
      Serial.print(".");
      delay(300);
      Serial.print(".");
      delay(300);
      Serial.println(".");
      delay(100);
      if (WiFi.status() == WL_CONNECTED){i = 10;}
      }
  if (i == 10 && WiFi.status() != WL_CONNECTED ){ Serial.println("Connection Failed");}
  }

void WeatherPull()
{
  // Create the structures that hold the retrieved weather
  OW_current *current = new OW_current;
  OW_hourly *hourly = new OW_hourly;
  OW_daily  *daily = new OW_daily;

  Serial.print("\nRequesting weather information from OpenWeather... ");

  //On the ESP8266 (only) the library by default uses BearSSL, another option is to use AXTLS
  //For problems with ESP8266 stability, use AXTLS by adding a false parameter thus       vvvvv
  //ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language, false);

  ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language);

  Serial.println("Weather from Open Weather\n");

  // Position as reported by Open Weather
  Serial.print("Latitude            : "); Serial.println(ow.lat);
  Serial.print("Longitude           : "); Serial.println(ow.lon);
  // We can use the timezone to set the offset eventually...
  Serial.print("Timezone            : "); Serial.println(ow.timezone);
  Serial.println();

  if (current)
  {
    Serial.println("############### Current weather ###############\n");
    Serial.print("dt (time)        : "); Serial.print(strTime(current->dt));
    Serial.print("sunrise          : "); Serial.print(strTime(current->sunrise));
    Serial.print("sunset           : "); Serial.print(strTime(current->sunset));
    Serial.print("temp             : "); Serial.println(current->temp);
    Serial.print("feels_like       : "); Serial.println(current->feels_like);
    Serial.print("pressure         : "); Serial.println(current->pressure);
    Serial.print("humidity         : "); Serial.println(current->humidity);
    Serial.print("dew_point        : "); Serial.println(current->dew_point);
    Serial.print("uvi              : "); Serial.println(current->uvi);
    Serial.print("clouds           : "); Serial.println(current->clouds);
    Serial.print("visibility       : "); Serial.println(current->visibility);
    Serial.print("wind_speed       : "); Serial.println(current->wind_speed);
    Serial.print("wind_gust        : "); Serial.println(current->wind_gust);
    Serial.print("wind_deg         : "); Serial.println(current->wind_deg);
    Serial.print("rain             : "); Serial.println(current->rain);
    Serial.print("snow             : "); Serial.println(current->snow);
    Serial.println();
    Serial.print("id               : "); Serial.println(current->id);
    Serial.print("main             : "); Serial.println(current->main);
    Serial.print("description      : "); Serial.println(current->description);
    Serial.print("icon             : "); Serial.println(current->icon);

    Serial.println();
  }

  if (hourly)
  {
    Serial.println("############### Hourly weather  ###############\n");
    for (int i = 0; i < MAX_HOURS; i++)
    {
      Serial.print("Hourly summary  "); if (i < 10) Serial.print(" "); Serial.print(i);
      Serial.println();
      Serial.print("dt (time)        : "); Serial.print(strTime(hourly->dt[i]));
      Serial.print("temp             : "); Serial.println(hourly->temp[i]);
      Serial.print("feels_like       : "); Serial.println(hourly->feels_like[i]);
      Serial.print("pressure         : "); Serial.println(hourly->pressure[i]);
      Serial.print("humidity         : "); Serial.println(hourly->humidity[i]);
      Serial.print("dew_point        : "); Serial.println(hourly->dew_point[i]);
      Serial.print("clouds           : "); Serial.println(hourly->clouds[i]);
      Serial.print("wind_speed       : "); Serial.println(hourly->wind_speed[i]);
      Serial.print("wind_gust        : "); Serial.println(hourly->wind_gust[i]);
      Serial.print("wind_deg         : "); Serial.println(hourly->wind_deg[i]);
      Serial.print("rain             : "); Serial.println(hourly->rain[i]);
      Serial.print("snow             : "); Serial.println(hourly->snow[i]);
      Serial.println();
      Serial.print("id               : "); Serial.println(hourly->id[i]);
      Serial.print("main             : "); Serial.println(hourly->main[i]);
      Serial.print("description      : "); Serial.println(hourly->description[i]);
      Serial.print("icon             : "); Serial.println(hourly->icon[i]);
      Serial.print("pop              : "); Serial.println(hourly->pop[i]);

      Serial.println();
    }
  }

  if (daily)
  {
    Serial.println("###############  Daily weather  ###############\n");
    for (int i = 0; i < MAX_DAYS; i++)
    {
      Serial.print("Daily summary   "); if (i < 10) Serial.print(" "); Serial.print(i);
      Serial.println();
      Serial.print("dt (time)        : "); Serial.print(strTime(daily->dt[i]));
      Serial.print("sunrise          : "); Serial.print(strTime(daily->sunrise[i]));
      Serial.print("sunset           : "); Serial.print(strTime(daily->sunset[i]));

      Serial.print("temp.morn        : "); Serial.println(daily->temp_morn[i]);
      Serial.print("temp.day         : "); Serial.println(daily->temp_day[i]);
      Serial.print("temp.eve         : "); Serial.println(daily->temp_eve[i]);
      Serial.print("temp.night       : "); Serial.println(daily->temp_night[i]);
      Serial.print("temp.min         : "); Serial.println(daily->temp_min[i]);
      Serial.print("temp.max         : "); Serial.println(daily->temp_max[i]);

      Serial.print("feels_like.morn  : "); Serial.println(daily->feels_like_morn[i]);
      Serial.print("feels_like.day   : "); Serial.println(daily->feels_like_day[i]);
      Serial.print("feels_like.eve   : "); Serial.println(daily->feels_like_eve[i]);
      Serial.print("feels_like.night : "); Serial.println(daily->feels_like_night[i]);

      Serial.print("pressure         : "); Serial.println(daily->pressure[i]);
      Serial.print("humidity         : "); Serial.println(daily->humidity[i]);
      Serial.print("dew_point        : "); Serial.println(daily->dew_point[i]);
      Serial.print("uvi              : "); Serial.println(daily->uvi[i]);
      Serial.print("clouds           : "); Serial.println(daily->clouds[i]);
      Serial.print("visibility       : "); Serial.println(daily->visibility[i]);
      Serial.print("wind_speed       : "); Serial.println(daily->wind_speed[i]);
      Serial.print("wind_gust        : "); Serial.println(daily->wind_gust[i]);
      Serial.print("wind_deg         : "); Serial.println(daily->wind_deg[i]);
      Serial.print("rain             : "); Serial.println(daily->rain[i]);
      Serial.print("snow             : "); Serial.println(daily->snow[i]);
      Serial.println();
      Serial.print("id               : "); Serial.println(daily->id[i]);
      Serial.print("main             : "); Serial.println(daily->main[i]);
      Serial.print("description      : "); Serial.println(daily->description[i]);
      Serial.print("icon             : "); Serial.println(daily->icon[i]);
      Serial.print("pop              : "); Serial.println(daily->pop[i]);

      Serial.println();
    }
  }

  // Delete to free up space and prevent fragmentation as strings change in length
  delete current;
  delete hourly;
  delete daily;
}

/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
String strTime(time_t unixTime)
{
  unixTime += TIME_OFFSET;
  return ctime(&unixTime);
}
   
void DataProcess(){
    Serial.println("Data Process Function");
    char *jsonfeed = new char[geocoder.geocoderstring.length() + 1];
  strcpy(jsonfeed, geocoder.geocoderstring.c_str());
  const char s[2] = "\n";
  String json = strtok(jsonfeed, s);
  String jsonlist[10];
  int jsonfind = 0;
      while( json != NULL ) {
    Serial.print(jsonfind);
    Serial.print(") line: ");
    Serial.println(json);
    jsonlist[jsonfind] += json;
    json = strtok(NULL, s);
    jsonfind++;
   }
   jsonlist[jsonfind] += json;
  DeserializationError error = deserializeJson(doc, jsonlist[1]);
  
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    delay (2000);
  }
  Serial.println(" Census GeoCoder JSON has been downloaded");
  DataParse();
}

void DataParse(){
  JsonObject result_input = doc["result"]["input"];

JsonObject result_input_benchmark = result_input["benchmark"];
const char* result_input_benchmark_id = result_input_benchmark["id"]; // "2020"
const char* result_input_benchmark_benchmarkName = result_input_benchmark["benchmarkName"];
const char* result_input_benchmark_benchmarkDescription = result_input_benchmark["benchmarkDescription"];
bool result_input_benchmark_isDefault = result_input_benchmark["isDefault"]; // false

JsonObject result_input_vintage = result_input["vintage"];
const char* result_input_vintage_id = result_input_vintage["id"]; // "2020"
const char* result_input_vintage_vintageName = result_input_vintage["vintageName"];
const char* result_input_vintage_vintageDescription = result_input_vintage["vintageDescription"];
bool result_input_vintage_isDefault = result_input_vintage["isDefault"]; // true

JsonObject result_input_address = result_input["address"];
const char* result_input_address_street = result_input_address["street"]; // "4600 Silver Hill Rd"
const char* result_input_address_city = result_input_address["city"]; // "Washington"
const char* result_input_address_state = result_input_address["state"]; // "DC"

JsonObject result_addressMatches_0 = doc["result"]["addressMatches"][0];
const char* result_addressMatches_0_matchedAddress = result_addressMatches_0["matchedAddress"]; // "4600 ...

double result_addressMatches_0_coordinates_x = result_addressMatches_0["coordinates"]["x"]; // -76.92744
double result_addressMatches_0_coordinates_y = result_addressMatches_0["coordinates"]["y"]; // 38.845985

const char* result_addressMatches_0_tigerLine_tigerLineId = result_addressMatches_0["tigerLine"]["tigerLineId"];
const char* result_addressMatches_0_tigerLine_side = result_addressMatches_0["tigerLine"]["side"];

JsonObject result_addressMatches_0_addressComponents = result_addressMatches_0["addressComponents"];
const char* result_addressMatches_0_addressComponents_fromAddress = result_addressMatches_0_addressComponents["fromAddress"];
const char* result_addressMatches_0_addressComponents_toAddress = result_addressMatches_0_addressComponents["toAddress"];
const char* result_addressMatches_0_addressComponents_preQualifier = result_addressMatches_0_addressComponents["preQualifier"];
const char* result_addressMatches_0_addressComponents_preDirection = result_addressMatches_0_addressComponents["preDirection"];
const char* result_addressMatches_0_addressComponents_preType = result_addressMatches_0_addressComponents["preType"];
const char* result_addressMatches_0_addressComponents_streetName = result_addressMatches_0_addressComponents["streetName"];
const char* result_addressMatches_0_addressComponents_suffixType = result_addressMatches_0_addressComponents["suffixType"];
const char* result_addressMatches_0_addressComponents_suffixDirection = result_addressMatches_0_addressComponents["suffixDirection"];
const char* result_addressMatches_0_addressComponents_suffixQualifier = result_addressMatches_0_addressComponents["suffixQualifier"];
const char* result_addressMatches_0_addressComponents_city = result_addressMatches_0_addressComponents["city"];
const char* result_addressMatches_0_addressComponents_state = result_addressMatches_0_addressComponents["state"];
const char* result_addressMatches_0_addressComponents_zip = result_addressMatches_0_addressComponents["zip"];

JsonObject result_addressMatches_0_geographies_Census_Blocks_0 = result_addressMatches_0["geographies"]["Census Blocks"][0];
const char* result_addressMatches_0_geographies_Census_Blocks_0_SUFFIX = result_addressMatches_0_geographies_Census_Blocks_0["SUFFIX"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_POP100 = result_addressMatches_0_geographies_Census_Blocks_0["POP100"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_GEOID = result_addressMatches_0_geographies_Census_Blocks_0["GEOID"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_CENTLAT = result_addressMatches_0_geographies_Census_Blocks_0["CENTLAT"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_BLOCK = result_addressMatches_0_geographies_Census_Blocks_0["BLOCK"];
int result_addressMatches_0_geographies_Census_Blocks_0_AREAWATER = result_addressMatches_0_geographies_Census_Blocks_0["AREAWATER"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_STATE = result_addressMatches_0_geographies_Census_Blocks_0["STATE"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_BASENAME = result_addressMatches_0_geographies_Census_Blocks_0["BASENAME"];
long long result_addressMatches_0_geographies_Census_Blocks_0_OID = result_addressMatches_0_geographies_Census_Blocks_0["OID"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_LSADC = result_addressMatches_0_geographies_Census_Blocks_0["LSADC"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_FUNCSTAT = result_addressMatches_0_geographies_Census_Blocks_0["FUNCSTAT"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_INTPTLAT = result_addressMatches_0_geographies_Census_Blocks_0["INTPTLAT"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_NAME = result_addressMatches_0_geographies_Census_Blocks_0["NAME"];
long result_addressMatches_0_geographies_Census_Blocks_0_OBJECTID = result_addressMatches_0_geographies_Census_Blocks_0["OBJECTID"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_TRACT = result_addressMatches_0_geographies_Census_Blocks_0["TRACT"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_CENTLON = result_addressMatches_0_geographies_Census_Blocks_0["CENTLON"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_BLKGRP = result_addressMatches_0_geographies_Census_Blocks_0["BLKGRP"];
long result_addressMatches_0_geographies_Census_Blocks_0_AREALAND = result_addressMatches_0_geographies_Census_Blocks_0["AREALAND"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_HU100 = result_addressMatches_0_geographies_Census_Blocks_0["HU100"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_INTPTLON = result_addressMatches_0_geographies_Census_Blocks_0["INTPTLON"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_MTFCC = result_addressMatches_0_geographies_Census_Blocks_0["MTFCC"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_LWBLKTYP = result_addressMatches_0_geographies_Census_Blocks_0["LWBLKTYP"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_UR = result_addressMatches_0_geographies_Census_Blocks_0["UR"];
const char* result_addressMatches_0_geographies_Census_Blocks_0_COUNTY = result_addressMatches_0_geographies_Census_Blocks_0["COUNTY"];

  Serial.println(result_addressMatches_0_coordinates_x);
  longitude = String(result_addressMatches_0_coordinates_x); 
  Serial.println(result_addressMatches_0_coordinates_y);
  latitude = String(result_addressMatches_0_coordinates_y); 
}


void DataGet(){
    client.setInsecure();
    while (!client.connect(server, 443)) {
    Serial.println(server);
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    while(Serial.available() == 0){}
    String Sread = Serial.readStringUntil(' ');
    int l = Sread.length();
    strcpy(server, Sread.c_str());
    Serial.println("newaddress check");
    }
  
    
     Serial.println("Valid Connection"); 
     
     if (client.connect(server, 443)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    OW_geo *geo = new OW_geo;
    geocoder.getGeo(geo, address);

  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
    while(Serial.available() == 0){}
    Serial.println("newaddress check");
  }
  }

void AddressQuery(){
  Serial.println("Please enter the House number & Street name: ");
  while(Serial.available() == 0){}
  address = Serial.readStringUntil('\n');
  Serial.println(address);
  int addi = address.length();
  char addpre[addi + 1];
  strcpy(addpre, address.c_str());
    for (int i = 0; i < address.length(); ++i) {
    if (addpre[i] == ' ')
      addpre[i] = '+';
  }
  Serial.println("Please enter Zipcode");
  while(Serial.available() == 0){}
  address = String(addpre);
  address = address + "%2C+";
  String adds = Serial.readStringUntil('\n');
  Serial.println(adds);
  address = address + adds;
  Serial.println(address);
  }
  
  
