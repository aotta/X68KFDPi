/*
 * X68KFDINO - External FD adapter for Sharp X68000
 * by Andrea Ottaviani june 2023
 * 
 * 
Access/Write strobe :

/OPTION_SEL_X : "Chip select" for the extra functions of drive DSX (X: 0-3)

Input / functions signals :

/EJECT_FUNC : If this signal is low when OPTION_SEL_X is asserted, the disk will be ejected.
/LOCK_FUNC (Eject MSK?): If this signal is low when OPTION_SEL_X is asserted, the disk will be locked (eject button off). If this signal is high when OPTION_SEL_X is asserted the disk will be unlocked (eject button on).
/BLINK_FUNC : If this signal is low when OPTION_SEL_X is asserted, the LED blink. If this signal is high when OPTION_SEL_X is asserted, the LED stop blinking.

Output signals (drive status):

/DISK_IN_DRIVE : when OPTION_SEL_X is asserted : Low = a disk in the drive. High = No disk.
/INSERT_FAULT : when OPTION_SEL_X is asserted : Low = a disk insert failure occured . High = No problem. This status is cleared at the rising edge of OPTION_SEL_X.

Output signal (Interrupt line):

/INT : Interrupt line : This signal is activated at each disk status change (inserted/ejected). The interrupt is cleared at the rising edge of /OPTION_SEL_X. (Note : more than one drive can activate this line in the same time -> Softwares scan all disk drive at each /INT assertion).

Note 1: For proper operation all output should be able to drive 5/150=33mA since 150 ohms pull-up are used.
Note 2: The X68000 /OPTION_SEL_X assertion duration is ~2.5us per access. This can depend of the software, the CPU speed.
 */


//Cablage
#define OPT0                2  // Set D2 as OPT0 Read strobe
#define OPT1                3  // Set D3 as OPT1 Read strobe
#define OPT2                4  // Set D4 as OPT2 Read strobe
#define OPT3                5  // Set D5 as OPT3 Read strobe
#define Eject               6  // Set D6 as Eject Read strobe
#define Inserted            7  // Set D7 as Inserted Read strobe
#define FDDINT              8  // Set D8 as FD Interrupt Read strobe
#define Error               9  // Set D9 as Error Read strobe
#define EjectMSK           10  // Set D10 as EjectMSK Read strobe
#define Motor              11  // Set D11 as Motor Read strobe
#define LEDFD0             12  // Set D12 as LED FD0 output (Green LED)
#define LEDFD1             13  // Set D13 as LED FD1 output (Green LED)
#define LED_FD0_RED        14  // Set D14 as LED FD0 output (Red LED)
#define LED_FD1_RED        15  // Set D15 as LED FD1 output (Red LED)
#define BT_FD0             16  // Set D16 as Button FD0 Read strobe
#define BT_FD1             17  // Set D17 as Button FD1 Read strobe
#define LED_BLINK          18  // Set A0 as LED (Blink) output


void setup() {
  // put your setup code here, to run once:
  // initialize digital pin 13 as an output.
  pinMode(OPT0, INPUT);
  pinMode(OPT1, INPUT);
  pinMode(OPT2, INPUT);
  pinMode(OPT3, INPUT);
  pinMode(Eject, INPUT);
  pinMode(BT_FD0, INPUT);
  pinMode(BT_FD1, INPUT);
  pinMode(LED_BLINK, INPUT); // Blink
  pinMode(EjectMSK, INPUT); // LOCK (Red led)
  pinMode(Motor, INPUT); // 
  pinMode(Inserted, OUTPUT);
  pinMode(Error, OUTPUT);
  pinMode(FDDINT, OUTPUT);
  pinMode(LEDFD0, OUTPUT);
  pinMode(LEDFD1, OUTPUT);
  pinMode(LED_FD0_RED, OUTPUT); // used for LED LOCK, Red light
  pinMode(LED_FD1_RED, OUTPUT); // used for LED LOCK, Red light
  
  Serial.begin(115200);

}

