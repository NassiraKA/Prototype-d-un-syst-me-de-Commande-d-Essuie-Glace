/*
 * 
 * Auteur : Hamza Jebbari & Nassira Kaddouri & Ayman Habbaz
 * Date de création : 11/05/2024
 * Description : Ce programme utilise FreeRTOS sur une carte Arduino pour surveiller les niveaux de pluie
 *               et de batterie, calculer le mode de contrôle en fonction des seuils définis, et transmettre
 *               le mode de contrôle via le port série.
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>


/************************************************************ Definition des broches***************************************************/
const int pin_Rain = A0;
const int pin_Battery = A1;


/***********************************************Definition des seuils les niveaux de pluie/batterie***********************************/
const int Rain_low = 50;
const int Rain_medium = 200;
const int Rain_high = 600;
const float Battery_low = 11.6;
const float Battery_high = 12.0;


/************************************************************Déclaration des tâches***************************************************/
void tache_Acquerir_niveau_pluie(void *pvParameters);
void tache_Acquerir_niveau_Battery(void *pvParameters);
void tache_Calcule_Mode(void *pvParameters);
void tache_Transmission_Mode(void *pvParameters);



/********************************************************** Déclaration de sémaphore**************************************************/

SemaphoreHandle_t mutexADC; // Mutex pour l'accès à l'ADC

/*********************************************Déclaration des queues pour la transmission des données*********************************/
QueueHandle_t queueModeTransmission;
QueueHandle_t queueRainTransmission;
QueueHandle_t queueBatteryTransmission;


/*************************************************************************************************************************************/
void setup() {
  
  /**************************************************Initialise le port série à 9600 bauds********************************************/
  Serial.begin(9600); 


  /********************************************************Initialisation des tâches****************************************************/
  xTaskCreate(tache_Acquerir_niveau_pluie, "AcquireRain", 100, NULL, 1, NULL);
  xTaskCreate(tache_Acquerir_niveau_Battery, "AcquireBattery", 100, NULL, 1, NULL);
  xTaskCreate(tache_Calcule_Mode, "CalculateMode", 100, NULL, 2, NULL);
  xTaskCreate(tache_Transmission_Mode, "TransmitMode", 100, NULL, 3, NULL);



  /*****************************************Initialisation des queues pour la transmission des données**********************************/
  queueRainTransmission = xQueueCreate(1, sizeof(float));
  queueBatteryTransmission = xQueueCreate(1, sizeof(float));


  /*******************************************Initialisation de la queue pour la transmission du mode************************************/
  queueModeTransmission = xQueueCreate(1, sizeof(int));



  /********************************************Initialisation du sémaphore pour l'exclusion mutuelle*************************************/


  mutexADC = xSemaphoreCreateMutex();// Initialisation du mutex pour l'ADC
}

void loop() {}

/************************************************* Tâche pour l'acquisition du niveau de pluie********************************************/
void tache_Acquerir_niveau_pluie(void *pvParameters) {
    (void) pvParameters;

    while (1) {
       
        // Acquérir le mutex pour l'ADC
        if (xSemaphoreTake(mutexADC, portMAX_DELAY) == pdTRUE) {
         int rainLevel = analogRead(pin_Rain);
          xSemaphoreGive(mutexADC); // Libérer le mutex pour l'ADC

         float rainVoltage = rainLevel * (5.0 / 1023.0);
         int rainValue = (rainVoltage / 5) * 1000;
 ///////////////////////////////////////////////////////////////////////////////////////// pour tester les valeurs
   Serial.print("Vpluie : ");
   Serial.println( rainValue);
   ////////////////////////////////////////////////////////////////////////////////////////

       
       
        xQueueSend(queueRainTransmission, &rainValue, portMAX_DELAY);
      

        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}



/*******************************************Tâche pour l'acquisition du niveau de batterie********************************************/
void tache_Acquerir_niveau_Battery(void *pvParameters) {
    (void) pvParameters;

    while (1) {

      // Acquérir le mutex pour l'ADC
       if (xSemaphoreTake(mutexADC, portMAX_DELAY) == pdTRUE) {
        
         float batteryLevel = analogRead(pin_Battery);
         xSemaphoreGive(mutexADC); // Libérer le mutex pour l'ADC

         float batteryVoltage = batteryLevel * (5.0 / 1023.0);
         batteryLevel = (batteryVoltage / 5) * 12.8;

///////////////////////////////////////////////////////////////////////////////////// just pour tester       
   Serial.print("Vbattery: ");
   Serial.println( batteryLevel);
  ////////////////////////////////////////////////////////////////////////////////

       
        
        xQueueSend(queueBatteryTransmission, &batteryLevel, portMAX_DELAY);
       

       }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
  


/***************************************************Tâche pour le calcul du mode de contrôle**********************************************/
void tache_Calcule_Mode(void *pvParameters) {
    (void) pvParameters;

    int rainValue;
    float batteryLevel;

    while (1) {

       
        xQueueReceive(queueRainTransmission, &rainValue, portMAX_DELAY);
        xQueueReceive(queueBatteryTransmission, &batteryLevel, portMAX_DELAY);
        

        int mode = 0; // Initialiser mode à 0

        // Calculer le mode de contrôle en fonction des seuils
        if (rainValue < Rain_low) {
            mode = 0;
        } else if (rainValue >= Rain_low && rainValue < Rain_medium && batteryLevel >= Battery_low) {
            mode = 1;
        } else if (rainValue >= Rain_medium && rainValue < Rain_high && batteryLevel >= Battery_low) {
            mode = 2;
        } else if (rainValue >= Rain_high && batteryLevel >= Battery_high) {
            mode = 3;
        }

        // Envoyer le mode calculé à la file d'attente de transmission du mode
        xQueueSend(queueModeTransmission, &mode, portMAX_DELAY);
        
    }
}

/************************************************Tâche pour la transmission périodique du mode de contrôle***********************************/
void tache_Transmission_Mode(void *pvParameters) {
    (void) pvParameters;

    while (1) {
        // Attendre le mode de contrôle calculé
        int mode;
        
       
        xQueueReceive(queueModeTransmission, &mode, portMAX_DELAY);
       

        // Transmettre le mode de contrôle
    Serial.print("Mode de controle : ");
    Serial.println(mode);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
