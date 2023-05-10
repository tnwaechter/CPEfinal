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
    readTempHumLCD();
    readWaterLvl();
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
    readWaterLvl();
    
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
    readTempHumLCD();
  
    runDCMotor();
    readWaterLvl();
    
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
