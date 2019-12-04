#include "stm32l476xx.h"
#include "SysClock.h"
#include "LED.h"
#include "UART.h"
#include "Timer.h"
#include "GPIOINP.h"
#include "POST.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32_t startTime;
extern uint32_t endTime;
uint8_t pulsing = 0;
uint32_t distanceArray[100];
uint8_t distanceIndex = 0;



int comparator(const void * p,const void * q)  
{   
    return (*((uint32_t *) p) - *((uint32_t *) q)); 
} 

int main(void){

	System_Clock_Init(); // Switch System Clock = 80 MHz
	UART2_Init();
  initGPIOA();
  initTIM2(); //1MHz clock frequency
	
	uint32_t lastMeasurement = 0;
	
	
	uint8_t pulseStart = 0;
	uint8_t postSuccess = 0;
	 
  
  //Run the post test, do not continue until POST passes
  putString("Please place an object 100mm away from the sensor.\r\n");
	while(!postSuccess){
		putString("\tPress ENTER when ready");
		char c = getChar();
		while(c != 0x0D){
			
		}
		putString("\r\n");
		setPA3();
		uint32_t currentTime = getCNT();
		while(getCNT() < currentTime + 10){
		}
		resetPA3();
		while(!(startTime && endTime)){
			
		}
		
		char str[100];
		uint32_t timeDifference = endTime - startTime;
		if(timeDifference < 300){
			putString("Object is too close! Try again?\r\n");
		}
		else if(timeDifference > 1000){
			putString("Object is too far! Try again?\r\n");
		}
		else{
			putString("POST was successful!\r\n");
			postSuccess = 1;
		}
		startTime = 0;
		endTime = 0;
	
	}
	
  
	//Main program loop
  while(1){
		
		//Get current time
		uint32_t currentTime = getCNT();
		
		//Sample every .1 seconds
		if(currentTime - lastMeasurement > 100000){
			pulsing = 1;
			setPA3();
			lastMeasurement = currentTime;
		}
		
		if(pulsing && (currentTime - lastMeasurement > 10)){
			resetPA3();
			pulsing = 0;
		}
		
		if(startTime && endTime){
			char str[100];
			uint32_t timeDifference = endTime - startTime;
			uint32_t distance = (timeDifference / 6);
			distanceArray[distanceIndex] = distance;
			distanceIndex++;
			startTime = 0;
			endTime = 0;
		}
		
		if(distanceIndex == 100){

			qsort(distanceArray, 100, sizeof(uint32_t), comparator);
			uint32_t mean = 0;
			for(int i = 0;  i < 100; i++){
				mean += distanceArray[i];
			}
			mean = mean / 100;
			uint32_t min = distanceArray[0];
			uint32_t max = distanceArray[99];
			uint32_t med = distanceArray[50];
		}
		
  }
}



