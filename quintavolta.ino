#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
//------------------------------------------------------------------------------------------------------------------------------------------------

//Impostazioni wiFi
const char* ssid = "FRITZ - 2.4 GHz";
const char* password = "17969564294078586789";

//Impostazioni MQTT
const char* mqtt_server = "cec9083493024ef58ce2c2cd6a476130.s2.eu.hivemq.cloud";
const int mqtt_port = 8883;
const int mqtt_socket = 8884;

//Credenziali MQTT
const char* mqtt_username = "tommyUnibo";
const char* mqtt_password = "tommyUnibo";

//Topic MQTT
const char* mqtt_temp_topic = "tempTopic";
const char* mqtt_hum_topic = "humTopic";

//Impostazioni Sensore Temperatura/Umidità
#define DHTPIN 5 //GPIO 5 Digital port 1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//Valori base di temperatura/umidità
float standardTemp = 0.0;
float standardHum = 0.0;

//------------------------------------------------------------------------------------------------------------------------------------------------

//Certificato per client WiFi Secure
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

X509List cert(root_ca);
//------------------------------------------------------------------------------------------------------------------------------------------------

//Definizione client WiFi e MQTT
WiFiClientSecure espClient;
PubSubClient client(espClient);
//------------------------------------------------------------------------------------------------------------------------------------------------

