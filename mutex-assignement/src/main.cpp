#include <Arduino.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Globals

static SemaphoreHandle_t mutex;
const int led_pin = 4;

void BlinkLed(void *parameters)
{
  int num;
  if (xSemaphoreTake(mutex, 0) == pdTRUE)
  { //xSemaphoreTake(mutex handle, number of ticks)
    // return pdTRUE if the mutex was taken
    // else pdFALSE

    num = *(int *)parameters; // derefrencing, crtitical point
  }
  else{
    Serial.println("mutex error");
  }
  Serial.print("Received: ");
  Serial.println(num);

  pinMode(led_pin, OUTPUT);
  while (1)
  {
    digitalWrite(led_pin, 0);

    vTaskDelay(num / portTICK_PERIOD_MS);

    digitalWrite(led_pin, 1);

    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  long Delay = 0;
  // Wait a moment to start (so we don't miss Serial output)
  vTaskDelay(500 / portTICK_PERIOD_MS);

  Serial.println();
  Serial.println("-- Free Rtos Mutex challenge---");
  Serial.println("\nEnter a number");

  // create mutex before task
  mutex = xSemaphoreCreateMutex();

  while (Serial.available() <= 0);

  Delay = Serial.parseInt();
  Serial.print("sending:");
  Serial.println(Delay);

  

  xTaskCreatePinnedToCore(
      BlinkLed,
      "BlinkedLed",
      1024,
      (void *)&Delay, // void *parameters
      1,
      NULL,
      app_cpu);
  Serial.println("done");
  vTaskDelay(500/portTICK_PERIOD_MS); // wait small, to allow the Blinked led task
                                      // to take the mutex first

 // check if mutex was taken, mutex taken=0 and mutex not take =1
 while(1){
   //check if mutex was taken
  if(xSemaphoreTake(mutex, 0) == pdTRUE) { //xSemaphoreTake(mutex handle, number of ticks)
      // return pdTRUE if the mutex was taken
      // else pdFALSE
     
     // if the mutex was taken succesfully 
     // that means it has been released by 
     // other task
     break;
  }
  else{
    // keep looping
  }

 }
}

void loop()
{
  // put your main code here, to run repeatedly:

  // do nothing but allowing yeilding to lower priority tasks
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}