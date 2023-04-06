#include "WiFi.h"
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "BT20ECE031_Sahil";
const char* password = "Ash_Sahil";
WiFiServer server(80);

const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s);
const long timeoutTime = 1000;

LiquidCrystal_I2C lcd (0x27, 16,2); 

// String html ="<!DOCTYPE html>\
// <html>\
// <body>\
// <p><h1> Distance in cm = " + distanceCm + "</h1></p>" + "<p><h1> Distance in Inch = " + distanceInch + "</h1></p>" +"</body></html>";


void setup() {

  lcd.backlight ();
  lcd.setCursor (0, 0);
  delay(1000);
  pinMode(2, OUTPUT);
  pinMode(19, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(19, LOW);
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);


  lcd.clear();
  delay(100);
  lcd.setCursor(0,0);
  delay(100);
  lcd.print("Distance : ");
  lcd.setCursor(0,1);
  String dis = String(distanceCm) + " cm";
  lcd.print(dis);
  delay(100);  
    
  if(distanceCm <= 10)
  {  
    digitalWrite(2, HIGH);
    digitalWrite(19, HIGH);
    //Serial.println("Active");
  }
  else
  {  
    digitalWrite(2, LOW);
    digitalWrite(19, LOW);
    //Serial.println("Not active");    
  }


  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the table
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:black; font-weight: bold; padding: 1px; }");
            client.println(".alert { color:red}");
            client.println(".ok { color:green}");

            // Web Page Heading

             client.println("</style></head><body><h2>RADAR</h2>");
            if (distanceCm > 10)
            {
              client.println("</style></head><body><p><h2>Monitoring the closest object near RADAR</h2></p><p><h3 class=\"ok\">All Ok</h3></p>");
            }
            else 
            {
              client.println("</style></head><body><h2 class =\"alert\">Enemys Invaided</h2>");
            }
      
            client.println("<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");
            client.println("<tr><td>Cm , Inch</td><td><span class=\"sensor\">");
            String s = String(distanceCm) + " , " + String(distanceInch);
            client.println(s);
            client.println("</span></td></tr></table>");
            

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  delay(1000);

}