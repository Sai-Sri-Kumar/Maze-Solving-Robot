#include <Arduino.h>

#define sensor1 A0  // Leftmost sensor
#define sensor2 A1  // Left sensor
#define sensor3 A2  // Center sensor
#define sensor4 A3  // Right sensor
#define sensor5 A4  // Rightmost sensor

#define motorLeft1 5
#define motorLeft2 6
#define motorRight1 9
#define motorRight2 10
#define speedLeft 3
#define speedRight 11

char path[100];  // Store path
int pathLength = 0;  // Path length index
int pathIndex = 0;
bool firstPass = true;
bool mazeChanged = false;

void setup() {
    Serial.begin(9600);
    pinMode(sensor1, INPUT);
    pinMode(sensor2, INPUT);
    pinMode(sensor3, INPUT);
    pinMode(sensor4, INPUT);
    pinMode(sensor5, INPUT);
    
    pinMode(motorLeft1, OUTPUT);
    pinMode(motorLeft2, OUTPUT);
    pinMode(motorRight1, OUTPUT);
    pinMode(motorRight2, OUTPUT);
    pinMode(speedLeft, OUTPUT);
    pinMode(speedRight, OUTPUT);
}

void moveForward() {
    digitalWrite(motorLeft1, HIGH);
    digitalWrite(motorLeft2, LOW);
    digitalWrite(motorRight1, HIGH);
    digitalWrite(motorRight2, LOW);
    analogWrite(speedLeft, 150);
    analogWrite(speedRight, 150);
}

void turnLeft() {
    digitalWrite(motorLeft1, LOW);
    digitalWrite(motorLeft2, HIGH);
    digitalWrite(motorRight1, HIGH);
    digitalWrite(motorRight2, LOW);
    analogWrite(speedLeft, 100);
    analogWrite(speedRight, 150);
}

void turnRight() {
    digitalWrite(motorLeft1, HIGH);
    digitalWrite(motorLeft2, LOW);
    digitalWrite(motorRight1, LOW);
    digitalWrite(motorRight2, HIGH);
    analogWrite(speedLeft, 150);
    analogWrite(speedRight, 100);
}

void stopMoving() {
    digitalWrite(motorLeft1, LOW);
    digitalWrite(motorLeft2, LOW);
    digitalWrite(motorRight1, LOW);
    digitalWrite(motorRight2, LOW);
}

void recordPath(char move) {
    if (firstPass) {
        path[pathLength++] = move;
    }
}

void simplifyPath() {
    if (pathLength < 3 || path[pathLength - 2] != 'B') return;
    int totalAngle = 0;
    for (int i = 1; i <= 3; i++) {
        switch (path[pathLength - i]) {
            case 'R': totalAngle += 90; break;
            case 'L': totalAngle += 270; break;
            case 'B': totalAngle += 180; break;
        }
    }
    totalAngle %= 360;
    switch (totalAngle) {
        case 0: path[pathLength - 3] = 'S'; break;
        case 90: path[pathLength - 3] = 'R'; break;
        case 180: path[pathLength - 3] = 'B'; break;
        case 270: path[pathLength - 3] = 'L'; break;
    }
    pathLength -= 2;
}

void firstPassMaze() {
    int s1 = digitalRead(sensor1);
    int s2 = digitalRead(sensor2);
    int s3 = digitalRead(sensor3);
    int s4 = digitalRead(sensor4);
    int s5 = digitalRead(sensor5);
    
    if (s1 == HIGH) { turnLeft(); recordPath('L'); }
    else if (s2 == HIGH) { turnLeft(); recordPath('L'); }
    else if (s3 == HIGH) { moveForward(); recordPath('S'); }
    else if (s4 == HIGH) { turnRight(); recordPath('R'); }
    else if (s5 == HIGH) { turnRight(); recordPath('R'); }
    else { stopMoving(); recordPath('B'); simplifyPath(); }
}

void secondPassMaze() {
    if (pathIndex >= pathLength) {
        stopMoving();
        return;
    }
    switch (path[pathIndex++]) {
        case 'L': turnLeft(); break;
        case 'R': turnRight(); break;
        case 'S': moveForward(); break;
        case 'B': turnRight(); turnRight(); break;
    }
}

void detectMazeChange() {
    int s1 = digitalRead(sensor1);
    int s2 = digitalRead(sensor2);
    int s3 = digitalRead(sensor3);
    int s4 = digitalRead(sensor4);
    int s5 = digitalRead(sensor5);
    
    if (firstPass == false && (s1 == HIGH || s2 == HIGH || s3 == HIGH || s4 == HIGH || s5 == HIGH)) {
        mazeChanged = true;
    }
}

void loop() {
    detectMazeChange();
    
    if (mazeChanged) {
        Serial.println("Maze changed! Restarting...");
        firstPass = true;
        pathLength = 0;
        pathIndex = 0;
        mazeChanged = false;
        delay(1000);
    }
    
    if (firstPass) {
        firstPassMaze();
        if (pathIndex >= pathLength) {
            firstPass = false;
            pathIndex = 0;
            delay(2000);
        }
    } else {
        secondPassMaze();
    }
}

