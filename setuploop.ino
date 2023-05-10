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
