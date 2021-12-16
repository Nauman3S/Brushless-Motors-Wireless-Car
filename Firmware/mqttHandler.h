
// Select your modem:
//#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM868
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_SIM7000
// #define TINY_GSM_MODEM_SIM7000SSL
// #define TINY_GSM_MODEM_SIM7080
// #define TINY_GSM_MODEM_SIM5360
// #define TINY_GSM_MODEM_SIM7600
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_SARAR4
// #define TINY_GSM_MODEM_M95
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_MC60
// #define TINY_GSM_MODEM_MC60E
// #define TINY_GSM_MODEM_ESP8266
// #define TINY_GSM_MODEM_XBEE
// #define TINY_GSM_MODEM_SEQUANS_MONARCH

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#ifndef __AVR_ATmega328P__
#define SerialAT Serial2

#endif

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
// NOTE:  DO NOT AUTOBAUD in production code.  Once you've established
// communication, set a fixed baud rate using modem.setBaud(#).
#define GSM_AUTOBAUD_MIN 9600
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay, if needed.
// This may be needed for a fast processor at a slow baud rate.
// #define TINY_GSM_YIELD() { delay(2); }

// Define how you're planning to connect to the internet.
// This is only needed for this example, not in other code.
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false

// set GSM PIN, if any
#define GSM_PIN ""

// Your GPRS credentials, if any
#include "networkAPN.h"

// Your WiFi connection credentials, if applicable
const char wifiSSID[] = "YourSSID";
const char wifiPass[] = "YourWiFiPass";

// MQTT details
const char *broker = "broker.hivemq.com";

const char *topicPosition = "brushlessCar/position";
const char *topicSpeed = "brushlessCar/speed";
const char *topicLedStatus = "GsmClientTest/ledStatus";
const char *topicInit = "GsmClientTest/ledStatus";

#include <TinyGsmClient.h>
#include <PubSubClient.h>

// Just in case someone defined the wrong thing..
#if TINY_GSM_USE_GPRS && not defined TINY_GSM_MODEM_HAS_GPRS
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS false
#define TINY_GSM_USE_WIFI true
#endif
#if TINY_GSM_USE_WIFI && not defined TINY_GSM_MODEM_HAS_WIFI
#undef TINY_GSM_USE_GPRS
#undef TINY_GSM_USE_WIFI
#define TINY_GSM_USE_GPRS true
#define TINY_GSM_USE_WIFI false
#endif

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif
TinyGsmClient client(modem);
PubSubClient mqtt(client);

#define LED_PIN 13
int ledStatus = LOW;

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{
    SerialMon.print("Message arrived [");
    SerialMon.print(topic);
    SerialMon.print("]: ");
    SerialMon.write(payload, len);
    SerialMon.println();
    String pLoad = "";
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
        pLoad = pLoad + String((char)payload[i]);
    }
    Serial.println();

    // Only proceed if incoming message's topic matches
    if (String(topic) == topicPosition)
    {
        if (pLoad == String("up"))
        {
            forward();
        }
        else if (pLoad == String("left"))
        {
            left();
        }
        else if (pLoad == String("right"))
        {
            right();
        }
        else if (pLoad == String("down"))
        {
            stop();
        }
    }
    else if (String(topic) == topicSpeed)
    {
        speed = pLoad.toInt();
    }
    else if (string(topic) == topicLedStatus)
    {
        if (pLoad == String("on"))
        {
            digitalWrite(LED_PIN, HIGH);
        }
        else
        {

            digitalWrite(LED_PIN, LOW);
        }
    }
}

boolean mqttConnect()
{
    SerialMon.print("Connecting to ");
    SerialMon.print(broker);

    // Connect to MQTT Broker
    String clientId = "Brushless-";
    clientId += String(random(0xffff), HEX);
    boolean status = mqtt.connect(clientId.c_str());

    // Or, if you want to authenticate MQTT:
    // boolean status = mqtt.connect("GsmClientName", "mqtt_user", "mqtt_pass");

    if (status == false)
    {
        SerialMon.println(" fail");
        return false;
    }
    SerialMon.println(" success");
    mqtt.publish(topicInit, "GsmClientTest started");
    mqtt.subscribe(topicPosition);
    mqtt.subscribe(topicSpeed);
    return mqtt.connected();
}

void setupGPRS()
{
    // Set console baud rate
    //   SerialMon.begin(115200);
    delay(10);

    pinMode(LED_PIN, OUTPUT);

    // !!!!!!!!!!!
    // Set your reset, enable, power pins here
    // !!!!!!!!!!!

    SerialMon.println("Wait...");

    // Set GSM module baud rate
    TinyGsmAutoBaud(SerialAT, GSM_AUTOBAUD_MIN, GSM_AUTOBAUD_MAX);
    // SerialAT.begin(9600);
    delay(6000);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    SerialMon.println("Initializing modem...");
    modem.restart();
    // modem.init();

    String modemInfo = modem.getModemInfo();
    SerialMon.print("Modem Info: ");
    SerialMon.println(modemInfo);

#if TINY_GSM_USE_GPRS
    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3)
    {
        modem.simUnlock(GSM_PIN);
    }
#endif

    SerialMon.print("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isNetworkConnected())
    {
        SerialMon.println("Network connected");
    }

#if TINY_GSM_USE_GPRS
    // GPRS connection parameters are usually set after network registration
    SerialMon.print(F("Connecting to "));
    SerialMon.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass))
    {
        SerialMon.println(" fail");
        delay(10000);
        return;
    }
    SerialMon.println(" success");

    if (modem.isGprsConnected())
    {
        SerialMon.println("GPRS connected");
    }
#endif

    // MQTT Broker setup
    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);
}

void loopGPRS()
{
    // Make sure we're still registered on the network
    if (!modem.isNetworkConnected())
    {
        SerialMon.println("Network disconnected");
        if (!modem.waitForNetwork(180000L, true))
        {
            SerialMon.println(" fail");
            delay(10000);
            return;
        }
        if (modem.isNetworkConnected())
        {
            SerialMon.println("Network re-connected");
        }

#if TINY_GSM_USE_GPRS
        // and make sure GPRS/EPS is still connected
        if (!modem.isGprsConnected())
        {
            SerialMon.println("GPRS disconnected!");
            SerialMon.print(F("Connecting to "));
            SerialMon.print(apn);
            if (!modem.gprsConnect(apn, gprsUser, gprsPass))
            {
                SerialMon.println(" fail");
                delay(10000);
                return;
            }
            if (modem.isGprsConnected())
            {
                SerialMon.println("GPRS reconnected");
            }
        }
#endif
    }

    if (!mqtt.connected())
    {
        SerialMon.println("=== MQTT NOT CONNECTED ===");
        // Reconnect every 10 seconds
        uint32_t t = millis();
        if (t - lastReconnectAttempt > 10000L)
        {
            lastReconnectAttempt = t;
            if (mqttConnect())
            {
                lastReconnectAttempt = 0;
            }
        }
        delay(100);
        return;
    }

    mqtt.loop();
}