#include <Arduino.h>


/*******************************************************Fréquence PWM***************************************************************/
const int FPWM = 1000; // Hz



/**********************************Broches de sortie PWM pour le moteur d'essuie-glace**********************************************/
const int pinMotorControlCW = 9;  // Sens horaire




/**********************************Définition des seuils pour les différents modes*************************************************/
const float DUTY_CYCLE_MODE_0 = 0.0;
const float DUTY_CYCLE_MODE_1 = 0.25;
const float DUTY_CYCLE_MODE_2 = 0.75;
const float DUTY_CYCLE_MODE_3 = 1.0;



/********************************************Déclaration des fonctions***********************************************************/
void setupMotorControl();
void setMotorSpeed(float dutyCycle, int pin);

void setup() {
  Serial.begin(9600); // Initialise la communication série à 9600 bauds
  setupMotorControl(); // Initialise le contrôle du moteur d'essuie-glace
}

void loop() {
  if (Serial.available() > 0) {


/*******************************************************Lire le mode reçu via UART**********************************************/
    int mode = Serial.parseInt();
    

/****************************************Calculer le rapport cyclique PWM en fonction du mode***********************************/
    float dutyCycle = 0.0;
    switch (mode) {
      case 0:
        dutyCycle = DUTY_CYCLE_MODE_0;
        break;
      case 1:
        dutyCycle = DUTY_CYCLE_MODE_1;
        break;
      case 2:
        dutyCycle = DUTY_CYCLE_MODE_2;
        break;
      case 3:
        dutyCycle = DUTY_CYCLE_MODE_3;
        break;
      default:
        // Mode par défaut si une valeur non valide est reçue
        dutyCycle = DUTY_CYCLE_MODE_0;
        break;
    }
    

/*************************************************Appliquer le rapport cyclique PWM au moteur**********************************/
   
    setMotorSpeed(dutyCycle, pinMotorControlCW);

  }
}


/*********************************************Initialise le contrôle du moteur d'essuie-glace********************************/
void setupMotorControl() {
  pinMode(pinMotorControlCW, OUTPUT);
  analogWrite(pinMotorControlCW, FPWM);
  
}



/******************************Régle la vitesse du moteur d'essuie-glace en fonction du rapport cyclique********************/
void setMotorSpeed(float dutyCycle, int pin) {
  int pwmValue = dutyCycle * 255; // Conversion du rapport cyclique en valeur PWM (0-255)
  analogWrite(pin, pwmValue);
}