//Definizione HTML pagina raccolta dati
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.3.1/dist/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
  <style>
    @import url("https://fonts.googleapis.com/css2?family=Montserrat:wght@300&display=swap");

    * {
        font-family: "Montserrat", sans-serif;
        user-select: none;
    }

    html {
        margin: 0px auto;
        text-align: center;
        margin: 0px;
        padding: 0;
        height: 100%;
        overflow-x: hidden;
        scroll-behavior: smooth;
    }

    h2 { font-size: 3.0rem; }

    body {
      /* background: linear-gradient(to bottom right, #102e4f 0%, #2261af 100%); */
      background-image: url(https://svgshare.com/i/11iY.svg);
      background-repeat: no-repeat;
      background-size: cover;
    }

    h2 {
      color: white;
    }

    .server{
        font-weight: bold;
        font-size: 45px;
        color: rgb(209, 233, 236);
        margin: 0px;
        filter: drop-shadow(0px 0px 10px rgba(255, 242, 242, 0.733));
    }

    h4 {
      color: white;
      font-size: 16px;
    }

    .title {
      margin-left: 2.5em;
      margin-top: 2.5em;
      cursor: default;
    }

    .profile{
      cursor: default;
      color: white;
      position: absolute;
      right: 0;
      margin-right: 2.5em;
      margin-top: 2.5em;
      text-align: end;
      padding: 2vh;
      border: 1px solid white;
      border-radius: 20px;
      transition: all 0.2s ease-in-out;
    }

    .profile:hover {
      transform: scale(1.1, 1.1);
    }

    .profile > p {
      margin-bottom: 0px;
    }

    .name {
      font-weight: bold;
    }

    .email, .matricola {
      font-style: italic;
    }

    .header {
      display: flex;
    }

    hr {
      border: none;
      height: 1px;
      background-color: #ffffff;
      margin: 10px 0;
    }

    .hover-temp, .hover-hum {
      transition: all 0.2s ease-out;
    }

    .hover-temp:hover, span#temperature {
      background: linear-gradient(to right, #f3bd0a 0%, #eb2b09 100%);
      background-clip: text;
      -webkit-background-clip: text;
      color: transparent;
    }

    .hover-hum:hover, span#humidity {
      background: linear-gradient(to right, #0af3a5 0%, #09b6eb 100%);
      background-clip: text;
      -webkit-background-clip: text;
      color: transparent;
    }

    .content {
      display: block;
      position: absolute;
      top: 40%;
      left: 10%;
      background:transparent;
      backdrop-filter:blur(20px);
      padding: 5vh;
      border-radius: 30px;
      box-shadow: rgb(0, 2, 3) 0px 20px 30px -10px;
      border: 1px solid rgba(211, 202, 202, 0.842);
      transition: all 0.3s ease-in-out;
    }

    .content:hover {
      transform: scale(1.1, 1.1);
    }

    .balloon{
      display: block;
      position: absolute;
      top: 25%;
      right: 10%;
      transition: all 0.3s ease-in-out;
    }

    .balloon:hover {
      transform: scale(1.2, 1.2);
    }

    .temperatura, .umidita {
      color: white;
      font-size: 2.2em;
      font-weight: bold;
    }

    .umidita{
      margin-top: 1.5em;
    }

    .units {
      align-items: baseline;
    }

    .tooltip {
      color: white;
      opacity: 1;
      border: 1px solid white;
      border-radius: 10px;
      padding: 1em;
      display: none;
      transition: all 0.3s ease-in-out;
    }

    .btn:hover+.tooltip{
      display: inline-flex;
    }
    
    .libraries>p, .devices>p {
      margin: 0;
      font-weight: bold;
    }

    .btn {
      display: block;
      width: fit-content;
      border: 1px solid white;
      border-radius: 10px;
      transition: all 0.3s ease-in-out;
    }

    .btn:hover{
      font-weight: bolder;
      color: #113359;
      transform: scale(1.05, 1.05);
    }
</style>
</head>
<body>
  <div class="header container-md container-fluid">
    <div class="title">
      <h2><span class="server">DHT22 | <span class="hover-temp">Temperatura</span> & <span class="hover-hum">Umidità</span></h2>
      <hr>
      <div class="menu">
        <h4 class="btn btn-outline-light">Web Server</span></h4>
        <div class="tooltip">
          <div class="libraries">
            <p>Librerie Utilizzate:</p>
            <ul>
              <li>ESP8266WiFi</li>
              <li>WiFiClientSecure</li>
              <li>PubSubClient</li>
              <li>ESP8266WebServer</li>
              <li>DHT</li>
            </ul>
          </div>
          <div class="devices">
            <p>Dispositivi:</p>
            <ul>
              <li>ESP8266</li>
              <li>DHT22</li>
            </ul>
          </div>
        </div>
      </div>
      </div>
    <div class="profile">
      <p class="name">Bagnolini Tommaso</p>
      <p class="email">tommaso.bagnolini@studio.unibo.it</p>
      <p class="matricola">Mat.: 0001071116</p>
    </div>
  </div>
  <div class="content">
    <div class="measurements">
      <div class="temperatura">
        <p>
          <i class="fas fa-thermometer-half" style="color: rgb(255, 255, 255)"></i> 
          <span class="dht-labels temp">Temperatura:</span> 
          <span id="temperature">%TEMPERATURE%</span>
          <sup class="units">°C</sup>
        </p>
      </div>
      <div class="umidita">
        <p>
          <i class="fas fa-tint" style="color:#ffffff;"></i> 
          <span class="dht-labels hum">Umidità:</span>
          <span id="humidity">%HUMIDITY%</span>
          <sup class="units">%</sup>
        </p>
      </div>
    </div>
  </div>
  <div class="balloon">
    <script src="https://unpkg.com/@dotlottie/player-component@latest/dist/dotlottie-player.mjs" type="module"></script>
    <dotlottie-player src="https://lottie.host/fb8cbfd0-ac65-4d81-9e32-2b0fdbb34291/54JiYD0JcZ.json" background="transparent" speed="1" style="width: 550px; height: 550px" direction="1" mode="normal" loop autoplay></dotlottie-player>
  </div>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";
//------------------------------------------------------------------------------------------------------------------------------------------------

//Funzione per la sostituzione e visualizzazione dati nella pagina HTML
String processor(const String& var){
  if(var == "TEMPERATURE"){
    return String(standardTemp);
  }
  else if(var == "HUMIDITY"){
    return String(standardHum);
  }
  return String();
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//Definizione client http
ESP8266WebServer server(80);
//------------------------------------------------------------------------------------------------------------------------------------------------

//Definizione di funzione di connessione WiFi
void setup_wifi() {
  delay(10);
  
  dht.begin();

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Serial.println("\nValori debug: ");
  // Serial.println(mqtt_server);
  // Serial.println(mqtt_password);
  // Serial.println(mqtt_username);
  // Serial.println(mqtt_port);
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//Definizione di funzione di connessione MQTT
void reconnect() {
  // Loop until we’re reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-"; // Create a random client ID
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(mqtt_temp_topic);
      client.subscribe(mqtt_hum_topic);
    }else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//Definizione ROUTES:
//Definizione handleHome
void handleHome() {
  server.send(200, "text/html", index_html);
}

//Definizione handleTemp
void handleTemp() {
  server.send(200, "text/plain", String(standardTemp).c_str());
}

//Definizione handleHum
void handleHum() {
  server.send(200, "text/plain", String(standardHum).c_str());
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//Dichiarazione delle ROUTES einizializzazione server
void setupRouting() {
  server.on("/", handleHome);
  server.on("/temperature", handleTemp);
  server.on("/humidity", handleHum);
  server.begin();
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//Setup
void setup() {
  configTime(0, 0, "pool.ntp.org");   // get UTC time via NTP
  espClient.setTrustAnchors(&cert);      // Add root certificate for api.telegram.org

  Serial.begin(9600);
  
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  setupRouting();
}
//------------------------------------------------------------------------------------------------------------------------------------------------

//Loop
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  server.handleClient();

  //Lettura temperatura
  float newTemp = dht.readTemperature();
  if(isnan(newTemp)) {
    Serial.println("Fallimento nella lettura del sensore DHT22 (temperatura)");
  }else{
    standardTemp = newTemp;
    // Serial.println(standardTemp);
    client.publish(mqtt_temp_topic, String(standardTemp).c_str());
  }

  //Lettura Umidità
  float newHum = dht.readHumidity();
  if(isnan(newHum)) {
    Serial.println("Fallimento nella lettura del sensore DHT22 (umidità)");
  }else{
    standardHum = newHum;
    // Serial.println(standardHum);
    client.publish(mqtt_hum_topic, String(standardHum).c_str());
  }

  delay(5000); // Ritardo prima di inviare nuovamente i dati  
}
