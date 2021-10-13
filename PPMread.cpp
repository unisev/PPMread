/*
 PPMread.cpp - Library for reading PPM 6ch (negative modulated).
 V004
 Uniformiser les datatypes
 V003
 Le code retour 2 (failsafe) n'est envoyé qu'une seule fois par trame (32ms)
 Chaque appel de la fonction read dans cet interval donnera un code retour 0
 V002
 Intégration d'un paramètre d'hystérésis dans la fonction read()
 */

#include <PPMread.h>
#include <Arduino.h>

PPMread::PPMread(int* rxPoint) : 
_rxPoint(rxPoint)
{
}

// Données "statiques" du Rx
unsigned char endTrame=0,forWrite=0,forRead=0;
volatile int chVal[2][6];
unsigned char _ecart;
#ifdef LEDPPM
unsigned int failflash;
#endif

void PPMread::begin(unsigned char ecart)
{
  // Timer1 setup
  TCCR1A=B00000000; // Timer: normal mode
  TCCR1B=B00000010; // CAPTUREPIN Interruption sur front déscendant, Timer:normal mode, Prescaler=clk/8 
  TIMSK1=B00100001; // Interruption sur CAPTUREPIN & timer1 Overflow
  _ecart=ecart;
#ifdef LEDPPM
  pinMode(13,OUTPUT);
#endif
}


ISR(TIMER1_CAPT_vect)
{
  static byte curEdge;
  static unsigned int edgeTStmp[8];
  
  edgeTStmp[curEdge]=ICR1; // Capture des timestamp 1 à 6
  curEdge++;
  if (curEdge>7) {     // A partie du 7ème front...   
    TCNT1=0;           // ..reset du compteur pour éviter l'overflow...
    if ((edgeTStmp[7]-edgeTStmp[1]) > 30000) {  //Si la trame totale dépasse 15ms - Trame KO...
      curEdge=0;            // ...On redémarrera la prochaine trame en "sautant" un wagon                      
    }
    else {                   // ...Sinon, une bonne trame est capturée...
      curEdge=1;               // ...on se prépare à compter la prochaine trame de la même façon...
      unsigned int chValTMP;
      for (int i=0;i<6;i++) {
        chValTMP=(edgeTStmp[i+2]-edgeTStmp[i+1]); // ...On mesure chaque canal...
        if (chValTMP > (chVal[forRead][i]+_ecart) || chValTMP<(chVal[forRead][i]-_ecart)) { //...on ne le garde que si la variation par rapport à la valaur précédente est plus grande que l'écart...
          chVal[forWrite][i]=chValTMP; 
        }
        else chVal[forWrite][i]=chVal[forRead][i];
      }
      if (forWrite) {
        forWrite=0;
        forRead=1;
      }
      else{
        forWrite=1;
        forRead=0;
      }
      endTrame=1;           // Pour dire à la fonction read que de nouvelles valeurs sont disponibles

#ifdef LEDPPM
      if (digitalRead(13)==LOW) digitalWrite(13,HIGH);
      else digitalWrite(13,LOW);
#endif
    }
  }
}

ISR(TIMER1_OVF_vect) // Le compteur a attends sa limite de 32ms (overflow)...
{
  endTrame=2;  // ...ce paramètre servira a la fonction read.
#ifdef LEDPPM
  if (failflash==10) {
    failflash=0;
    if (digitalRead(13)==LOW) digitalWrite(13,HIGH);
    else digitalWrite(13,LOW);
  }
  else failflash++;
#endif
}

int PPMread::read()
{
  switch (endTrame) {
  case 2:
    endTrame=0;
    return 2;
    break;
  case 1:
    endTrame=0;
    {
      int* movingPoint=_rxPoint;     // On initialise un pointeur équivalent à celui fourni par la classe...

      for (int i=0;i<6;i++) { 
        *movingPoint=chVal[forRead][i]; // ..On met à jour une valeur...
        movingPoint++;                  // ...on augmente le pointeur d'un.
      }
    }
    return 1;
    break;
  default:
    return 0;
  }
}





















