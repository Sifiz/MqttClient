#include <Arduino.h>  //not needed in the arduino ide

// Captive Portal
#include <AsyncTCP.h>  //https://github.com/me-no-dev/AsyncTCP using the latest dev version from @me-no-dev
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>	//https://github.com/me-no-dev/ESPAsyncWebServer using the latest dev version from @me-no-dev
#include <esp_wifi.h>			//Used for mpdu_rx_disable android workaround
#include <Ccaptive_portal.h>


Portal::Portal()
    : ssid("Votre SPA"),
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
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Network Selection</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f4;
            margin: 0;
            padding: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }

        .container {
            display: flex;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        .text-container {
            flex: 1;
            padding: 20px;
            text-align: center;
        }

        .form-container {
            flex: 1;
            padding: 20px;
        }

        label {
            display: block;
            margin-bottom: 10px;
            font-weight: bold;
        }

        select {
            width: 100%;
            padding: 10px;
            margin-bottom: 20px;
            box-sizing: border-box;
            border: 1px solid #ccc;
            border-radius: 4px;
            appearance: none; /* Remove default arrow */
            background-image: url('data:image/svg+xml;utf8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="%23444444" width="18px" height="18px"><path d="M7 10l5 5 5-5z" /></svg>'); /* Add custom arrow */
            background-repeat: no-repeat;
            background-position: right 10px top 50%;
        }

        input {
            width: 100%;
            padding: 10px;
            margin-bottom: 20px;
            box-sizing: border-box;
            border: 1px solid #ccc;
            border-radius: 4px;
        }

        button {
            background-color: #2358C2;
            color: #fff;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }

        button:hover {
            background-color: #1C4599;
        }
    </style>
</head>

<body>
    <div class="container">
        <div class="text-container">
            <h3>Bienvenue dans votre SPA !</h3>
            <p>Pour commencer à l'utiliser, connectez votre réseau ici.</p>
            <?xml version="1.0" encoding="utf-8"?><!-- Uploaded to: SVG Repo, www.svgrepo.com, Generator: SVG Repo Mixer Tools -->
            <svg fill="#2358C2" width="80px" height="80px" viewBox="0 0 50 50" version="1.2" baseProfile="tiny" xmlns="http://www.w3.org/2000/svg" overflow="inherit"><path d="M47.231 41h-16.231v4h16.397c1.348 0 1.603-1.175 1.603-1.175v-.97s-.012-1.855-1.769-1.855zm-2.245-.79c2.214 0 4.014-1.802 4.014-4.022 0-2.227-1.8-4.023-4.014-4.023-2.211 0-4.006 1.797-4.006 4.023 0 2.219 1.795 4.022 4.006 4.022zm-8.454-.21c2.106 0 3.8-1.484 3.8-3.587-.001-2.099-1.701-3.2-3.791-3.216l-5.541.702v6.101h5.532zm-18.009-26.067c2.459 0 4.448-2 4.448-4.467 0-2.467-1.989-4.466-4.448-4.466-2.457 0-4.449 1.999-4.449 4.466s1.992 4.467 4.449 4.467zm-4.542 14.456l-.04-.058-2.016-4.694-.003 9.363h6.025l1.316-1.4-3.569-1.374c-.846-.318-1.447-1.038-1.713-1.837zm1.277-.421c.195.438.556.793 1.01.956l6.877 2.549c.88.389 1.908-.015 2.294-.898.386-.889-.015-1.924-.896-2.311l-6.345-2.396-1.96-4.701 1.278-.476 1.661 4.028 4.823 1.788v-4.922l2.56 1.523 1.276 6.428c.313.918 1.304 1.401 2.218 1.084.905-.314 1.386-1.315 1.075-2.229l-1.43-6.191c-.094-.347-.293-.65-.562-.877-.733-.812-4.44-4.957-4.704-5.234-.387-.388-1.272-1.089-2.944-1.089h-6c-3.41 0-4.066 3.48-3.369 5.25l3.138 7.718zm14.742 6.032h-27.033c-.984 0-1.967.555-1.967 1.776v9.224h29v-11z"/></svg>
        </div>

        <div class="form-container">
            <h2>Network Selection</h2>
            <form action="/connect" method="post">
                <label for="network">Select Network:</label>
                <select id="network" name="network">
    )=====";
                    for (int i = 0; i < n; ++i) {
				html += "<option value=\"" + WiFi.SSID(i)+ "\">" + WiFi.SSID(i) + "</option>";
		}
        html += R"=====(
                </select>

                <label for="password">Enter Password:</label>
                <input type="password" id="password" name="password" placeholder="Enter your password">

                <button type="submit">Connect</button>
            </form>
        </div>
    </div>
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
	server.on("/connect", HTTP_POST,[&](AsyncWebServerRequest *request) { // Capture wifiSSID and wifiPassword by reference
		// Read the form parameters
		wifiSSID = request->arg("network");
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
