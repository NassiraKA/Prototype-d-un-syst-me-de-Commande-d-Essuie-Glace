#include <Stepper.h>

// Définition des broches pour le contrôle du moteur pas à pas
const int stepsPerRevolution = 200;  // Nombre de pas par révolution du moteur

// Création de l'objet Stepper
// Pins : IN1-IN3-IN2-IN4 pour une séquence de pas correcte
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  Serial.begin(9600); // Initialise la communication série à 9600 bauds
}

void loop() {
  if (Serial.available() > 0) {
    // Lire le mode reçu via UART
    int mode = Serial.parseInt();

    // Vérifier si le mode est valide
    if (mode >= 0 && mode <= 3) {
      // Convertir l'angle en nombre de pas
     
      // Définir la vitesse en fonction du mode
      int speed = 10; // Valeur par défaut
      switch (mode) {
        case 0:
          speed = 5; // Vitesse en mode 0 (en tr/min)
          break;
        case 1:
          speed = 10; // Vitesse en mode 1
          break;
        case 2:
          speed = 15; // Vitesse en mode 2
          break;
        case 3:
          speed = 25; // Vitesse en mode 3
          break;
      }
    /*************************************************Déplacer le moteur pas à pas****************************************************/
     // map it to a range from 0 to 100:

    if (speed > 0) {
      // Tourner dans un sens sur 180 degrés
      myStepper.setSpeed(Speed);
    // step 1/100 of a revolution:
      myStepper.step(stepsPerRevolution / 100);
      delay(500); // Attend un court instant avant de changer de direction
      // Retour à la position d'origine
      myStepper.setSpeed(-speed);
      myStepper.step(stepsPerRevolution/2); // Retourne à la position d'origine
      delay(500); // Attend un court instant avant de changer de direction
      // Tourner dans l'autre sens sur 180 degrés
      
     
    } else {
      // Arrête le moteur si la vitesse est 0
      myStepper.setSpeed(0);
      myStepper.step(0);
    }
  }
  }
}
