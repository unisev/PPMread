#include <PPMread.h>          // On inclue la librairie

// Données du Rx
int cha[6];           // On crée le tableau des valeurs des canaux

PPMread ppm(&cha[0]);          // On lance l'instance, en fournissant le pointeur du tableau

void setup()
{
  Serial.begin(115200);        // On prepare la sortie série
  Serial.println("");
  Serial.println("PPM read, librarie test");

  ppm.begin();                // On démarre la lecture PPM en arrière-plan, avec un "lissage" de 3 par défaut
}

void loop()
{
switch (ppm.read()) {  // La fonction read retourne 0 si rien n'a changé depuis son dernier appel
  case 1:              // Si read retourne 1, les valeurs ont été mises à jour
    for (char i=0; i < 6; i++){ 
      Serial.print(cha[i]);
      Serial.print(";");
    }
    Serial.println("");    
    break;
  case 2:        // Si read retourne 2, c'est qu'il existe un problème de signal
    Serial.println("FAILSAFE");
  }
}






