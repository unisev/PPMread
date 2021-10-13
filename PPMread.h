/*
 PPMread.h V004 - Librairie pour lire un signal PPM 6 canaux.
 Vous devez impértativement connecter votre Rx au PIN 8 de l'Arduino
 */

#ifndef PPMread_h
#define PPMread_h

// #define LEDPPM 1   // Décommenter cette ligne pour utiliser la LED (pin13) de l'Arduino
                      // Elle clignote rapidement si le signal est Ok et lentement sinon.

class PPMread
{
public:
  PPMread(int* rxPoint);
  void begin(unsigned char ecart=3);
  int read();
private:
  int* _rxPoint;
};

#endif





