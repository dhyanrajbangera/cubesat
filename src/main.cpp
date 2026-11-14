#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include "wifi_setup.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_BME280.h>
#include <TinyGPS++.h>
#include <math.h>
#include <DHT.h>
#include "dashboard.h"
// Non-blocking sound queue: Wi-Fi and GPS alerts wait for the boot melody.
constexpr uint8_t BUZZER_PIN=13;
struct SoundNote { uint16_t hz,ms; };
// Original "Orbital Arrival": a dramatic 1.28-second startup signature.
// Low launch pulse, rising minor arpeggio, and a bright octave landing.
// Finishes before the four-second intro gives way to live readings.
const SoundNote bootSong[]={
 {220,160},{0,35},{220,80},{0,25},{330,90},{440,90},
 {523,110},{659,120},{0,40},{880,180},{440,90},{880,260}
};
// Short, distinct musical signatures rather than high-pitched beeps.
const SoundNote wifiSong[]={{523,100},{0,25},{659,100},{0,25},{784,220},{0,80}};
const SoundNote gpsSong[]={{659,90},{0,20},{784,90},{0,20},{1047,130},{0,25},{1319,260},{0,80}};
const SoundNote* playingSound=nullptr;
uint8_t soundIndex=0,soundLength=0;
uint32_t soundStarted=0;
bool pendingWifiSound=false,pendingGpsSound=false,previousGpsFix=false;
void startSound(const SoundNote* notes,uint8_t count){
 playingSound=notes;soundLength=count;soundIndex=0;soundStarted=millis();
 if(notes[0].hz)tone(BUZZER_PIN,notes[0].hz);else digitalWrite(BUZZER_PIN,LOW);
}
void serviceSound(){
 if(playingSound && millis()-soundStarted>=playingSound[soundIndex].ms){
  if(playingSound[soundIndex].hz)noTone(BUZZER_PIN);
  if(++soundIndex>=soundLength)playingSound=nullptr;
  else {soundStarted=millis();if(playingSound[soundIndex].hz)tone(BUZZER_PIN,playingSound[soundIndex].hz);}
 }
 if(!playingSound){
  if(pendingWifiSound){pendingWifiSound=false;startSound(wifiSong,sizeof(wifiSong)/sizeof(wifiSong[0]));}
  else if(pendingGpsSound){pendingGpsSound=false;startSound(gpsSong,sizeof(gpsSong)/sizeof(gpsSong[0]));}
 }
}
Adafruit_SSD1306 oled(128,64,&SPI,27,26,25);
Adafruit_BMP280 bmp;
Adafruit_BME280 bme;
TinyGPSPlus gps;
DHT dht(4,DHT11);
float dhtTemp=NAN,dhtHumidity=NAN;
uint32_t lastDht=0;

WebServer server(80);
DNSServer dns;
Preferences settings;
String savedSsid,savedPass,setupToken;
bool joinPending=false,wasConnected=false;
uint32_t joinAt=0;
String networkAddress(){return WiFi.status()==WL_CONNECTED?WiFi.localIP().toString():String("192.168.4.1");}
void serviceNetwork(){
 if(joinPending && millis()-joinAt>=800){joinPending=false;WiFi.disconnect();WiFi.begin(savedSsid.c_str(),savedPass.c_str());}
 bool connected=WiFi.status()==WL_CONNECTED;
 if(connected!=wasConnected){if(connected)pendingWifiSound=true;wasConnected=connected;Serial.print(connected?"Router connected: http://":"Router unavailable; backup: http://");Serial.println(networkAddress());}
}

