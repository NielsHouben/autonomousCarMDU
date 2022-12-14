#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define LEFT 5  // D1
#define RIGHT 4 // D2
#define LEFT_DIRECTION 0

const char *ssid = "ABBgym_2.4";
const char *password = "mittwifiarsabra";
const char *mqtt_server = "0d64678487414d04aa9598dfec7afddd.s2.eu.hivemq.cloud";

static const char *fingerprint PROGMEM = "F2 F7 5C 9F 1F 89 48 59 CC B3 D2 2A 5F 4A 6A 6D B3 6B 1F B1";

// WiFiClient espClient;
WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

int power = 400;
int leftPower = 0;
int rightPower = 0;

//==========================================
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//=======================================
void callback(char *topic, byte *payload, unsigned int length)
{
  String topicString((char *)topic);
  String message = "";
  Serial.println(topicString);
  for (unsigned int i = 0; i < length; i++)
  {
    // Serial.print((char)payload[i]);
    message += (char)payload[i];
  }

  if (topicString == "/pos")
  {
    Serial.println("pos");
    Serial.println(message);
    // int leftPower = 100 * message.toFloat();
    // Serial.println(leftPower);

    // analogWrite(LEFT, leftPower);
    // snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", payload);
  }
  else if (topicString == "/NHSI/leftPower")
  {
    Serial.print("left: ");
    Serial.println(message);

    float factor = message.toFloat();
    leftPower = power * factor;
    Serial.println(leftPower);
    analogWrite(LEFT, leftPower);
  }
  else if (topicString == "/NHSI/rightPower")
  {
    Serial.print("right: ");
    Serial.println(message);

    float factor = message.toFloat();
    rightPower = power * factor;
    Serial.println(leftPower);
    analogWrite(RIGHT, rightPower);
  }
  else if (topicString == "/NHSI/speed")
  {
    Serial.print("speed: ");
    Serial.println(message);

    // float factor = message.toFloat();
    // rightPower = power * factor;
    power = message.toInt();
    Serial.println(power);
    // analogWrite(RIGHT, rightPower);
  }
  else
  {
    Serial.print("topic: ");
    Serial.print(topicString);
    Serial.print(", message: ");
    Serial.println(message);
  }

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
  {
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  }
}
//=====================================
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection…");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), "nisse", "ostKaka123"))
    {
      // if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement…
      // client.publish("outTopic", "hello world");
      // … and resubscribe
      client.subscribe("/pos");
      client.subscribe("/NHSI/leftPower");
      client.subscribe("/NHSI/rightPower");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//============================================/right====
void setup()
{
  delay(500);
  Serial.begin(9600);
  delay(500);
  setup_wifi();

  pinMode(RIGHT, OUTPUT);
  pinMode(LEFT, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
  espClient.setFingerprint(fingerprint);
  // Setting insecure disables the fingerprint verification.
  // espClient.setInsecure 21();

  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000)
  {
    lastMsg = now;
    ++value;
    // snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    // Serial.print("Publish message: ");
    // Serial.println(msg);
    // client.publish("outTopic", msg);
    // client.publish("/pos", msg);
  }
}