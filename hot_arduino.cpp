
#include <Wire.h>
const byte bufferSize = 32;
const int parallelPins[8] = {2,3,4,5,6,7,8,9};
char commandBuffer[bufferSize];

char IOflag = 'c';
boolean newData = false;
using namespace std;

enum IOCommands{
    pinOUT='o',
    pinIN='i',
    parallelOUT='O',
    parallelIN='I',
    pinHigh='h',
    pinLow='l',
    pinSet='s',
    parallelSet='S',
    parallelRead='p',
    pinReadDigital='r',
    pinReadAnalog='R'
};

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithStartEndMarkers();
    processIOCommands();
}

void processIOCommands(){
    if (newData) {
        char firstByte = commandBuffer[0];
        if (firstByte == IOflag) {
            char commandByte = commandBuffer[1];
            int targetByte = commandBuffer[2] - 32;
            byte argumentByte = commandBuffer[3];
            switch (commandByte) {
                case pinOUT:
                    pinMode(targetByte, OUTPUT);
                    break;
                case pinIN:
                    pinMode(targetByte, INPUT);
                    break;
                case parallelOUT: {
                    for(int i = 0; i < 8; i++){
                        pinMode(parallelPins[i], OUTPUT);
                    }
                }
                    break;
                case parallelIN: {
                    for(int i = 0; i < 8; i++){
                        pinMode(parallelPins[i], INPUT);
                    }
                }
                    break;
                case pinHigh:
                    digitalWrite(targetByte, HIGH);
                    break;
                case pinLow:
                    digitalWrite(targetByte, LOW);
                    break;
                case pinSet:
                    analogWrite(targetByte, argumentByte);
                    break;
                case parallelSet: {
                    for(int i = 0; i < 8; i++){
                        digitalWrite(parallelPins[i], (bitRead(targetByte, i)) ? HIGH : LOW);
                    }
                }
                    break;
                case parallelRead: {
                    byte parallelBuffer = 0b00000000;
                    for(int i = 0; i < 8; i++){
                        int value = (digitalRead(parallelPins[i])) ? 1 : 0;
                        bitWrite(parallelBuffer, i, value);
                    }
                    Serial.print("<t");
                    Serial.print(parallelBuffer);
                    Serial.print(">");
                }
                    break;
                case pinReadDigital: {
                    int value = (digitalRead(targetByte) == HIGH) ? 1 : 0;
                    Serial.print("<t");
                    Serial.print(value);
                    Serial.print(">");
                }
                    break;
                case pinReadAnalog: {
                    int value = analogRead(targetByte);
                    Serial.print("<t");
                    Serial.write(value);
                    Serial.print(">");
                }
                    break;
                default:
                    break;
            }
        }
        newData = false;
    }
}

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
                commandBuffer[ndx] = rc;
                ndx++;
                if (ndx >= bufferSize) {
                    ndx = bufferSize - 1;
                }
            }
            else {
                commandBuffer[ndx] = '\0'; // terminate the string
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
