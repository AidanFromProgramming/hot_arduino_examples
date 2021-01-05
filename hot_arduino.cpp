

const byte numChars = 32;
char receivedChars[numChars];
char flagByte = 'c';
boolean newData = false;
boolean toggle = true;
using namespace std;

enum commands{
    pinOUT='o',
    pinIN='i',
    pinHigh='h',
    pinLow='l',
    pinSet='s',
    pinReadDigital='r',
    pinReadAnalog='R'
};

void setup() {
    Serial.begin(9600);
    pinMode(6, OUTPUT);
    Serial.println("<Arduino is ready>");
}

void loop() {
    recvWithStartEndMarkers();
    if (newData == true) {
        processSerialCommand();
        newData = false;
        byte ran = random(255);
        Serial.print("<cs&");
        Serial.write(ran);
        Serial.print(">");
    }
}

void processSerialCommand(){
    char firstByte = receivedChars[0];
    if (firstByte == flagByte){
        char commandByte = receivedChars[1];
        int targetByte = receivedChars[2] - 32;
        int argumentByte = receivedChars[3];
        switch (commandByte) {
            case pinOUT:
                pinMode(targetByte, OUTPUT);
                break;
            case pinIN:
                pinMode(targetByte, INPUT);
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
            case pinReadDigital: {
                    int value = (digitalRead(targetByte) == HIGH) ? 1 : 0;
                    Serial.print(value);
                }
                break;
            case pinReadAnalog: {
                    int value = analogRead(targetByte);
                    Serial.write(value);
                }
            break;
            default:
                break;
        }
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
