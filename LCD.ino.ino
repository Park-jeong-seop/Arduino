#include "SoftwareSerial.h"
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

Servo locker;

LiquidCrystal lcd(12,11,5,4,3,2); 

char input[4];
char newInput[4];
char key;

const byte rows = 4;
const byte cols = 4;

byte rowPins[rows] = {8, 7, 6, 5};
byte colPins[cols] = {4, 3, 2, 1};

char secretCode[4] = {'1', '2', '3', '4'}; 

char adminCode[3] = {'#', '#', '#'}; 
char adminInput[3];

int a = 0;
int n = 0;
int i = 0;
int j = 0;
int wrong = 0;
int Adwrong = 0;

char keys[rows][cols] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols); 

#define trigPin 12 
#define echoPin 13
long cm, duration;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  locker.attach(11); 

   // 서보모터의 디폴트 각도 지정
   locker.write(0); 

   // 서보모터 핀모드
   pinMode(10,INPUT); 

   pinMode(trigPin, OUTPUT); 
    pinMode(echoPin, INPUT); 
}

void loop() { 
   start();   
   while (a == 0) {    
        // 비밀번호 입력 모드
        inputKey();
        
        // 비밀번호 확인
        password();
        
        // 현재 상태 LCD에 보여줌
        passwordCheck();       
    }
    
     // 만약 비밀번호가 맞다면
    if (a == 1) {
        pinMode(10,OUTPUT);

        // 서보모터 90도로 회전하며 금고 열림
        locker.write(90);      
        delay(1000);
        pinMode(10,INPUT);

        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 1\r\n");
        Serial.print("$PRINT  * : Lock\r\n");
        Serial.print("$GO 2 1\r\n");
        Serial.print("$PRINT  # : AdminMode\r\n");
        key = keypad.waitForKey();
    
        if (key == '*') {  
            lockOn();
        }
        else if (key == '#') {
            // admin mode를 위한 input 가능해짐.
            Serial.println("Admin Mode Available ");
            inputAdmin();

            AdminPassword();

            AdminPasswordCheck();
        }
    }
}

void start(){
  Serial.print("$CLEAR\r\n");
    while(1){
        digitalWrite(trigPin, LOW);
        delayMicroseconds(10); 
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10); 
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        cm = microsecondsToCentimeters(duration);
        if(cm <2){
          break;
        }
        Serial.print(cm);
        Serial.println(" cm");
        delay(100);
    }
}

long microsecondsToCentimeters(long microseconds)
{
    // The speed of sound is 340 m/s or 29 microseconds per centimeter.
    // The ping travels out and back, so to find the distance of the
    // object we take half of the distance travelled.
    
    return microseconds / 29 / 2;
}

