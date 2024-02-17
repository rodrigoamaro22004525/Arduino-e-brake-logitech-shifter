





//A0 E A2 SAO PARA O SHIFTER A1 PARA O HANDBRAKE QUE NESTE CASO É O ACELERADOR






#include <Joystick.h>

// H-shifter mode analog axis thresholds
#define HS_XAXIS_12        400 //default 390
#define HS_XAXIS_56        640
#define HS_YAXIS_135       595
#define HS_YAXIS_246       350



// Handbrake mode analog axis limits
#define HB_MAXIMUM         530
#define HB_MINIMUM         400
#define HB_RANGE           (HB_MAXIMUM-HB_MINIMUM)

// Digital inputs definitions
#define DI_REVERSE         1
#define DI_MODE            3
#define DI_RED_CENTERRIGHT 4
#define DI_RED_CENTERLEFT  5
#define DI_RED_RIGHT       6
#define DI_RED_LEFT        7
#define DI_BLACK_TOP       8
#define DI_BLACK_RIGHT     9
#define DI_BLACK_LEFT      10
#define DI_BLACK_BOTTOM    11
#define DI_DPAD_RIGHT      12
#define DI_DPAD_LEFT       13
#define DI_DPAD_BOTTOM     14
#define DI_DPAD_TOP        15

// Shifter state
#define DOWN_SHIFT         -1
#define NO_SHIFT           0
#define UP_SHIFT           1

// Shifter mode
#define SHIFTER_MODE       0
#define HANDBRAKE_MODE     1

/* old unsmoothed pedals code (working)
//int zAxis_ = 0; only used for 5AXIS
//int RxAxis_ = 0; only used for 5AXIS                   
int RyAxis_ = 0;  
int RzAxis_ = 0;          
int Throttle_ = 0;         
*/

const bool initAutoSendState = true; 

// LED blink counter
int led=0;

// Shifter state
int shift=NO_SHIFT;

// Handbrake mode
int mode=SHIFTER_MODE;

int b[16];

int gear=0;                          // Default value is neutral

// Constant that maps the phyical pin to the joystick button.
//const int pinToButtonMap = 9;

// Last state of the button
int lastButtonState = 0;

//test smoothing code initialize

const int numReadings = 20;

int thReadings[numReadings];      // throttle readings from the analog input
int thReadIndex = 0;              // throttle index of the current reading
int thTotal = 0;                  // throttle running total
int thAverage = 0;                // throttle average
int thMapped = 0;                 // mapped throttle


//end of test smoothing code initialize

// Create the Joystick
Joystick_ Joystick;

//shifter delay using millis() initialize
int period = 100;
unsigned long time_now = 0; 
//end of shifter delay using millis() initialize

//combined setup() for pedals and shifter
void setup()
{
  // G29 shifter analog inputs configuration, A0 changed to A4, and A2 changed to A3
  pinMode(A0, INPUT_PULLUP);   // X axis
  pinMode(A1, INPUT_PULLUP);   // Y axis

  pinMode(2, INPUT); 

  for(int i=0; i<16; i++) b[i] = 0;
  b[DI_MODE] =0;
  
  // Initialize Joystick Library
  Joystick.begin();

  //test smoothing code setup
  
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    thReadings[thisReading] = 0;

  }
  
  //end of test smoothing code setup
}

//combined loop() for pedals and shifter
void loop(){
  
  /* old unsmoothed pedals code (working)
  RyAxis_ = analogRead(A0);
  RyAxis_ = map(RyAxis_,0,1023,0,255);
  Joystick.setRyAxis(RyAxis_);

  RzAxis_ = analogRead(A1);
  RzAxis_ = map(RzAxis_,1023,0,255,0);            
  Joystick.setRzAxis(RzAxis_);
  
  Throttle_ = analogRead(A2);
  Throttle_ = map(Throttle_,1023,0,255,0);         
  Joystick.setThrottle(Throttle_);                
  */

  //test smoothing code for pedals

  //throttle
   thTotal = thTotal - thReadings[thReadIndex];
   thReadings[thReadIndex] = analogRead(A2);
   thTotal = thTotal + thReadings[thReadIndex];
   thReadIndex = thReadIndex + 1;

   if (thReadIndex >= numReadings) {
     thReadIndex = 0;
   }

   thAverage = thTotal / numReadings;
   thMapped = map(thAverage,2023,64,140,0);

   Joystick.setThrottle(thMapped);


  
  //end of test smoothing code for pedals

  //shifter delay using millis()
  if (millis() >= time_now + period) {
    time_now += period;

    int x=analogRead(0);                 // X axis
    int y=analogRead(1);                 // Y axis
  
    int _isreverse = digitalRead(2) && y<HS_YAXIS_246 && x>HS_XAXIS_56;;
    int _gear_ = 0;
  
    if( _isreverse == 1 ){
  
        _gear_ = 7;
        b[DI_REVERSE]= 1;
  
    }else{ 
  
    if(b[DI_MODE]==0)                    // H-shifter mode?
      {
        if(x<HS_XAXIS_12)                  // Shifter on the left?
        {
          if(y>HS_YAXIS_135) _gear_=1;       // 1st gear
          if(y<HS_YAXIS_246) _gear_=2;       // 2nd gear
        }
        else if(x>HS_XAXIS_56)             // Shifter on the right?
        {
          if(y>HS_YAXIS_135) _gear_=5;       // 5th gear
          if(y<HS_YAXIS_246) _gear_=6;       // 6th gear
         
        }
        else                               // Shifter is in the middle
        {
          if(y>HS_YAXIS_135) _gear_=3;       // 3rd gear
          if(y<HS_YAXIS_246) _gear_=4;       // 4th gear
        }
      }
    }
    
    
    if(gear!=6) b[DI_REVERSE]=0;         // Reverse gear is allowed only on 6th gear position
    
    if (_gear_ != gear ){
      gear = _gear_;
      depress();
      Joystick.setButton(gear-1, HIGH);
    }
  }
  //end of shifter delay using millis()
}
void depress(){
  // Depress virtual button for current gear
  for(int i = 0; i <= 10 ; i++ )  Joystick.setButton(i, LOW);
}

