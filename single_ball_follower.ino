/*
This code is based on the examples at http://forum.arduino.cc/index.php?topic=396450
*/


// Example 5 - parsing text and numbers with start and end markers in the stream

#include "helper.h"

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

dataPacket packet;


boolean newData = false;

//============

void setup() {
    Serial.begin(115200);
    Serial.println("This demo expects 3 pieces of data - text, an integer and a floating point value");
    Serial.println("Enter data in this style <HelloWorld, 12, 24.7>  ");
    Serial.println();
    initServos();
   
}

//============

void loop() {
    recvWithStartEndMarkers();
    if (newData == true) {
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        packet = parseData();
        showParsedData(packet);

        //yawCurrent+=yawVectorToDegrees(packet.yaw);
        //pitchCurrent+=pitchVectorToDegrees(packet.pitch);
        Serial.print("Yaw ten: ");
        Serial.println(packet.yaw);
        Serial.print("Pitch ten: ");
        Serial.println(packet.pitch);
       
        if(yawCurrent<yawMax && packet.yaw<0)
        {
          yawCurrent++;
        }
        else if (yawCurrent>yawMin && packet.yaw>0)
        {
          yawCurrent--;
        }

        
        if(pitchCurrent<pitchMax && packet.pitch<0)
        {
          pitchCurrent++;
        }
        else if (pitchCurrent>pitchMin && packet.pitch>0)
        {
          pitchCurrent--;
        }

        servoYaw.write(yawCurrent);
        servoPitch.write(pitchCurrent);        
        
        newData = false;
    }
    //yawCurrent=yawVectorToDegrees(read_number());
    //pitchCurrent=pitchVectorToDegrees(read_number());
    //servoYaw.write(yawCurrent);
    //servoPitch.write(pitchCurrent);
}

//============

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

//============

dataPacket parseData() {      // split the data into its parts

    dataPacket tmpPacket;

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    strcpy(tmpPacket.message, strtokIndx); // copy it to messageFromPC
 
    strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    tmpPacket.pitch = atoi(strtokIndx);     // convert this part to an integer

    strtokIndx = strtok(NULL, ",");
    tmpPacket.yaw = atoi(strtokIndx);     // convert this part to a float

    return tmpPacket;
}


void showParsedData(dataPacket packet) {
    Serial.print("Message ");
    Serial.println(packet.message);
    Serial.print("Yaw ");
    Serial.println(packet.yaw);
    Serial.print("Pitch ");
    Serial.println(packet.pitch);
}
