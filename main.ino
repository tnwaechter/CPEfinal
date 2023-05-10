#include "header.h"

volatile unsigned char *portDDRA = (unsigned char *) 0x21;
volatile unsigned char *portA    = (unsigned char *) 0x22;
volatile unsigned char *portB    = (unsigned char *) 0x25;
volatile unsigned char *portE    = (unsigned char *) 0x2E;
volatile unsigned char *portG    = (unsigned char *) 0x32;
volatile unsigned char *portH    = (unsigned char *) 0x100;

volatile unsigned char *portDDRG = (unsigned char *) 0x33;
volatile unsigned char *portDDRB = (unsigned char *) 0x24;
volatile unsigned char *portDDRE = (unsigned char *) 0x2D;
volatile unsigned char *portDDRH = (unsigned char *) 0x101;

DS3231  clock;
ezButton  Button(B);
RTCDateTime dt;
LiquidCrystal LCD  (RS, E, D4, D5, D6, D7);
Servo SM;
dht DHT;

bool ST_DISABLED, ST_IDLE, ST_ERROR, ST_RUNNING;

int value; //Water Reading
int pos;  //Motor Position
float TEMP = 22;  //Temperature Control
char buf[80]; //Input Buffer
int STATE;  //State of action


// Prototypes
void readTemperatureLCD();
void readWtrLevel();
void runDCMotor();
void checkTime();

void setup() {
//States
    ST_ERROR = false;
    ST_IDLE = false;
    ST_RUNNING = false;
    ST_DISABLED = true;
//LED'S 
    *portDDRA |= 0b00010000;  //Yellow to Output
    *portDDRB |= 0b10000000;  //Green to Output
   
    *portDDRE |= 0b00010000;  //Blue to Output
    *portDDRE |= 0b00100000;  //Red to Output
    
  
    *portA |= 0b11101111; //Turn Yellow OFF
    *portB |= 0b01111111; //Turn Green OFF
    
    *portE |= 0b11101111; //Turn Blue OFF
    *portE |= 0b11011111; //Turn Red OFF
  
//DC Motor
    *portDDRE |= 0b00001000;  //Set to Output
    *portDDRH |= 0b00001000;  //Set to Output
    *portDDRG |= 0b00100000;  //Set to Output

//Servo Motor
    SM.attach(SM_PIN);
    SM.write(90);

//Start Systems
    Serial.begin(9600);
    LCD.begin(16, 2);
    
//Start Clock
    clock.begin();  

//Set Clock
    clock.setDateTime(__DATE__, __TIME__);

//Delay Program
    delay(1000);
}

void loop() 
{
    Button.loop(); // Monitor    
    
    STATE = getState(); //State control
  
    switch(STATE) {
        case 1:
            Disabled();
            break;
        case 2:
            servoPosition();
            Idle();
            break;
        case 3:
            Error();
            break;
        case 4:
            servoPosition();
            Running();
            break;
        default:
            ST_DISABLED = true;
            break;
    }
    delay(3000); //Delay    
}

void Disabled() 
{
    *portA |= 0b00010000;  //Yellow LED ON
    int btnState = Button.getState();
    
  if (btnState == 0) 
  {
        *portA |= 0b11101111; //Yellow LED OFF
        ST_IDLE = true;
        ST_DISABLED = false;
        
  }
}

void Idle() 
{
    *portB |= 0b10000000; //Green LED ON
    readTemperatureLCD();
    readWtrLevel();
    if (DHT.temperature > TEMP) 
    {
        *portB |= 0b01111111; //Green LED OFF
        ST_RUNNING = true;
        ST_IDLE = false;
        
    }
    if (value < 100) 
    {
        *portB |= 0b01111111; //Green LED OFF
        ST_ERROR = true;
        ST_IDLE = false;
        
    }
}
void Error() 
{
    *portE |= 0b00100000; //Red LED ON
    readWtrLevel();
    
    LCD.clear();
    
    LCD.print("Water Level Low! :C ");
    if (value > 200) 
    {
        *portE |= 0b11011111; //Red LED OFF
        ST_IDLE = true;
        ST_ERROR = false;
        
    }
}

void Running() 
{
    checkTime();
    *portE |= 0b00010000; //Blue LED ON
    readTemperatureLCD();
  
    runDCMotor();
    readWtrLevel();
    
  if (DHT.temperature > TEMP) 
  {
        *portE |= 0b11110111; 
        *portE |= 0b11101111; //Blue LED OFF
        ST_IDLE = true;
        ST_RUNNING = false;
       
        
    }
    if (value < 100) {
        *portE |= 0b11110111;
        *portE |= 0b11101111; //Blue LED OFF
        ST_ERROR = true;
        ST_RUNNING = false;
        
    }
    checkTime();
}
int getState() 
{
    int temp;
  
    if (ST_DISABLED)
        temp = 1;
    
  else if(ST_IDLE)
        
      temp = 2;
    
  else if(ST_ERROR)
        
      temp = 3;
    
  else if(ST_RUNNING)
       
      temp = 4;
    return temp;
}

int readline(int readch, char *buffer, int len) 
{
    static int pos = 0;
    
  int rpos;
    if (readch > 0) 
    {
        switch (readch) 
        {
            case '\r':
                break;
            
            case '\n':
                rpos = pos;
                pos = 0;
                return rpos;
            default:
                if (pos < len-1) 
                {
                    buffer[pos++] = readch;
                    buffer[pos] = 0;
                }
        }
    }
    return 0;
}
void readTemperatureLCD() 
{
    DHT.read11(DHT_PIN);
    LCD.clear();
    LCD.setCursor(0,0);
    
    LCD.print("Temp: ");
    LCD.print(DHT.temperature);
    LCD.print((char)223);
    LCD.print("C");
    
    LCD.setCursor(0,1);
    LCD.print("Humidity: ");
    LCD.print(DHT.humidity);
    LCD.print("%");
}

void servoPosition() {
    if (readline(Serial.read(), buf, 80) > 0) 
    {
        Serial.print("Servo Position: ");
        
        Serial.println(buf);
        
        pos = atoi(buf);
        SM.write(pos);
    }
}
void readWtrLevel() 
{
    value = analogRead(WLD_SNS_PIN);
  
    Serial.print("Water Level: ");
    Serial.println(value);
}
void runDCMotor() 
{
    *portE |= 0b00001000;
    *portH |= 0b00001000;
    *portG |= 0b11011111;
}
