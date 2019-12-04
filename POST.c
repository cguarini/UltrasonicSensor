#include "POST.h"
#include <string.h>
#include <stdio.h>


char post[] = "Checking POST configuration\r\n";
char pfine[] = "POST works fine. Proceeding to the next steps\r\n\n";
 //int post_count =1;
 uint32_t postprev = 0;
 uint32_t  postcurr = 0;
 uint32_t timeint;
 char tbuff[300];
 uint32_t prevCNT = 0;
 
 
 
int POSTtest(void) {
    
  
    USART_Write(USART2, post, strlen(post));

		//Delay function so we don't spam the terminal
		while(getCNT() < (prevCNT + 5000000));
		prevCNT = getCNT();
    //check if the SR is set to 1 and the input pulse is captured

      postprev = 0;
        while(1)
            {
            if(TIM2->SR  &  TIM_SR_CC1IF){
              postcurr = getCCR1();
            
							if(postprev > 0)
							{
								timeint= postcurr - postprev;
								sprintf(tbuff, "The time interval is %d\r\n", timeint);
								putString(tbuff);
							}else{
                postprev = postcurr;
                continue;
              }
							
					
							
							if(timeint >= 100)
							{
									USART_Write(USART2, pfine, strlen(pfine));
									return 1;
							}
              else{
                return 0;
              }

    
            }
            else if(getCNT() > prevCNT + 200000){
              putString("POST timed out...\r\n");
              return 0;
            }
          }
}
            
    
        