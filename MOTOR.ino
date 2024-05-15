#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char * WIFI_SSID = "Galaxy A34 5G 9EFF";
const char * WIFI_PASS = "10000001";
const char * MQTT_BROKER_HOST = "a14d7apumdcnw6-ats.iot.us-east-1.amazonaws.com";
const int MQTT_BROKER_PORT = 8883;

const char * MQTT_CLIENT_ID = "ESP-32";                                               // Unique CLIENT_ID

const char * UPDATE_TOPIC = "$aws/things/thing/shadow/update";              // publish
const char * UPDATE_DELTA_TOPIC = "$aws/things/thing/shadow/update/delta";  // subscribe

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAKQDUch2iAmRb648KLxV9AZqZoNiMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDA0MjkyMDI1
MTRaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDOPkk0IdFOBJ3uii3z
6FZmJcprRVpyH8RF9vvK2m0+oTx4dAuo53hSvSVRxPtghM1OmyUtaZC9GzWx67Zh
cdpmVj6ZdahTtUK1AmIaX9IXidGlPHp9cuhjSx4Y1zkXUGSE/1wA1DeRjPLR+jUf
ETs6d7dnQvsBzOKt29+npmxftpiScoOpPdBYmkR1Rb+8xY1HHs7Cx+4acnZ/y7O2
Pcw0dVhLzUV2FfeyHnnkm012GlpjcY/eRaoUZoWjCaz3yud2YvkRES1YnBAu6kAw
20b7dQvp29UOFCZb4kZkULl0piXiMj14qHEqvveAcdXKi3FacI3K4WXDjskD3ges
YoqBAgMBAAGjYDBeMB8GA1UdIwQYMBaAFLj6PsLhzp7H210pM/BTuVXvAwhvMB0G
A1UdDgQWBBSyfVSZaZccsRlK755FcIXpQiewkTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAG95sVtYkcFIp5QjsRT4VHsGb
VnnGT3CG6gB0frbBu1PhUvGu1GVawh8rXpMIs13+wJuis+lxKx5VIY5zwnH3gZDb
L4sXf4jLVJFY6Wu6UrAhbTCu1cO5EdyZoA7AVVSBnS/3f2kVtGFdTn8FMNPCVcLv
kXcmDtuw66QP9OINpJMx6MaWvmJphmSeUyl1hR+a11WruKpKmCP19Fv0WPSV3MSF
9L8qLH6m7e01WjfQRmI6Lpy7X8Ij1nNoNnRSBDEOd5IqKTqu4aqdPZfvSmhMpq/W
f4g4qmIo/MPSQ6w+6bxAph5nJqW49QbQm8zMFB7NHXcjN9XVSjZ72DtPjHffWg==
-----END CERTIFICATE-----

)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAzj5JNCHRTgSd7oot8+hWZiXKa0Vach/ERfb7ytptPqE8eHQL
qOd4Ur0lUcT7YITNTpslLWmQvRs1seu2YXHaZlY+mXWoU7VCtQJiGl/SF4nRpTx6
fXLoY0seGNc5F1BkhP9cANQ3kYzy0fo1HxE7One3Z0L7Aczirdvfp6ZsX7aYknKD
qT3QWJpEdUW/vMWNRx7OwsfuGnJ2f8uztj3MNHVYS81FdhX3sh555JtNdhpaY3GP
3kWqFGaFowms98rndmL5EREtWJwQLupAMNtG+3UL6dvVDhQmW+JGZFC5dKYl4jI9
eKhxKr73gHHVyotxWnCNyuFlw47JA94HrGKKgQIDAQABAoIBAB1Kw2LgGrYV+chT
X8twiDTh0SoDI6Fp2ZqGRaS9+R4YEFqbQLywg2pc6tv+OiWvNumfJ1yXzbLOFoY6
kCCu5vH59dId39ZjvCGxQPW1/kTRkjEBsfP3SgIwPQoSJ02kYWJyUkLb4TiO6D3i
Z4EOFXdsYWQNI1FSJC/FNrK89E9zgin/Vg81UyhSaLZPTmGdvhdG0IPxhXuPPJyQ
4IKSoxc8Q7CkXiMFVggTAzHjOKU91ngJUY8qdYxX2chWbrJTl0RMBKgJCqjc4myE
/xAUCV9hF5ro4hOPg+FKglQV6z4ucc6GDNCLLMb/0G+fPT7zy9u0xynFVBARrjox
ZAZeM0ECgYEA+ADX0ghtsE7L/M+7epKH3fge1SqsH0E7JPwwTj7RnQoQoM47sfSk
aWXiBCMQMN+DbJUts7SfPDQyCYoiZ4g8gowjoQCF3qjWAPkbO557HpR8Qty+i2MG
1cSnGh8e/qjQ1GfkfFHk1xjocvlGaobwGr4MsEzNNTI5E9I0phNQJB0CgYEA1OS7
lg4Sni2EUqDPHoZtqJHc4vc2uNDyAW4dEmwBNLmJtezmbgIQKl4HkFq5k9RZODuf
rUisWaqXG5xc5s3GPKdE7a5Bhkz+oCsnQnq/Y5rwrB4nKWwUs3hcoJeydpXha4Yw
ZjWduJftanhv0rnXjE+VZvBCPBSkafLa03fJarUCgYA2M2D3mzX1yVLVnsTXhfzB
qz950QirEAOQErmv8BS5oIB/1HLBcrkX+9dIli0ssTy012mUk84VR0cERLc+L00i
LuR+hHG9duLGdarY/crkpQj/R+Uw62SMBZ2LnO+WMmjpcwdBkFhWFjB/KDGWiIUj
IxP819wlWyOUTGWLQj9KlQKBgH0XWVG2OyJORG34pzqcalqeOlfyQmkd/NTySorE
g4DjdBM9e7gv5ie+0Fr1tA00vKfaANzk0sBtCBTlL4E+b7i74Dh4J3AuVX7vXEYn
scVpCPlZTlF8fD1n/6Us/WPIdYekdVPZNS99tbbVxX7gS9LaAENX8NM0ndbTJ9kR
jXvxAoGBAOMU6TX9nKxBeKbts2ldqNuYNWet/Qy6veqCJ8Y53JjLfU1vSMSU0rLm
acVVBiL9fKHNt7RygjtlqHshNeTj/l5iE4IVSB0QXWEeoSDtRRTYmVJZjHRa60vi
FHtaczAdV49Qux57RHKwP7eL/gvA4gOomhvK/2KwZkpWFjFubneM
-----END RSA PRIVATE KEY-----
)KEY";

