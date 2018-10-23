#include <OneWire.h>

#include <DallasTemperature.h>

#define LED 13

#define sensor_con 2                               
    
    OneWire x(sensor_con);                         
    
    DallasTemperature sensor(&x);                  

int pulsePin = 0;                

int blinkPin = 13;                  

volatile int BPM;          

volatile int Signal;              

volatile int IBI = 600;          

volatile boolean Pulse = false;    

volatile int rate[10];                           

volatile int P = 512;          

volatile int T = 512;        

volatile int thresh = 512;              

volatile int amp = 100;                 

volatile boolean firstBeat = true;      

volatile boolean secondBeat = false;    

volatile unsigned long samplecounter = 0;  
volatile unsigned long lastBeatTime = 0;



void setup()

{

  pinMode(blinkPin,OUTPUT);        
   Serial.begin(9600);       
    Serial.println("Temperature Demo ");

   sensor.begin();                                    
     pinMode(LED, OUTPUT);
                     
  Serial.begin(115200);           

  interruptSetup();                  

}



void loop()

{ 
digitalWrite(LED, HIGH);   
  
  delay(50);                       
  
  digitalWrite(LED , LOW);    
  
  delay(500);                     
  
  Serial.print("Requesting Temperature:");

  sensor.requestTemperatures();                        

  Serial.println("Input successful");
  
  Serial.print("Temperature Readings are:");
  
  Serial.println(sensor.getTempFByIndex(0));        
                                                  
  
      Serial.print("BPM: ");

      Serial.println(BPM);

      delay(200); //  take a break

}



void interruptSetup()

{    

  TCCR2A = 0x02;  
  OCR2A = 0X7C;   
  TCCR2B = 0x06;  
  TIMSK2 = 0x02;  

  sei();          

}


ISR(TIMER2_COMPA_vect)

{ 

  cli();                                     

  Signal = analogRead(pulsePin);            

  samplecounter += 2;                        

  int N = samplecounter - lastBeatTime;      


  if(Signal < thresh && N > (IBI/5)*3)

    {     

      if (Signal < T)

      {                       

        T = Signal;

      }

    }


  if(Signal > thresh && Signal > P)

    {        

      P = Signal;

    }                          



   if (N > 250)

  {                            

    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) )

      {       

        Pulse = true;          

        digitalWrite(blinkPin,HIGH);

        IBI = samplecounter - lastBeatTime;

        lastBeatTime = samplecounter;     



       if(secondBeat)

        {                        

          secondBeat = false;   

          for(int i=0; i<=9; i++)    

          {            

            rate[i] = IBI; //Filling the array with the heart rate values                    

          }

        }


        if(firstBeat)

        {                        

          firstBeat = false;

          secondBeat = true;

          sei();            

          return;           

        }  


      word runningTotal = 0;  


      for(int i=0; i<=8; i++)

        {               

          rate[i] = rate[i+1];

          runningTotal += rate[i];

        }


      rate[9] = IBI;             

      runningTotal += rate[9];   

      runningTotal /= 10;        

      BPM = 60000/runningTotal;

    }                      

  }




  if (Signal < thresh && Pulse == true)

    {  

      digitalWrite(blinkPin,LOW); 

      Pulse = false;             

      amp = P - T;

      thresh = amp/2 + T; 

      P = thresh;           

      T = thresh;

    }


  if (N > 2500)

    {                          

      thresh = 512;                     

      P = 512;                 

      T = 512;               

      lastBeatTime = samplecounter;     

      firstBeat = true;                 

      secondBeat = false;               

    }


 sei();  
}                              
