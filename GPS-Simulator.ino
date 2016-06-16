#include <Time.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <math.h>

SoftwareSerial mySerial(10, 11); // RX, TX
bool answer; //check if user inputs are complete
int choice; //path choice
double latStart;
double latEnd;
double lonStart;
double lonEnd;
double speed;

void setup() {
  //Open serial communications and wait for port to open:
  Serial.begin(4800);
  mySerial.begin(4800);
  answer = false;
  String input = "";
  choice = 1; //default path if time expires
  int format = 13; //date format
  int hh = 0; //default date if time expires
  int mm = 0;
  int ss = 0;
  int dd = 7;
  int MM = 3;
  int yy = 16;
  mySerial.println("Enter the current GMT time and date in this format:\n\rhhmmssddMMyy");
  long t = 60000; //time limit
  while (millis() <= t) {
    while(mySerial.available()) {
      char incoming = mySerial.read();
      mySerial.print(incoming); //view typing
      input += incoming;
      if (incoming == 127) { //adjust for backspaces in user input
        format += 2;
      }//end if
      else if (input.substring(0,3)== "egm" and input.substring(3).toInt() >= 1 and 
          input.substring(3).toInt() <= 14 and incoming == '\r') {
          //set input as the path choice
          choice = input.substring(3).toInt();
          t=0;
      } //end else if
      else if (input.substring(0,3) == "egm" and incoming == '\r' and input.length() == 4) {
        //default path choice if no input
        t=0;
      } //end else if
      else if (input.substring(0,3)== "egm" and incoming == '\r' and 
          (input.substring(3).toInt() < 1 or input.substring(3).toInt() > 14)) {
          //invalid path choice
          input = "egm"; //reset path choice input
          mySerial.println("That is not a Path option. Must be 1-14");
          t+= millis(); //add more time
          continue;
      } //end else if
      else if (incoming == '\r') { //enter button is pressed for the date input
        if (input.length() == format) { //12 character date
          hh = input.substring(0,2).toInt();
          mm = input.substring(2,4).toInt();
          ss = input.substring(4,6).toInt();
          dd = input.substring(6,8).toInt();
          MM = input.substring(8,10).toInt();
          yy = input.substring(10).toInt();
          t += millis(); //add more time
          mySerial.println("");
          mySerial.println("Enter the Path Number of your choice:\n\r(1)AX_1\n\r(2)AX_2");
          mySerial.println("(3)AX_7\n\r(4)AX_8\n\r(5)AX_10\n\r(6)AX_18\n\r(7)AX_20");
          mySerial.println("(8)AX_25\n\r(9)AX_32\n\r(10)AX_90\n\r(11)AX_97\n\r(12)MX_1");
          mySerial.println("(13)MX_2\n\r(14)MX_4"); //list of path options
          input = "egm"; //differentiate path input from date input
        } //end if
        else if (input.length() == 1){ //default date if no input
          int hh = 0;
          int mm = 0;
          int ss = 0;
          int dd = 7;
          int MM = 3;
          int yy = 16;
          input = "egm"; //differentiate path input from date input
          t += millis(); //add more time
          mySerial.println("Enter the Path Number of your choice:\n\r(1)AX_1\n\r(2)AX_2");
          mySerial.println("(3)AX_7\n\r(4)AX_8\n\r(5)AX_10\n\r(6)AX_18\n\r(7)AX_20");
          mySerial.println("(8)AX_25\n\r(9)AX_32\n\r(10)AX_90\n\r(11)AX_97\n\r(12)MX_1");
          mySerial.println("(13)MX_2\n\r(14)MX_4"); //list of path options
        } //end else if
        else { //date input is improper format
          input = "";
          mySerial.println("");
          mySerial.println("That is not the proper format, please try again");
          mySerial.println("hhmmssddMMyy");
          t+= millis(); //add more time
          continue;
        } //end else
      } //end else if
    } //end while
  } //end while
  setTime(hh,mm,ss,dd,MM,yy);
  answer = true;
} //end setup

