#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>

// Define the Arduino pin connected to the ultrasonic sensor
const int ultrasonicPin = 2;

// Define the web server details
const char* server = "example.com";
const int port = 80;
const char* endpoint = "/parking/update.php"; // Change this to your endpoint URL

EthernetClient client;
HttpClient httpClient(client, server, port);

void setup() {
  // Initialize the Ethernet shield
  byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Change this to your Ethernet shield MAC address
  Ethernet.begin(mac);
  delay(1000);

  // Initialize the serial communication
  Serial.begin(9600);

  // Set ultrasonic sensor pin as input
  pinMode(ultrasonicPin, INPUT);
}

void loop() {
  // Read the distance from the ultrasonic sensor
  int distance = getDistance();

  // Check if a car is present in the parking space
  bool isCarPresent = (distance < 50); // Adjust the threshold as per your setup

  // Update the parking space status on the web server
  updateParkingSpaceStatus(isCarPresent);

  // Wait for some time before repeating the process
  delay(5000);
}

int getDistance() {
  // Measure the distance using the ultrasonic sensor
  digitalWrite(ultrasonicPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicPin, LOW);
  int duration = pulseIn(ultrasonicPin, HIGH);
  int distance = duration * 0.034 / 2;

  // Print the measured distance to the serial monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  return distance;
}

void updateParkingSpaceStatus(bool isCarPresent) {
  // Prepare the data to be sent to the server
  String data = "space1=" + String(isCarPresent ? 0 : 1); // Change 'space1' to your parking space identifier

  // Send the HTTP POST request to update the parking space status
  httpClient.post(endpoint, "application/x-www-form-urlencoded", data);

  // Print the response received from the server
  Serial.print("Server Response: ");
  Serial.println(httpClient.responseStatusCode());
  Serial.println(httpClient.responseBody());

  // Close the connection
  httpClient.stop();
}