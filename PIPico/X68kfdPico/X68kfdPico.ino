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
  unsigned int DF0=1; // 0=eject, 1=insert, 2=lock, 3=blink
  unsigned int DF1=1; // 0=eject, 1=insert, 2=lock, 3=blink
  unsigned int bit0Blink=0;
  unsigned int bit1Blink=0;
   
  // put your main code here, to run repeatedly:

 while(1) {
    
    if (digitalRead(BT_FD0)) {
       if (DF0==1) {
        DF0=0;
      } else {
        DF0=1;
        digitalWrite(LED_FD0_RED,LOW);
      }
    Serial.print("DF0:");Serial.println(DF0);
    delay(300);
    digitalWrite(FDDINT,LOW);
    }

  if (digitalRead(BT_FD1)==HIGH) {
      if (DF1==1) {
        DF1=0;
      } else {
        digitalWrite(LED_FD1_RED,LOW);
        DF1=1;
      }
    Serial.print("DF1:");Serial.println(DF1);
    delay(300);
    digitalWriteFast(FDDINT,LOW);
    }

   if (digitalRead(OPT0)==LOW) {
    if (digitalRead(Eject)==LOW) {
       DF0=0;
       digitalWrite(FDDINT,LOW);
       Serial.print("Ej 0-");
    }

    if (digitalReadFast(EjectMSK)==LOW) {
      digitalWriteFast(LED_FD0_RED,HIGH);
      } else {
      digitalWriteFast(LED_FD0_RED,LOW);
    }
    if (DF0==1) {
        digitalWriteFast(Inserted,LOW);
        digitalWriteFast(FDDINT,HIGH); //not remove
        digitalWriteFast(LEDFD0,HIGH);
      } else {
         digitalWriteFast(Inserted,HIGH);
     //    digitalWriteFast(FDDINT,LOW);  
         digitalWriteFast(LEDFD0,LOW);
      }
   }  // opt 0

 if (digitalReadFast(OPT1)==LOW) {
  if (digitalReadFast(Eject)==LOW) {
      DF1=0;
      digitalWriteFast(FDDINT,LOW);
      Serial.print("Ej 1-");
    }
   
   if (digitalReadFast(EjectMSK)==LOW) {
      digitalWriteFast(LED_FD1_RED,HIGH);
      } else {
      digitalWriteFast(LED_FD1_RED,LOW);
    }
    
     if (DF1==1) {
        digitalWriteFast(Inserted,LOW);
        digitalWriteFast(FDDINT,HIGH);  // not remove!!!
        digitalWriteFast(LEDFD1,HIGH);
      } else {
         digitalWriteFast(Inserted,HIGH);
      //   digitalWriteFast(FDDINT,LOW); 
         digitalWriteFast(LEDFD1,LOW);
      } 
  
  } //opt1
  
 } //while
 
}