/*{latstart,latend,lonstart,lonend}
AX_1 {59.786,58.636,-43.786,-5.002};
AX_2 {47.561,64.127, -52.713, -21.817};
AX_7 {25.77,36.141, -80.19,-5.354};
AX_8 {-33.925,40.736,18.424,-74.172};
AX_10 {40.713,18.466,-74.006,-66.106};
AX_18 {-34.604,-33.925,-58.382,18.424};
AX_20 {4.922,48.349,-52.313,-4.406};
AX_25 {-33.925,-71.314,18.424,-1.132};
AX_32 {40.736,32.310,-74.172,-64.750};
AX_90 {64.127,58.636,-21.817,-5.002};
AX_97 {-22.907,-20.508,-43.173,-29.331};
MX_1 {37.923,36.467,16.025,22.451};
MX_2 {36.141,44.399,-5.354,8.962};
MX_4 {44.399,38.116,8.962,13.361};*/
double path[][4] = {{59.786,58.636,-43.786,-5.002},{47.561,64.127, -52.713, -21.817},
{25.77,36.141, -80.19,-5.354},{-33.925,40.736,18.424,-74.172},
{40.713,18.466,-74.006,-66.106},{-34.604,-33.925,-58.382,18.424},
{4.922,48.349,-52.313,-4.406},{-33.925,-71.314,18.424,-1.132},
{40.736,32.310,-74.172,-64.750},{64.127,58.636,-21.817,-5.002},
{-22.907,-20.508,-43.173,-29.331},{37.923,36.467,16.025,22.451},
{36.141,44.399,-5.354,8.962},{44.399,38.116,8.962,13.361}};

double knots = 20.0;
int voyages = 10;
int trips = 0;
int period = 1000;
double totalDistance = 0;
double bearing;
double currentBearing = 0;
long currentTime;
long previousTime;
String wantedPort = "/dev/ttyUSB0";

class Position {
  double latitude;
  double longitude;
  double latMin; 
  double lonMin;
  double lonPos;
  public:
    Position(double lat, double lon) {
      latitude = lat;
      longitude = lon;
    }
    double getLatitude() {
      return latitude;
    } //end getLatitude
    double getLongitude() {
      return longitude;
    } //end getLongitude   
    String getLatitudeDegrees() { //formatting Latitude Degrees to GPRMC standard
      if ((int) abs(latitude) < 10) {
        return "0" + String((int) abs(latitude));
      } //end if 
      else {
        return String((int) abs(latitude));
      } //end else
    } //end getLatitudeDegrees
    String getLatitudeMinutes() { //formatting Latitude Minutes to GPRMC standard 
      if (latitude < 0) {
        latMin = 60.0*(latitude-(int)latitude)*-1.0;
      } //end if
      else {
        latMin = 60.0*(latitude-(int)latitude);
      } //end else
      if (latMin >= 10){ 
        return String(latMin,3);
      } //end if
      else {
        return "0" + String(latMin,3);
      } //end else
    } //getLatitudeMinutes
    String getLongitudeDegrees() { //formatting Longitude Degrees to GPRMC standard
      lonPos = (int) abs(longitude);
      if (lonPos >= 100) {
        return String((int) lonPos);
      } //end if
      else if (10 <= lonPos and lonPos < 100) {
        return "0" + String((int) lonPos);
      } //end else if
      else {
        return "00" + String((int) lonPos);
      } //end else
    } //getLongitudeDegrees
    String getLongitudeMiuntes() { //formatting Longitude Minutes to GPRMC standard 
      if (longitude < 0) {
        lonMin = 60.0*(longitude-(int)longitude)*-1.0;
      } //end if
      else {
        lonMin = 60.0*(longitude-(int)longitude);
      } //end else
      if (0 < lonMin and lonMin < 10){ 
        return "0" + String(lonMin,3);
      } //end if
      else {
        return String(lonMin,3);
      } //end else
    } //getLongitudeMiuntes
    double getLatitudeInRadians() { //convert to Radians
      return (latitude)* PI/180.0;
    } //end getLatitudeInRadians
    double getLongitudeInRadians() { //convert to Radians
      return (longitude)* PI/180.0;
    } //end getLongitudeInRadians       
    void setLatitude(double l) {
      latitude = l;
    } //end setLatitude
    void setLongitude(double l) {
      longitude = l;
    } //end setLongitude
    void setPosition(Position p) {
      latitude=p.getLatitude();
      longitude=p.getLongitude();
    } //end setPosition
    String getLatitudeDirection() { //North or South
      String direction = "N";
      if(latitude < 0) {
        direction = "S";
      } //end if
      return direction;
    } //end getLatitudeDirection
    String getLongitudeDirection() { //East or West
      String direction = "E";
      if(longitude < 0){
        direction = "W";
      } //end if
      return direction;
    } //end getLongitudeDirection      
    String toString() {
      String x;
      x=getLatitudeDegrees()+" "+getLatitudeMinutes()+" "+getLatitudeDirection()+" "+ 
        getLongitudeDegrees()+" "+getLongitudeMiuntes()+" "+getLongitudeDirection();
      return x;
    } //end toString
}; //end Position class