void inputKey() {
    Serial.print("$CLEAR\r\n");                      
    Serial.print("$GO 1 5\r\n");
    Serial.print("$PRINT     Password\r\n");
    Serial.print("$GO 2 6\r\n");
    Serial.print("$PRINT :\n");
    for (i = 0; i < 4; i++) {
        key = keypad.waitForKey();
        input[i] = key;
    
        if (input[0] == '#') {
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 1\r\n");

            // 첫 입력부터 # 버튼 누를 시
            Serial.print("$PRINT      '#'is \r\n");
            delay(2000);
            Serial.print("$GO 1 7\r\n");
            
            // 삭제키라는 사실 알려줌
            Serial.print("$PRINT    Delete Key\r\n");
            delay(2000);

            // 다시 비밀번호 첫자리 입력하도록
            i = i - 1; 
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 5\r\n");
            Serial.print("$PRINT     Password\r\n");
            Serial.print("$GO 2 6\r\n");
            Serial.print("$PRINT :\n");
        }
    
        else if (input[0] == '*') {
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 1\r\n");

            // 첫 입력부터 * 버튼 누를 시
            Serial.print("$PRINT      '*' is \r\n"); 
            delay(2000);  
            Serial.print("$GO 1 8\r\n");

            // 잠금키라는 사실 알려줌
            Serial.print("$PRINT     Lock Key\r\n");
            delay(2000);

            // 다시 비밀번호 첫자리 입력하도록
            i = i - 1; 
            Serial.print("$CLEAR\r\n");                      
            Serial.print("$GO 1 5\r\n");
            Serial.print("$PRINT     Password\r\n");
            Serial.print("$GO 2 6\r\n");
            Serial.print("$PRINT :\n");
        }
      
        else if (i > 0 && input[i] == '#') {
            i = i - 1;
            if(i==0){
              Serial.print("$GO 2 6\r\n");
              // 직전에 입력했던 key 삭제
              Serial.print("$PRINT : \n");
            }
            else if(i==1){
              Serial.print("$GO 2 6\r\n");
              // 직전에 입력했던 key 삭제
              Serial.print("$PRINT :* \n");
            }
            else if(i==2){
              Serial.print("$GO 2 6\r\n");
              // 직전에 입력했던 key 삭제
              Serial.print("$PRINT :** \n");
            }
            // 컴퓨터 모니터에도 보여줌
            Serial.print(" "); 

            // 직전 key 다시 입력하도록
            i = i - 1; 
        }

        // * key는 패스워드에 포함되지 않도록
        else if (input[i] == '*') {
            i = i - 1;
        }
    
        else {
            if(i==0){
              Serial.print("$GO 2 7\r\n");
            }
            else if(i==1){
              Serial.print("$GO 2 8\r\n");
            }
            else if(i==2){
              Serial.print("$GO 2 9\r\n");
            }
            Serial.print("$PRINT  *\n");
            Serial.print(input[i]);
            Serial.print(",");
        }
    
      
        if (i == 3) {
            Serial.println("");
        }
    }
}


void password() {
    wrong = 0;
    for (j = 0; j < 4; j++) {
        if (secretCode[j] == input[j]) {
        }
        
        else if (secretCode[j] != input[j]) {
            wrong += 1;
        }
    }
}


void passwordCheck() {
    if (wrong == 0) {
        Serial.println("correct");
        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 6\r\n");
        Serial.print("$PRINT     CORRECT\r\n");
        delay(1000);
        a = 1;
        Serial.print("$CLEAR\r\n");
    }
    
    else if (wrong != 0) {
        Serial.println("wrong");
        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 7\r\n");
        Serial.print("$PRINT      WRONG\r\n");
        delay(2000);
        a = 0;
        Serial.print("$CLEAR\r\n");
        wrong = 0;
    }
}

// 금고 잠금
void lockOn() {
    // flag 다시 0으로 초기화 
    a = 0;
                      
    Serial.print("$CLEAR\r\n");
    Serial.print("$GO 1 6\r\n");
    pinMode(10,OUTPUT);

    Serial.print("$PRINT Lock ON\r\n");

    // 서보모터 0도로 회전하며 금고 잠금
    locker.write(0);     
    //ringPiezo(99);
      
    delay(2000);
    pinMode(10,INPUT);  
}

void AdminPassword()  {
  Adwrong = 0;
    for (j = 0; j < 3; j++) {
        if (adminCode[j] == input[j]) {
        }
        
        else if (adminCode[j] != input[j]) {
            Adwrong += 1;
        }
    }
}

void AdminPasswordCheck() {
  if (Adwrong == 0) {
        Serial.println("correct");
        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 6\r\n");
        Serial.print("$PRINT     CORRECT\r\n");
        delay(2000);
        Serial.print("$CLEAR\r\n");
        adminMode();
    }
    
    else if (Adwrong != 0) {
        Serial.println("wrong");
        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 7\r\n");
        Serial.print("$PRINT      WRONG\r\n");
        delay(2000);
        Serial.print("$CLEAR\r\n");
        Adwrong = 0;
        lockOn();
    }
}