void loop() {
  unsigned char DF0=1; // 0=eject, 1=insert, 2=lock, 3=blink
  unsigned char DF1=1; // 0=eject, 1=insert, 2=lock, 3=blink
  unsigned char bitBlink=0;
  unsigned short int BlinkCounter=0;
   
  // put your main code here, to run repeatedly:
 Serial.println("Start");
     digitalWrite(Inserted,HIGH);
  while(1) {

    BlinkCounter++;
    if (BlinkCounter>=29999) BlinkCounter=0;
    
    if (digitalRead(BT_FD0)) {
      digitalWrite(FDDINT,LOW);
       if (DF0==1) {
        DF0=0;
       } else {
        bitBlink=0;
        DF0=1;
         }
    Serial.print("DF0:");Serial.println(DF0);
    delay(200);
    }
   
  if (digitalRead(BT_FD1)) {
      digitalWrite(FDDINT,LOW);
        if (DF1==1) {
        DF1=0;
        } else {
        bitBlink=0;
        DF1=1;
       }
    Serial.print("DF1:");Serial.println(DF1);
    delay(200);
    }
  
  if (!digitalReadFast(LED_BLINK)) {
       bitBlink=1;
     // Serial.print("Led Blink");
      }    

  if (!digitalReadFast(OPT0)) {

    if (!digitalReadFast(Eject)) {   // era Eject
       DF0=0;
    //   digitalWriteFast(FDDINT,LOW);   // ERA LOW
       Serial.println("Ej 0-");
    }      
    if (!DF0) {
        digitalWriteFast(Inserted,HIGH);  // for TT!!        
      } else {
        digitalWriteFast(Inserted,LOW);
         digitalWriteFast(Error,HIGH);  
         digitalWriteFast(FDDINT,HIGH);  // not remove!! era HIGH
      }

    if (!digitalReadFast(EjectMSK)) {
      digitalWriteFast(LED_FD0_RED,HIGH);
      digitalWriteFast(LEDFD0,LOW);
       Serial.print("EjMSK 0-");
      } else {
      digitalWriteFast(LED_FD0_RED,LOW);
    }
 

 }  // opt 0

 if (!digitalReadFast(OPT1)) {
    if (!(digitalReadFast(Eject))) { // era eject
     DF1=0;
     //digitalWriteFast(FDDINT,LOW);
     Serial.println("Ej 1-");
    }
      
   if (!DF1) {
        digitalWriteFast(Inserted,HIGH);  // for TT!!        
      } else {
        digitalWriteFast(Inserted,LOW);
         digitalWriteFast(Error,HIGH);  
         digitalWriteFast(FDDINT,HIGH);  // not remove!! era HIGH
      }

    if (!digitalReadFast(EjectMSK)) {
      digitalWriteFast(LED_FD1_RED,HIGH);
      digitalWriteFast(LEDFD1,LOW);
     Serial.print("EjMSK 1-");
      } else {
      digitalWriteFast(LED_FD1_RED,LOW);
    }
    
     digitalWrite(FDDINT,HIGH);
  
  } //opt1

  if (!DF0) {
        digitalWriteFast(LEDFD0,LOW);
        digitalWriteFast(Inserted,LOW);  // for TT!!  
        digitalWriteFast(FDDINT,HIGH);  // not remove!! era HIGH
      } else {
        digitalWriteFast(LEDFD0,HIGH);
        digitalWriteFast(Inserted,HIGH);
        digitalWriteFast(Error,HIGH);  
        digitalWriteFast(FDDINT,HIGH);  // not remove!! era HIGH
      }
  if (!DF1) {
        digitalWriteFast(LEDFD1,LOW);
        digitalWriteFast(Inserted,LOW);  // for TT!!  
        digitalWriteFast(FDDINT,HIGH);  // not remove!! era HIGH
    } else {
         digitalWriteFast(LEDFD1,HIGH);
        digitalWriteFast(Inserted,HIGH);
        digitalWriteFast(Error,HIGH);  
        digitalWriteFast(FDDINT,HIGH);  // not remove!! era HIGH
      }

  if (bitBlink==1) {
    if (!DF0) {
      // Serial.print("b0"); // do not remove, needed for timing
       delay(10);
      if (BlinkCounter>=3000) {
        digitalWriteFast(LEDFD0,LOW);
      } else {
        digitalWriteFast(LEDFD0,HIGH);
      }
    }
    if (!DF1) {
      //Serial.print("b1"); // do not remove, needed for timing
       delay(10);
    if (BlinkCounter>=3000) {
        digitalWriteFast(LEDFD1,LOW);
      } else {
        digitalWriteFast(LEDFD1,HIGH);
      }
    }
  }
  
 } //while
 
}