class NMEASentenceGenerator { //produce the proper format for GPRMC 
  public:
    String generateGPRMC(Position p, double bearing, double sog) {
      //time formatting
      String hr;
      String min;
      String sec;
      String mon;
      String d;
      String yr;
      time_t t = now(); // Store the current time in time variable t 
      if (hour(t) < 10) {
        hr = "0" + String(hour(t));
      } //end if
      else {
        hr = String(hour(t));
      } //end else
      if (minute(t) < 10) {
        min = "0" + String(minute(t));
      } //end if
      else {
        min = String(minute(t));
      } //end else
      if (second(t) < 10) {
        sec = "0" + String(second(t));
      } //end if
      else {
        sec = String(second(t));
      } //end else
      if (month(t) < 10) {
        mon = "0" + String(month(t));
      } //end if
      else {
        mon = String(month(t));
      } //end else
      if (day(t) < 10) {
        d = "0" + String(day(t));
      } //end if
      else {
        d = String(day(t));
      } //end else
      yr = String(year()).substring(2);
      String time = hr + min + sec;
      String date = d + mon + yr;
      String magVar = "003.1,W";
      double lat = p.getLatitude();
      double lon = p.getLongitude();
      String latDirection = p.getLatitudeDirection();
      String lonDirection = p.getLongitudeDirection();
      String latS = p.getLatitudeDegrees() + p.getLatitudeMinutes(); //Latitude
      String lonS = p.getLongitudeDegrees() + p.getLongitudeMiuntes(); //Longitude
      String trackAngle = "";
      String sogS = "";
      int checksum = 0;
      String hexChecksum = "";
      trackAngle = (bearing);
      sogS = (sog);
      String RMC = "GPRMC," + time + ",A," + latS + "," + latDirection + ","
        + lonS + "," + lonDirection + "," + sogS + "," + trackAngle + "," + date + 
        "," + magVar;//get checksum
      for (int i = 0; i < RMC.length(); i++) {
        checksum ^= RMC.charAt(i);
      } //end for
      hexChecksum = String(checksum,HEX);
      hexChecksum = ("00" + hexChecksum).substring(hexChecksum.length());
      hexChecksum.toUpperCase();
      return "$" + RMC + "*" + hexChecksum;
    } //end generate GPRMC    
}; //end NMEA class