void inputAdmin() {
    Serial.print("$CLEAR\r\n");
    Serial.print("$GO 1 2\r\n");
    pinMode(10,OUTPUT);

    Serial.print("$PRINT Admin mode\r\n"); 
    delay(1000);
    pinMode(10,INPUT); 

    Serial.print("$CLEAR\r\n");
    Serial.print("$GO 1 \r\n");
    Serial.print("$PRINT Admin Password\r\n"); 
    
    for (i = 0; i < 3; i++) {
        key = keypad.waitForKey();
   
            Serial.print(i);
            input[i] = key;

            if(i==0){
              Serial.print("$GO 2 7\r\n");
            }
            else if(i==1){
              Serial.print("$GO 2 8\r\n");
            }
            else if(i==2){
              Serial.print("$GO 2 9\r\n");
            }
            Serial.print("$PRINT  *\n");
    }
}

void adminMode()
{
    Serial.println("---- Admin Mode On ----");
  
    Serial.print("$CLEAR\r\n");
    pinMode(10,OUTPUT);

    Serial.print("$GO 1 1\r\n");
    Serial.print("$PRINT 1. P/W Change\r\n");
  
    delay(2000);
    pinMode(10,INPUT);
    key = keypad.waitForKey();
  
    
    if (key == '1') {
        while (n == 0) {
            // 새 비밀번호 입력
            inputNewPassword();  
            
            // 새 비밀번호 다시 입력 
            inputNewPasswordAgain(); 
            
            // 위 두 번의 새 비밀번호 입력을 대조해봄
            newPassword(); 
            
            // 그 결과를 LCD로 띄움, wrong값 초기화
            newPasswordCheck(); 
            
        }
  
        if (a == 1) {
            // 두 값이 같을 때 비밀번호 변경
            setNewPassword(); 
        }
    }
}

void inputNewPassword() {
    // 비밀번호 변경
    Serial.println("Password change");
    
    Serial.print("$CLEAR\r\n");
    pinMode(10,OUTPUT);
  
    
    Serial.print("$GO 1 2\r\n");
    Serial.print("$PRINT Input New P/W\r\n");
    Serial.print("$GO 2 6\r\n");
    Serial.print("$PRINT :\n");

    for (i = 0; i < 4; i++) {
        key = keypad.waitForKey();
        input[i] = key;
      
        if (input[0] == '#') {
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 1\r\n");

            Serial.print("$PRINT      '#'is \r\n");
            delay(2000);
            Serial.print("$GO 1 7\r\n");

            Serial.print("$PRINT    Delete Key\r\n");
            delay(2000);
        
            i = i - 1; 
            Serial.print("$CLEAR\r\n");                     
            Serial.print("$GO 1 1\r\n");
            Serial.print("$PRINT Input New P/W\r\n");
            Serial.print("$GO 2 6\r\n");
            Serial.print("$PRINT :\n");
        }
    
        else if (input[0] == '*') {
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 1\r\n");

            Serial.print("$PRINT      '*' is \r\n"); 
            delay(2000);  
            Serial.print("$GO 1 8\r\n");

            Serial.print("$PRINT Lock Key\r\n");
            delay(2000);
        
            i = i - 1;
            Serial.print("$CLEAR\r\n");                     
            Serial.print("$GO 1 4\r\n");
            Serial.print("$PRINT Password\r\n");
            Serial.print("$GO 2 6\r\n");
            Serial.print("$PRINT :\n");
        }
      
        else if (i > 0 && input[i] == '#') {
            i = i - 1;
            if(i==0){
              Serial.print("$GO 2 6\r\n");
              Serial.print("$PRINT : \n");
            }
            else if(i==1){
              Serial.print("$GO 2 6\r\n");
              Serial.print("$PRINT :* \n");
            }
            else if(i==2){
              Serial.print("$GO 2 6\r\n");
              Serial.print("$PRINT :** \n");
            }
            Serial.print(" "); 

            i = i - 1; 
        }
    
        else if(input[i] == '*') {
            i = i - 1;
        }
    
        else {
            if(i==0){
              Serial.print("$GO 2 7\r\n");
            }
            else if(i==1){
              Serial.print("$GO 2 8\r\n");
            }
            else if(i==2){
              Serial.print("$GO 2 9\r\n");
            }
            Serial.print("$PRINT  *\n");
            Serial.print(input[i]);
            Serial.print(",");
        }
    
      
        if (i == 3) {
            Serial.println("");
        }
    }
}