WiFiClientSecure wiFiClient;
PubSubClient mqttClient(wiFiClient);

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;

StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
char outputBuffer[128];

unsigned char builtInLed = 0;

void reportBuiltInLed() {
  outputDoc["state"]["reported"]["BuiltInLed"] = builtInLed;
  serializeJson(outputDoc, outputBuffer);
  mqttClient.publish(UPDATE_TOPIC, outputBuffer);
}

void setBuiltInLed() {
  if (builtInLed) digitalWrite(0, HIGH);
  else digitalWrite(0, LOW);
  reportBuiltInLed();
}

void callback(const char * topic, byte * payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += String((char) payload[i]);
  Serial.println("Message from topic " + String(topic) + ":" + message);
  DeserializationError err = deserializeJson(inputDoc, payload);
  if (!err) {
    if (String(topic) == UPDATE_DELTA_TOPIC) {
      builtInLed = inputDoc["state"]["BuiltInLed"].as<int8_t>();
      setBuiltInLed();
    }
  }
}

void setup() {
  pinMode(0, OUTPUT);
  Serial.begin(115200);

  Serial.print("Connecting to " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  wiFiClient.setCACert(AMAZON_ROOT_CA1);
  wiFiClient.setCertificate(CERTIFICATE);
  wiFiClient.setPrivateKey(PRIVATE_KEY);

  mqttClient.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
  mqttClient.setCallback(callback);

  Serial.print("Connecting to " + String(MQTT_BROKER_HOST));
  if (mqttClient.connect(MQTT_CLIENT_ID)) {
    Serial.println(" Connected!");

    delay(100);
    mqttClient.subscribe(UPDATE_DELTA_TOPIC);
    Serial.println("Subscribed to " + String(UPDATE_DELTA_TOPIC));

    delay(100);
    reportBuiltInLed();
  }
}

void loop() {
  if (mqttClient.connected()) {
    mqttClient.loop();
  }
  else {
    Serial.println("MQTT broker not connected!");
    delay(2000);
  }
}