class EarthGeodesics {
  double R = 6371;
  public:
    double getGreatCircleDistance(Position p0, Position p1) { //find distance of path
      double latStart = p0.getLatitudeInRadians();
      double lonStart = p0.getLongitudeInRadians();
      double latEnd = p1.getLatitudeInRadians();
      double lonEnd = p1.getLongitudeInRadians();
      double dLat = latEnd - latStart;
      double dLon = lonEnd - lonStart;
      double a = pow(sin(dLat / 2), 2) + cos(latStart) * cos(latEnd) * pow(sin(dLon / 2), 2);
      return R * 2 * atan2(sqrt(a), sqrt(1 - a));
    } //end getGreatCircleDistance   
    double getBearing(Position p0, Position p1) {
      //calculate bearing toward final lat/lon coordinate
      double latStart = p0.getLatitudeInRadians();
      double lonStart = p0.getLongitudeInRadians();
      double latEnd = p1.getLatitudeInRadians();
      double lonEnd = p1.getLongitudeInRadians();
      double dLon = lonEnd - lonStart;
      double bearing = (180.0 / PI) * atan2(sin(dLon) * cos(latEnd), cos(latStart) *
        sin(latEnd) - sin(latStart) * cos(latEnd) * cos(dLon));
      if (bearing < 0) {
        bearing += 360;
      } //end if
      return bearing;
    } //end getBearing
    Position getNextPosition(Position p, double distance, double bearing) {
      //use bearing to find next lat/lon coordinate
      double lat = p.getLatitudeInRadians();
      double lon = p.getLongitudeInRadians();
      bearing = bearing * PI / 180.0;
      Position p0 = Position(0,0);
      p0.setLatitude((180.0 / PI) * asin(sin(lat) * cos(distance / R) + cos(lat) *
        sin(distance / R) * cos(bearing)));
      p0.setLongitude((180.0 / PI) * (lon + atan2(sin(bearing) * sin(distance / R) *
        cos(lat), cos(distance / R) - sin(lat) * sin(p0.getLatitudeInRadians()))));
      return p0;
    } //end getNextPosition  
}; //end EarthGeodesics

void loop() {
  if (trips == voyages) { //automatically begin another path after 10 trips
    choice += 1;
    trips = 0;
    if (choice == 15){
      choice = 1;
    } //end if
  } //end if
  //start and end coordinates for path choice
  double latStart = path[choice-1][0];
  double latEnd = path[choice-1][1];
  double lonStart = path[choice-1][2];
  double lonEnd = path[choice-1][3];
  double distanceInPeriod;
  if (trips%2 == 1) { //begin trip in the opposite direction once completed
    latStart = path[choice-1][1];
    latEnd = path[choice-1][0];
    lonStart = path[choice-1][3];
    lonEnd = path[choice-1][2];
  }
  double distanceTraveled = 0;
  Position currentPosition = Position(latStart, lonStart);
  Position startPosition = Position(latStart, lonStart);
  Position endPosition = Position(latEnd, lonEnd);
  EarthGeodesics cs = EarthGeodesics();
  NMEASentenceGenerator nsg = NMEASentenceGenerator();
  bearing = cs.getBearing(startPosition, endPosition);
  currentBearing = bearing;
  totalDistance = cs.getGreatCircleDistance(startPosition, endPosition);
  //Arduino Serial monitor display
  Serial.print("       Starting: ");
  Serial.println(startPosition.toString());
  Serial.print("         Ending: ");
  Serial.println(endPosition.toString());
  Serial.print("Initial Bearing: ");
  Serial.print(currentBearing);
  Serial.println(" Degrees");
  Serial.print("          Speed: ");
  Serial.print(knots);
  Serial.println(" Knots");
  Serial.print(" Total Distance: ");
  Serial.print(totalDistance);
  Serial.println("Km");
  Serial.print("    Serial Port: ");
  Serial.println(wantedPort);
  previousTime = millis();
  delay(period);
  while (answer == true) {
    //periodically print GPRMC to Serial port
    mySerial.println(nsg.generateGPRMC(currentPosition, currentBearing, knots) + "\n\r");
    currentTime = millis();
    distanceInPeriod = (currentTime - previousTime) * knots / 3600000.0;
    distanceTraveled += distanceInPeriod; //keep track of total distance traveled
    currentPosition.setPosition(cs.getNextPosition(currentPosition, distanceInPeriod, 
      currentBearing)); //set the new position as the cuurent position
    currentBearing = cs.getBearing(currentPosition, endPosition); //get new bearing
    previousTime = currentTime;
    if (distanceTraveled >= totalDistance) { //check if the trip has been completed  
      trips += 1;
      delay(period);
      break;
    } //end if
    delay(period);
  } //end while
} //end loop