void inputNewPasswordAgain() {
    Serial.println("New password Check Again.");
    Serial.print("$CLEAR\r\n");
    Serial.print("$GO 1 3\r\n");
    Serial.print("$PRINT Please Again\r\n");
    Serial.print("$GO 2 6\r\n");
    Serial.print("$PRINT :\n");
    
    for (i = 0; i < 4; i++) {
        key = keypad.waitForKey();
        newInput[i] = key;
      
        if (newInput[0] == '#') {
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 1\r\n");

            Serial.print("$PRINT      '#'is \r\n");
            delay(2000);
            Serial.print("$GO 1 7\r\n");

            Serial.print("$PRINT    Delete Key\r\n");
            delay(2000);
        
            i = i - 1; 
            Serial.print("$CLEAR\r\n");                     
            Serial.print("$GO 1 1\r\n");
            Serial.print("$PRINT Input New P/W\r\n");
            Serial.print("$GO 2 6\r\n");
            Serial.print("$PRINT :\n");
        }
      
        else if (newInput[0] == '*') {
            Serial.print("$CLEAR\r\n");
            Serial.print("$GO 1 1\r\n");

            Serial.print("$PRINT      '*' is \r\n"); 
            delay(2000);  
            Serial.print("$GO 1 8\r\n");

            Serial.print("$PRINT Lock Key\r\n");
            delay(2000);
        
            i = i - 1;
            Serial.print("$CLEAR\r\n");                     
            Serial.print("$GO 1 4\r\n");
            Serial.print("$PRINT Password\r\n");
            Serial.print("$GO 2 6\r\n");
            Serial.print("$PRINT :\n");
        }
        
        else if (i > 0 && newInput[i] == '#') {
            i = i - 1;
            if(i==0){
              Serial.print("$GO 2 6\r\n");
              Serial.print("$PRINT : \n");
            }
            else if(i==1){
              Serial.print("$GO 2 6\r\n");
              Serial.print("$PRINT :* \n");
            }
            else if(i==2){
              Serial.print("$GO 2 6\r\n");
              Serial.print("$PRINT :** \n");
            }
            Serial.print(" "); 

            i = i - 1; 
        }
      
        else if (newInput[i] == '*') {
            i = i - 1;
        }
      
        else {
            if(i==0){
              Serial.print("$GO 2 7\r\n");
            }
            else if(i==1){
              Serial.print("$GO 2 8\r\n");
            }
            else if(i==2){
              Serial.print("$GO 2 9\r\n");
            }
            Serial.print("$PRINT  *\n");
            Serial.print(newInput[i]);
            Serial.print(",");
        }
        
        if (i == 3) {
            Serial.println(""); 
        }
    }
}



void newPassword() {
    Serial.println("new Password matching...");
    wrong = 0;
    
    for (j = 0; j < 4; j++) {
        if (input[j] == newInput[j]) {
        }
        
        else if (input[j] != newInput[j]) {
            wrong += 1;
        }
    }
}


void newPasswordCheck() {
    // 새 비밀번호가 일치하면
    if (wrong == 0) {
        Serial.println("setting new Password...");
        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 1\r\n");
        Serial.print("$PRINT Setting Password...\r\n");
        delay(2000);
        n = 1;
    }
    
    else if (wrong != 0) {
        Serial.println("New password is NOT correct");
        Serial.print("$CLEAR\r\n");
        Serial.print("$GO 1 1\r\n");
        Serial.print("$PRINT NOT CORRECT!\r\n");
    
        delay(2000);
        n = 0;
        wrong = 0;
    }
}


void setNewPassword() {
    Serial.println("SET NEW PASSWORD");
    
    for (j = 0; j < 4; j++) {
      secretCode[j] = newInput[j];
      Serial.print(newInput[j]);
    }
    
    Serial.println("");
  
    Serial.print("$CLEAR\r\n");
    Serial.print("$GO 1 1\r\n");
    Serial.print("$PRINT SET NEW PASSWORD\r\n");
  
    delay(2000);

    // 다시 n값 초기화
    n = 0; 
}
