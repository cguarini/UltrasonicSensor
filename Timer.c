#include "Timer.h"
#include "UART.h"

#include <string.h>
#include <stdio.h>
char buf[100];

uint32_t startTime = 0;
uint32_t endTime = 0;
extern uint8_t pulsing;


void TIM2_IRQHandler(void){
	if(!pulsing){
    if(startTime){
			endTime = TIM2->CCR1;
		}
		else{
			startTime = TIM2->CCR1;
		}
		
		//clear interrupt
    TIM2->SR &= ~(TIM_SR_CC1IF);
		TIM2->SR &= ~(TIM_SR_UIF);
		TIM2->SR &= ~(TIM_SR_CC1OF);
		
	}
}

void initTIM2(){
    //TODO Enable TIM2 Clock
    RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);
    //Load Prescaler
    TIM2->PSC = 79;
    //Generate event
    TIM2->EGR |= TIM_EGR_UG;
    
    //Disable Timer
    TIM2->CCER &= (~TIM_CCER_CC1E);
    
    //Enable input capture
    TIM2->CCMR1 |= (TIM_CCMR1_CC1S & 1);
    
    //Select edge of active transition
    TIM2->CCER |= (TIM_CCER_CC1NP);
    TIM2->CCER |= (TIM_CCER_CC1P);
    
    //Clear input prescaler
    TIM2->CCMR1 &= (~TIM_CCMR1_IC1PSC);
	
		TIM2->DIER |= TIM_DIER_CC1IE;
		NVIC_EnableIRQ(TIM2_IRQn);
	
   
    
    //enable timer
    TIM2->CCER |= (TIM_CCER_CC1E);
    
    TIM2->CR1 |= TIM_CR1_CEN;

    
    
    
}

uint32_t getCCR1(){
    return TIM2->CCR1;
}

uint32_t getCNT(){
    TIM2->SR &= (~TIM_SR_CC1IF);
    return TIM2->CNT;
}


