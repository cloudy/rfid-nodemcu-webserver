#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define ssid      "akernet"       // WiFi SSID
#define password  "thepasswordis1"  // WiFi password

#define MAXUSERS  13 // Maximum numbers of users on device
#define MAXDOORS 2  //

class User {
  public:

   void setUserName(String name_s) {
    username = name_s;
   }
   void setCard(int fc, int cc) {
    facilitycode = fc;
    cardcode = cc;
   }
   
   void setAccess(int door) {
    access[door] = access[door] ? 0 : 1; 
   }

   String getUserName() {
    return username;
   }
   
   int getAccess(int door) {
    return access[door];
   }

   int getCode(int FC) {
    if (FC)
      return facilitycode;
    return cardcode;
   }

  private:
   int facilitycode = 0;
   int cardcode = 0;
   int access[MAXDOORS];
   String username = "NA";
};

User USERS[MAXUSERS];


const uint8_t GPIOPIN[4] = {LED_BUILTIN,D6,D7,D8};  // Led
float   t = 0 ;
float   h = 0 ;
float   p = 0;
String  GPIOSTATE[4] = {"OFF","OFF","OFF","OFF"};


// Création des objets / create Objects
ESP8266WebServer server ( 80 );

String getPage(){
  String page = "<html lang='en'><head><meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1'/>";
  page += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>";
  page += "<title>RFID Control Center</title></head><body>";
  page += "<div class='container-fluid'>";
  page +=   "<div class='row'>";
  page +=     "<div class='col-md-12'>";
  page +=       "<h1>RFID Control Center</h1>";

  page += userTable(); 
  
  page +=       "<h3>GPIO</h3>";
  page +=       "<div class='row'>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>LED_BUILTIN ";
  page +=           "<span class='badge'>";
  page +=           GPIOSTATE[0];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='LED_BUILTIN' value='1' class='btn btn-success btn-s'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='LED_BUILTIN' value='0' class='btn btn-danger btn-s'>OFF</button></form></div>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D6 ";
  page +=           "<span class='badge'>";
  page +=           GPIOSTATE[1];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D6' value='1' class='btn btn-success btn-s'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D6' value='0' class='btn btn-danger btn-s'>OFF</button></form></div>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D7 ";
  page +=           "<span class='badge'>";
  page +=           GPIOSTATE[2];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D7' value='1' class='btn btn-success btn-s'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D7' value='0' class='btn btn-danger btn-s'>OFF</button></form></div>";
  page +=         "<div class='col-md-4'><h4 class ='text-left'>D8 ";
  page +=           "<span class='badge'>";
  page +=           GPIOSTATE[3];
  page +=         "</span></h4></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D8' value='1' class='btn btn-success btn-s'>ON</button></form></div>";
  page +=         "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='D8' value='0' class='btn btn-danger btn-s'>OFF</button></form></div>";
  page +=       "</div>";
  page +=     "<br>";//<p><a href='http://www.projetsdiy.fr'>www.projetsdiy.fr</p>";
  page += "</div></div></div>";
  page += "</body></html>";
  return page;
}

String userTable(){
  String usertable = "<table class='table'>"; 
  usertable +=         "<thead><tr><th>User</th><th>Front Door Access</th><th>Nathan's Room Access</th><th>Facility Code</th><th>Card Code</th><th>Delete User</th></tr></thead>";
  usertable +=         "<tbody>"; 

  for(int i = 0; i < MAXUSERS; i++){
    usertable += "<tr><td>";
    usertable += USERS[i].getUserName();
    usertable += "</td><td>";
    usertable += USERS[i].getAccess(0);
    usertable += "</td><td>";
    usertable += USERS[i].getAccess(1);
    usertable += "</td><td>";
    usertable += USERS[i].getCode(1);
    usertable += "</td><td>";
    usertable += USERS[i].getCode(0);
    usertable += "</td><td>-</td></tr>";
  }
  usertable +=       "</tbody></table>";
  
  return usertable;
}



void handleRoot(){ 
  if ( server.hasArg("LED_BUILTIN") ) {
    handleLED_BUILTIN();
  } else if ( server.hasArg("D6") ) {
    handleD6();
  } else if ( server.hasArg("D7") ) {
    handleD7();
  } else if ( server.hasArg("D8") ) {
    handleD8();
  } else {
    server.send ( 200, "text/html", getPage() );
  }  
}

void handleLED_BUILTIN() {
  String LED_BUILTINValue; 
  updateGPIO(0,server.arg("LED_BUILTIN")); 
}

void handleD6() {
  String D6Value; 
  updateGPIO(1,server.arg("D6")); 
}

void handleD7() {
  String D7Value; 
  updateGPIO(2,server.arg("D7")); 
}

void handleD8() {
  String D8Value; 
  updateGPIO(3,server.arg("D8")); 
}

void updateGPIO(int gpio, String DxValue) {
  Serial.println("");
  Serial.println("Update GPIO "); Serial.print(GPIOPIN[gpio]); Serial.print(" -> "); Serial.println(DxValue);
  
  if ( DxValue == "1" ) {
    digitalWrite(GPIOPIN[gpio], HIGH);
    GPIOSTATE[gpio] = "On";
    server.send ( 200, "text/html", getPage() );
  } else if ( DxValue == "0" ) {
    digitalWrite(GPIOPIN[gpio], LOW);
    GPIOSTATE[gpio] = "Off";
    server.send ( 200, "text/html", getPage() );
  } else {
    Serial.println("Err Led Value");
  }  
}

void setup() {
  for ( int x = 0 ; x < 5 ; x++ ) { 
    pinMode(GPIOPIN[x],OUTPUT);
  }  
  Serial.begin ( 115200 );

  WiFi.begin ( ssid, password );
  // Attente de la connexion au réseau WiFi / Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); Serial.print ( "." );
  }
  // Connexion WiFi établie / WiFi connexion is OK
  Serial.println ( "" ); 
  Serial.print ( "Connected to " ); Serial.println ( ssid );
  Serial.print ( "IP address: " ); Serial.println ( WiFi.localIP() );

  // On branche la fonction qui gère la premiere page / link to the function that manage launch page 
  server.on ( "/", handleRoot );

  server.begin();
  Serial.println ( "HTTP server started" );
  
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
  t = 0;
  h = 69;
  p = 89;

  delay(1000);
}

