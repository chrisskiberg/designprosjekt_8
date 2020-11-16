// Eksempel kode for oppsett av timer-interrupt for en konstant samplingsfrekvens

#include <TimerOne.h>
/* Om du ikke har innstallert dette biblioteket
gå til Sketch -> Include library -> Manage Libraries
Søk opp TimerOne og innstaller*/

// Globale variaber
volatile int sample; // Holder siste sample
bool newSample; // Støtte varibel for å sjekke om ny sample er tatt
int N0=13;
int N1=17;

int n=10; // stabilitet og liten forsinkelse

int x0[10]; 
int x1[10];

int i;

int samples0[13];
int samples1[17];

int sum0;
int sum1;

int x0_abs_sum;
int x1_abs_sum;

int x0_abs_avg;
int x1_abs_avg;
int d_n;

int LED= 2;
int DECODE=3;


void setup() {
  // Oppsett av timer interrupt
  Timer1.initialize(500); // 500 mikrosekund mellom hver sample -> gir F_s = 2kHz
  // Argumentet i "attachInterrupt" bestemmer hvilken funskjon som er interrupt handler 
  Timer1.attachInterrupt(takeSample); 
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(DECODE, OUTPUT);


  // setter x[n]-verdiene til 0
  for (i = 0; i < n; ++i){
    x0[i]=0;
  }
  
  for (i = 0; i < n; ++i){
    x1[i]=0;
  }

   for (i = 0; i < N0; ++i){
    samples0[i]=0;
   } 
   
  for (i = 0; i < N1; ++i){
    samples1[i]=0;
   } 

   // initialize digital pin LED_BUILTIN as an output.
}





void loop() {
  
  if(newSample){
    // Nye sample er tatt       
      
    // Kode for H0 - Middelverdifilter

      for (i = n-1; i > 0; --i){
        x0[i]=x0[i-1];
      }

      for (i = n-1; i > 0; --i){
        x1[i]=x1[i-1];
      }

      sum0=0;
      sum1=0;

      // r[n] -> x[n]
      for (i = 0; i < N0; ++i){
        sum0+=samples0[i];
      }
      x0[0]=sum0/N0;
      
      // x[n] -> |x[n]|
      // |x[n]| -> x_avg[n]
      x0_abs_sum=0;
      for (i = 0; i < n; ++i){
        x0_abs_sum+=abs(x0[i]);
      }
      int y=0;
      x0_abs_avg=x0_abs_sum/n;
      Serial.print(x0_abs_avg); // blaa
      Serial.print(" ");

    
    
    // Kode for H1 - Middelverdifilter;
    // r[n] -> x[n]
      for (i = 0; i < N1; ++i){
        sum1+=samples1[i];
      }
      x1[0]=sum1/N1;
      
      // x[n] -> |x[n]|
      // |x[n]| -> x_avg[n]
      x1_abs_sum=0;
      for (i = 0; i < n; ++i){
        x1_abs_sum+=abs(x1[i]);
      }
      
      x1_abs_avg=x1_abs_sum/n;
      Serial.println(x1_abs_avg); // reod
      
      // maa forklare magisk tall
      if (x1_abs_avg<700&& x1_abs_avg>395 && x0_abs_avg<700&& x0_abs_avg>395){
        digitalWrite(DECODE, LOW);
      } else { 
        digitalWrite(DECODE, HIGH);
        
        d_n=x0_abs_avg-x1_abs_avg;
        if (d_n<0) {
           digitalWrite(LED, HIGH);
        } else {
           digitalWrite(LED, LOW);
      }
      }

    newSample = false;
  }    
}


// Interrupt-handler (denne kalles ved hvert interrupt)
void takeSample(void){

  for (i = N0-1; i > 0; --i){
       samples0[i]=samples0[i-1];
  }

  for (i = N1-1; i > 0; --i){
       samples1[i]=samples1[i-1];
  }
  
  sample = analogRead(0)-416; // Sampler på A0 - forklare magisk tall
  samples0[0]=sample;
  samples1[0] = samples0[0];
  newSample = true;
  //Serial.write(samples0[0]);
}
