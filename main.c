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
#include <math.h>

extern uint32_t startTime;
extern uint32_t endTime;
uint8_t pulsing = 0;
int distanceArray[100];
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
		
		uint16_t interrupted = 0;
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
			
			if(distance > 1000 || distance < 50){
				distanceArray[distanceIndex] = -1;
			}
			else{
				distanceArray[distanceIndex] = distance;
			}
			distanceIndex++;
			startTime = 0;
			endTime = 0;
		}
		
		//interrupts
		if((USART2->ISR & USART_ISR_RXNE)){
			getChar();
			
			for( int i = distanceIndex; i < 100; i++){
				distanceArray[i] = -1;
			}
			interrupted = 1;
			
		}
		
		if(distanceIndex == 99 || interrupted){

			qsort(distanceArray, distanceIndex, sizeof(uint32_t), comparator);
			double mean = 0;
			
			uint16_t usableIndex = 0;
			float meanAmount = 0;
			//find mean
			for(int i = 0;  i < distanceIndex; i++){
				
				char report[100];
				
				if(distanceArray[i] == -1){
					usableIndex++;
					sprintf(report, "***,");
				}
				else{
					mean += distanceArray[i];
					meanAmount += 1.0;
					sprintf(report, "%d,", distanceArray[i]);
				}
				putString(report);
			}
			putString("\r\n");
			mean = mean / meanAmount;
			uint32_t min = distanceArray[usableIndex];
			uint32_t max = distanceArray[distanceIndex-1];
			uint32_t med = distanceArray[((distanceIndex - usableIndex) + ((distanceIndex - usableIndex)/2))];
			//find std deviation
			double stdDev = 0;
			for(int i = usableIndex; i < distanceIndex; i++){
				stdDev += (distanceArray[i] - mean) * (distanceArray[i] - mean);
				if(i == usableIndex + (distanceIndex - usableIndex) /2){
					med = distanceArray[i];
				}

			}
			stdDev = stdDev / (float) ((distanceIndex - usableIndex) - 1);
			stdDev = sqrt(stdDev);
			
			//Report
			char report[100];
			sprintf(report, "Min: %d\r\nMax: %d\r\nMed: %d\r\nMean: %f\r\nStd Dev: %f\r\n",
				min, max, med, mean, stdDev);
			putString(report);
			
			distanceIndex = 0;
		}
		
  }
}