const IPAddress apIP(192,168,4,1);
bool screen=false,imu=false,baro=false,isBme=false;
float ax=NAN,ay=NAN,az=NAN,gx=NAN,gy=NAN,gz=NAN,roll=NAN,pitch=NAN,temp=NAN,pressure=NAN,humidity=NAN;
uint32_t imuStamp=0,lastSample=0,lastEnv=0,lastDraw=0;
uint32_t introStarted=0;
uint8_t imuAddress=0x68;
float ambientTemp();
String bootId;
struct HistorySample { uint32_t ms; float roll,pitch,temp,pressure; };
const uint16_t historyCapacity=900;
HistorySample historySamples[historyCapacity];
uint16_t historyHead=0,historyCount=0;
uint32_t lastHistory=0;
void recordHistory(uint32_t now){
 if(now-lastHistory<1000)return;
 lastHistory=now;
 historySamples[historyHead]={now,roll,pitch,ambientTemp(),pressure};
 historyHead=(historyHead+1)%historyCapacity;
 if(historyCount<historyCapacity)historyCount++;
}

bool responds(uint8_t a){Wire.beginTransmission(a);return Wire.endTransmission()==0;}
int readReg(uint8_t a,uint8_t r){if(!responds(a))return -1;Wire.beginTransmission(a);Wire.write(r);if(Wire.endTransmission(true))return -1;if(Wire.requestFrom(a,(uint8_t)1)!=1)return -1;return Wire.read();}
int readImuReg(uint8_t a,uint8_t r){return readReg(a,r);}
void writeReg(uint8_t r,uint8_t v){Wire.beginTransmission(imuAddress);Wire.write(r);Wire.write(v);Wire.endTransmission();}
float validReading(float v,float low,float high){return isfinite(v)&&v>=low&&v<=high?v:NAN;}
float ambientTemp(){return isfinite(temp)?temp:dhtTemp;}
String num(double n,int digits=2){return isfinite(n)?String(n,digits):String("null");}
bool fix(){return gps.location.isValid()&&gps.location.age()<3000;}
String telemetry(){
 String s="{\"bootId\":\""+bootId+"\",\"temperatureSource\":\""+String(isfinite(temp)?(isBme?"BME280":"BMP280"):isfinite(dhtTemp)?"DHT11":"unavailable")+"\",\"sampleMs\":"+String(millis())+",\"dhtTemp\":"+num(dhtTemp)+",\"dhtHumidity\":"+num(dhtHumidity)+",\"uptime\":"+String(millis()/1000)+",\"imu\":"+String(imu&&millis()-imuStamp<1000?"true":"false")+",\"baro\":"+String(baro?"true":"false")+",\"fix\":"+String(fix()?"true":"false");
 const char* names[]={"ax","ay","az","gx","gy","gz","roll","pitch","temp","pressure","humidity"};
 float vals[]={ax,ay,az,gx,gy,gz,roll,pitch,ambientTemp(),pressure,isfinite(humidity)?humidity:dhtHumidity};
 for(int i=0;i<11;i++)s+=",\""+String(names[i])+"\":"+num(vals[i]);
 s+=",\"lat\":"+num(fix()?gps.location.lat():NAN,6)+",\"lon\":"+num(fix()?gps.location.lng():NAN,6);
 s+=",\"gpsAgeMs\":"+String(gps.location.isValid()?gps.location.age():uint32_t(0xFFFFFFFF));
 s+=",\"alt\":"+num(fix()&&gps.altitude.isValid()&&gps.altitude.age()<3000?gps.altitude.meters():NAN);
 s+=",\"speed\":"+num(fix()&&gps.speed.isValid()&&gps.speed.age()<3000?gps.speed.kmph():NAN);
 s+=",\"sats\":"+String(gps.satellites.isValid()&&gps.satellites.age()<5000?gps.satellites.value():0)+",\"gpsChars\":"+String(gps.charsProcessed())+"}";return s;
}
void detectSensors(){
 if(!imu){
 for(uint8_t address: {uint8_t(0x68),uint8_t(0x69)}){
 int id=readImuReg(address,0x75);
 Serial.printf("Motion sensor address 0x%02X: WHO_AM_I=%d%s\n",address,id,id<0?" (no readable response)":"");
 if(id==0x70||id==0x71||id==0x73){imuAddress=address;imu=true;break;}
 }
 if(imu){writeReg(0x6B,0x80);delay(100);writeReg(0x6B,1);writeReg(0x6C,0);writeReg(0x1B,0);writeReg(0x1C,0);writeReg(0x1A,3);delay(100);}
 }
 if(!baro)for(uint8_t address: {uint8_t(0x76),uint8_t(0x77)}){
 int bid=readReg(address,0xD0);isBme=bid==0x60;
 if(isBme)baro=bme.begin(address);else if(bid==0x58)baro=bmp.begin(address);
 if(baro)break;
 }
}
// Short non-blocking boot animation; normal sensor/network service continues.
void drawIntro(uint32_t elapsed){
 oled.clearDisplay();oled.setTextSize(1);oled.setTextColor(SSD1306_WHITE);
 oled.setCursor(43,0);oled.print("CUBESAT");
 const float angle=elapsed*0.0012f;
 int px[8],py[8];
 for(int i=0;i<8;i++){
  float x=(i&1)?10:-10,y=(i&2)?10:-10,z=(i&4)?10:-10;
  float rx=x*cosf(angle)+z*sinf(angle),rz=-x*sinf(angle)+z*cosf(angle);
  float ry=y*0.92f-rz*0.39f;
  px[i]=64+int(rx);py[i]=29+int(ry);
 }
 for(int i=0;i<8;i++)for(int bit=1;bit<=4;bit*=2)
  if(!(i&bit))oled.drawLine(px[i],py[i],px[i|bit],py[i|bit],SSD1306_WHITE);
 // Solar wings with individual cells, plus an antenna.
 oled.drawLine(32,29,47,29,SSD1306_WHITE);oled.drawLine(81,29,96,29,SSD1306_WHITE);
 oled.drawRect(17,20,24,18,SSD1306_WHITE);oled.drawRect(87,20,24,18,SSD1306_WHITE);
 for(int x=23;x<41;x+=6)oled.drawLine(x,20,x,37,SSD1306_WHITE);
 for(int x=93;x<111;x+=6)oled.drawLine(x,20,x,37,SSD1306_WHITE);
 oled.drawLine(17,29,40,29,SSD1306_WHITE);oled.drawLine(87,29,110,29,SSD1306_WHITE);
 oled.drawLine(px[2],py[2],px[2]-6,py[2]+5,SSD1306_WHITE);
 oled.setCursor(25,48);oled.print("Made by Dhyan");
 oled.drawRect(24,60,80,3,SSD1306_WHITE);
 oled.fillRect(25,61,min(78,int(elapsed*78UL/4000)),1,SSD1306_WHITE);
 oled.display();
}
void setup(){
 pinMode(BUZZER_PIN,OUTPUT);digitalWrite(BUZZER_PIN,LOW);
 Serial.begin(115200);bootId=String((uint32_t)esp_random(),HEX);dht.begin();Wire.begin(21,22);Wire.setClock(100000);Wire.setTimeOut(30);SPI.begin(18,19,23,25);
 screen=oled.begin(SSD1306_SWITCHCAPVCC,0,true,false);
 Serial.printf("OLED buffer: %s; I2C SDA21=%d SCL22=%d\n",screen?"ready":"FAILED",digitalRead(21),digitalRead(22));
 if(screen){
  introStarted=millis();drawIntro(0);
 }
 Serial2.setRxBufferSize(1024);Serial2.begin(9600,SERIAL_8N1,32,-1);
 detectSensors();
 WiFi.persistent(false);
 settings.begin("cubesat-net",false);
 savedSsid=settings.getString("ssid","");savedPass=settings.getString("pass","");
 setupToken=String((uint32_t)esp_random(),HEX)+String((uint32_t)esp_random(),HEX);
 WiFi.mode(WIFI_AP_STA);
 WiFi.setAutoReconnect(true);
 WiFi.setSleep(false);
 bool configured=WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0));
 bool started=WiFi.softAP("CubeSat-Lab","cubesat2026",1,false,4);
 Serial.printf("Wi-Fi AP: %s; address configuration: %s\n",started?"started":"FAILED",configured?"OK":"FAILED");
 Serial.print("AP address: ");Serial.println(WiFi.softAPIP());
 dns.start(53,"*",apIP);
 if(savedSsid.length())WiFi.begin(savedSsid.c_str(),savedPass.c_str());
 server.on("/wifi",HTTP_GET,[]{
  String page=FPSTR(WIFI_SETUP);page.replace("%TOKEN%",setupToken);
  server.sendHeader("Cache-Control","no-store");server.send(200,"text/html; charset=utf-8",page);
 });
 server.on("/api/network",HTTP_GET,[]{
  bool connected=WiFi.status()==WL_CONNECTED;
  server.sendHeader("Cache-Control","no-store");
  server.send(200,"application/json",String("{\"connected\":")+(connected?"true":"false")+",\"configured\":"+(savedSsid.length()?"true":"false")+",\"ip\":\""+networkAddress()+"\",\"rssi\":"+String(connected?WiFi.RSSI():0)+"}");
 });
 server.on("/wifi/save",HTTP_POST,[]{
  if(server.arg("token")!=setupToken){server.send(403,"text/plain","Reload the Wi-Fi setup page and try again.");return;}
  String ssid=server.arg("ssid"),pass=server.arg("password");
  if(ssid.length()<1||ssid.length()>32||pass.length()>63||(pass.length()>0&&pass.length()<8)){
   server.send(400,"text/plain","Use a network name up to 32 bytes and a password of 8–63 characters (or blank for an open network).");return;
  }
  savedSsid=ssid;savedPass=pass;settings.putString("ssid",savedSsid);settings.putString("pass",savedPass);
  joinPending=true;joinAt=millis();server.sendHeader("Cache-Control","no-store");server.send(200,"text/plain","Saved. Joining your network; check status below. If it fails, correct your settings here.");
 });
 auto serveDashboard=[](){
  server.sendHeader("Cache-Control","no-store, no-cache, must-revalidate");
  server.send_P(200,"text/html; charset=utf-8",DASHBOARD);
 };
 server.on("/",HTTP_GET,serveDashboard);
 server.on("/dashboard",HTTP_GET,serveDashboard);
 server.on("/index.html",HTTP_GET,serveDashboard);
 server.on("/api/telemetry",[]{server.sendHeader("Cache-Control","no-store");server.send(200,"application/json",telemetry());});
 server.on("/api/history",[]{
  String body;body.reserve(historyCount*110+100);
  body="{\"bootId\":\""+bootId+"\",\"sampleMs\":"+String(millis())+",\"samples\":[";
  for(uint16_t i=0;i<historyCount;i++){
   auto &h=historySamples[(historyHead+historyCapacity-historyCount+i)%historyCapacity];
   if(i)body+=",";
   body+="{\"sampleMs\":"+String(h.ms)+",\"roll\":"+num(h.roll)+",\"pitch\":"+num(h.pitch)+",\"temp\":"+num(h.temp)+",\"pressure\":"+num(h.pressure)+"}";
  }
  body+="]}";server.sendHeader("Cache-Control","no-store");server.send(200,"application/json",body);
 });
 // Phone network-check URLs lead to the local dashboard, not the internet.
 server.onNotFound([]{
  server.sendHeader("Location",String("http://")+server.client().localIP().toString()+"/",true);
  server.sendHeader("Cache-Control","no-store");
  server.send(302,"text/plain","Open the dashboard at the OLED address using http:// (not https://).");
 });
 server.begin();
 Serial.println("CubeSat-Lab / cubesat2026 / http://192.168.4.1");
 startSound(bootSong,sizeof(bootSong)/sizeof(bootSong[0]));
 // Start the visible startup animation.
 if(screen){introStarted=millis();drawIntro(0);}
}
void loop(){
 serviceSound();
 static bool hadGpsFix=false;
 bool gpsFixed=fix();
 if(gpsFixed&&!hadGpsFix)pendingGpsSound=true;
 hadGpsFix=gpsFixed;
 while(Serial2.available())gps.encode(Serial2.read());dns.processNextRequest();server.handleClient();serviceNetwork();uint32_t now=millis();
 if(now-lastSample>=10){lastSample=now;
 if(imu){Wire.beginTransmission(imuAddress);Wire.write(0x3B);bool ok=Wire.endTransmission(true)==0;
 if(ok&&Wire.requestFrom(imuAddress,(uint8_t)14)==14){int16_t v[7];for(int i=0;i<7;i++)v[i]=(int16_t)((Wire.read()<<8)|Wire.read());
 ax=v[0]/16384.0;ay=v[1]/16384.0;az=v[2]/16384.0;gx=v[4]/131.0;gy=v[5]/131.0;gz=v[6]/131.0;
 float ar=atan2(ay,az)*180/PI,ap=atan2(-ax,sqrt(ay*ay+az*az))*180/PI;
 float dt=(now-imuStamp)*0.001f;
 if(!isfinite(roll)||dt>0.1f){roll=ar;pitch=ap;}
 else {float predicted=roll+gx*dt;float error=ar-predicted;while(error>180)error-=360;while(error< -180)error+=360;
 float weight=dt/(0.35f+dt);float magnitude=sqrt(ax*ax+ay*ay+az*az);if(fabs(magnitude-1)>0.15f)weight=0;
 roll=predicted+weight*error;while(roll>180)roll-=360;while(roll< -180)roll+=360;
 pitch=(1-weight)*(pitch+gy*dt)+weight*ap;}imuStamp=now;
 }else{imu=false;ax=ay=az=gx=gy=gz=roll=pitch=NAN;}}
 }
 static uint32_t lastRetry=0;
 if(now-lastRetry>=10000&&(!imu||!baro)){lastRetry=now;detectSensors();}
 if(now-lastDht>=2500){lastDht=now;dhtTemp=validReading(dht.readTemperature(),0,50);dhtHumidity=validReading(dht.readHumidity(),20,90);if(!isfinite(dhtHumidity))dhtTemp=NAN;}
 if(now-lastEnv>=1000){lastEnv=now;if(baro){temp=isBme?bme.readTemperature():bmp.readTemperature();pressure=(isBme?bme.readPressure():bmp.readPressure())/100;humidity=isBme?validReading(bme.readHumidity(),0,100):NAN;temp=validReading(temp,-40,85);pressure=validReading(pressure,300,1100);if(!isfinite(pressure))temp=NAN;}}
 if(screen&&now-introStarted<4000&&now-lastDraw>=60){lastDraw=now;drawIntro(now-introStarted);}
 if(screen&&now-introStarted>=4000&&now-lastDraw>=100){lastDraw=now;String text=String(ambientTemp(),1)+"/"+String(roll,0)+"/"+String(pitch,0)+"/"+String(pressure,0)+"/"+String(fix())+"/"+String(gps.satellites.value())+"/"+networkAddress();static String old;if(text!=old){old=text;oled.clearDisplay();oled.setTextSize(1);oled.setTextColor(SSD1306_WHITE);oled.fillRect(0,0,128,11,SSD1306_WHITE);oled.setTextColor(SSD1306_BLACK);oled.setCursor(4,2);oled.print("CUBESAT / LIVE");oled.setTextColor(SSD1306_WHITE);oled.setCursor(0,15);oled.print("TEMP ");if(isfinite(ambientTemp()))oled.print(ambientTemp(),1);else oled.print("--");oled.print(" C");oled.setCursor(0,26);oled.print("R ");oled.print(isfinite(roll)?String(roll,0):"--");oled.print("  P ");oled.print(isfinite(pitch)?String(pitch,0):"--");oled.setCursor(0,37);oled.print("PRESS ");oled.print(isfinite(pressure)?String(pressure,0):"--");oled.print(" hPa");oled.setCursor(0,48);oled.print(fix()?"GPS FIX":"GPS WAIT");oled.print(" / SAT ");oled.print(gps.satellites.value());oled.setCursor(0,57);oled.print(networkAddress());oled.display();}}
 recordHistory(now);
 static uint32_t lastReport=0;
 if(now-lastReport>=5000){lastReport=now;Serial.println(telemetry());}
 delay(1);
}
