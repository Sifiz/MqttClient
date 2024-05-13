#include <Arduino.h>  //not needed in the arduino ide

// Captive Portal
#include <AsyncTCP.h>  //https://github.com/me-no-dev/AsyncTCP using the latest dev version from @me-no-dev
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>	//https://github.com/me-no-dev/ESPAsyncWebServer using the latest dev version from @me-no-dev
#include <esp_wifi.h>			//Used for mpdu_rx_disable android workaround
#include <Ccaptive_portal.h>


Portal::Portal()
    : ssid("ESP32 Captive Portal"),
      password(""),
      wifiSSID(""),
      wifiPassword("")
{
}
int i = 0;
int n = 0;
const IPAddress localIP(4, 3, 2, 1);		   // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(4, 3, 2, 1);		   // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0);  // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://4.3.2.1";	 // a string version of the local IP with http, used for redirecting clients to your webpage

String generateHTML() {
    String html = R"=====(
<!DOCTYPE html> <html>
    <head>
      <title>ESP32 Captive Portal</title>
      <style>
        body {background-color: #06cc13;}
        h1, h2 {color: white;}
        form {text-align: center; margin-top: 20px;}
        label {display: block; margin-bottom: 10px;}
        input {width: 200px; padding: 5px;}
        button {padding: 10px; background-color: #4CAF50; color: white; border: none;}
		.custom-select 
{
    display: inline-block;
    position: relative;
    top: 50%;
    -webkit-transform: translateY(-50%);
    -ms-transform: translateY(-50%);
    transform: translateY(-50%);
    border-radius: 5px;
    box-shadow: 0 0 1em rgba(white,0.2), inset 0 0 1px rgba(white,0.8);
    /* Styling the select background */
    background-color: lighten(#1FBDED,20%);

    select 
    {
        width: auto;
        margin: 0;
        padding: .75em 1.5em;
        outline: none;
        cursor: pointer;
        border: none;
        border-radius: 0;
        background-color: transparent;
        /* Styling the select text color */
        color: darken(#1FBDED,35%);

        /* removes the fucking native down arrow */
        -webkit-appearance: none;
        -moz-appearance: none;
        text-indent: 0.01px;
        text-overflow: '';
      
        &::-ms-expand 
        {
            display: none;
        }
      
    }

    &:before,
    &:after
    {
        position: absolute;
        top: 50%;
        right: 1em;
        width: 0;
        height: 0;
        padding: 0;
        content: '';
        pointer-events: none;
    }

    &:before 
    {
        transform: translateY(-125%);
        /* Styling the down arrow */
        border-left: .25em solid transparent;
        border-right: .25em solid transparent;
        border-bottom: .375em solid darken(#1FBDED,25%);
    }

    &:after 
    {
        /* Styling the down arrow */
        border-left: .25em solid transparent;
        border-right: .25em solid transparent;
        border-top: .375em solid darken(#1FBDED,25%);
    }
}
      </style>
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
    </head>
    <body>
      <h1>Hello World!</h1>
      <h2>This is a captive portal example. All requests will be redirected here </h2>
       <form action="/save" method="post">
          <label class ="custom-select" for="ssid">SSID:</label>
            <select name="ssid" id="ssid">
    )=====";

		for (int i = 0; i < n; ++i) {
				html += "<option value=\"" + WiFi.SSID(i)+ "\">" + WiFi.SSID(i) + "</option>";
		}

		html += R"=====(
						</select>
						<br><br>
					<label for="password">Password:</label>
					<input type="password" id="password" name="password" required><br>
					<button type="submit">Save</button>
				</form>
		</body>
	</html>
)=====";
    return html;
}

DNSServer dnsServer;
AsyncWebServer server(80);

void setUpDNSServer(DNSServer &dnsServer, const IPAddress &localIP) {
// Define the DNS interval in milliseconds between processing DNS requests
#define DNS_INTERVAL 30

	// Set the TTL for DNS response and start the DNS server
	dnsServer.setTTL(3600);
	dnsServer.start(53, "*", localIP);
}

void startSoftAccessPoint(const char *ssid, const char *password, const IPAddress &localIP, const IPAddress &gatewayIP) {
// Define the maximum number of clients that can connect to the server
#define MAX_CLIENTS 4
// Define the WiFi channel to be used (channel 6 in this case)
#define WIFI_CHANNEL 6

	// Set the WiFi mode to access point and station
	WiFi.mode(WIFI_MODE_AP);

	// Define the subnet mask for the WiFi network
	const IPAddress subnetMask(255, 255, 255, 0);

	// Configure the soft access point with a specific IP and subnet mask
	WiFi.softAPConfig(localIP, gatewayIP, subnetMask);

	// Start the soft access point with the given ssid, password, channel, max number of clients
	WiFi.softAP(ssid, password, WIFI_CHANNEL, 0, MAX_CLIENTS);

	// Disable AMPDU RX on the ESP32 WiFi to fix a bug on Android
	esp_wifi_stop();
	esp_wifi_deinit();
	wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
	my_config.ampdu_rx_enable = false;
	esp_wifi_init(&my_config);
	esp_wifi_start();
	vTaskDelay(100 / portTICK_PERIOD_MS);  // Add a small delay
}
int ScanNetworks() {
	// Scan for WiFi networks and return the number of networks found
	WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
	Serial.println("Scan start");
    // WiFi.scanNetworks will return the number of networks found.
    n = WiFi.scanNetworks();
    Serial.println("Scan done");
	if (n == 0) {
		Serial.println("No networks found");
	} else {
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; ++i) {
			// Print SSID and RSSI for each network found
			Serial.print(i + 1);
			Serial.print(": ");
			Serial.print(WiFi.SSID(i));
			Serial.print(" (");
			Serial.print(WiFi.RSSI(i));
			Serial.print(")");
			Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
			delay(10);
		}
	}
	Serial.println("");
	return n;
}
String Portal::initPortal() {
	ScanNetworks();
    startSoftAccessPoint(ssid, password, localIP, gatewayIP);
    setUpDNSServer(dnsServer, localIP);


	// Required
	server.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
	server.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	// Background responses: Probably not all are Required, but some are. Others might speed things up?
	// A Tier (commonly used by modern systems)
	server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
	server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
	server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
	server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
	server.on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
	server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home
	server.on("/save", HTTP_POST,[&](AsyncWebServerRequest *request) { // Capture wifiSSID and wifiPassword by reference
		// Read the form parameters
		wifiSSID = request->arg("ssid");
		wifiPassword = request->arg("password");
		request->redirect(localIPURL);
	});
	// return 404 to webpage icon
	server.on("/favicon.ico", [](AsyncWebServerRequest *request)
				{ request->send(404); }); // webpage icon
  // Serve Basic HTML Page
	server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", generateHTML());
		response->addHeader("Cache-Control", "public,max-age=31536000");  // save this file to cache for 1 year (unless you refresh)
		request->send(response);
		Serial.println("Served Basic HTML Page");
	});

	// the catch all
	server.onNotFound([](AsyncWebServerRequest *request) {
		request->redirect(localIPURL);
		Serial.print("onnotfound ");
		Serial.print(request->host());	// This gives some insight into whatever was being requested on the serial monitor
		Serial.print(" ");
		Serial.print(request->url());
		Serial.print(" sent redirect to " + localIPURL + "\n");
	});
    server.begin();

    while (wifiSSID.isEmpty() || wifiPassword.isEmpty()) {
        dnsServer.processNextRequest();
        delay(DNS_INTERVAL);
    }

    // Retourner une chaîne unique contenant les deux valeurs
    return wifiSSID + "|" + wifiPassword;
}
