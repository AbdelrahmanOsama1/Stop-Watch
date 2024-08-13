#define F_CPU 8000000
#include <util/delay.h>

#define SET_BIT(regis , bit)  (regis=regis|(1<<bit))
#define CLR_BIT(regis , bit)  (regis=regis&(~(1<<bit)))
#define GET_BIT(regis , bit)  (1&(regis>>bit))
#define TOG_BIT(regis , bit)  (regis=regis^(1<<bit))



#define PORTA   ( * ( volatile unsigned char *  ) 0x3B )
#define DDRA   ( * ( volatile unsigned char *  ) 0x3A )
#define PINA   ( * ( volatile unsigned char *  ) 0x39 )

#define PORTB   ( * ( volatile unsigned char *  ) 0x38 )
#define DDRB   ( * ( volatile unsigned char *  ) 0x37 )
#define PINB   ( * ( volatile unsigned char *  ) 0x36 )

#define PORTC   ( * ( volatile unsigned char *  ) 0x35 )
#define DDRC   ( * ( volatile unsigned char *  ) 0x34 )
#define PINC   ( * ( volatile unsigned char *  ) 0x33 )

#define PORTD   ( * ( volatile unsigned char *  ) 0x32 )
#define DDRD   ( * ( volatile unsigned char *  ) 0x31 )
#define PIND   ( * ( volatile unsigned char *  ) 0x30 )

#define SEG_PORT1  PORTC
#define SEG_PORT2  PORTD

#define  DEFAULT_NUM  10

void SEGMENT_DISPLAY(unsigned char n){
	unsigned char num_1;
	unsigned char num_2;

	const char arr[10] = {0x3f , 0x06 , 0x5B ,0x4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};     // common cathode 1st 7 bins
	num_1 = n%10;
	num_2 = n/10;
	

	
	SEG_PORT1 = arr[num_1];
	SEG_PORT2 = arr[num_2];
	
}

int main(void)
{
	
DDRC=0XFF;//OUT
DDRD=0XFF;//OUT
DDRA=0X0;//IN
PORTA=0XFF;//PULL UP RESIS. ACTIVE



unsigned char num , adjusted_default , i  , button1_flag =0 , mode_flag = 1 , n1 ,n2 ;


	num = DEFAULT_NUM ;
	adjusted_default = DEFAULT_NUM ;
	SEGMENT_DISPLAY(num);
		
	while (1)
	{
			
		
		
	if(mode_flag == 1 ){		// normal mode


		if(GET_BIT(PINA,0)==0){  //button (resume/pause) pressed
			button1_flag = 1 ; 
			while(GET_BIT(PINA,0)==0); //busy wait
		}
		
			
			
			
			if(button1_flag==1){   //button (resume/pause) pressed once
				
				SEGMENT_DISPLAY(num);
				num--;				
				
				
				
				for (i=0;i<50;i++)     // delay but divided to check button (reset) every 20msec in the 1 sec 
				{
					
					
					
					
					if(GET_BIT(PINA,1)==0){  //button (reset) pressed
						button1_flag = 0 ; 
						num = adjusted_default;
						SEGMENT_DISPLAY(num);
						while(GET_BIT(PINA,1)==0); //busy wait                        // if reset button pressed while counting , stop counting , go to reset setting
						
						break;
					}
					
					
					
					
					if(GET_BIT(PINA,0)==0){  //button (resume/pause) pressed once again
						button1_flag = 0 ;
						num++ ;  // to resume from the number i paused not the number before , because (num--) is done above after show num 
						while(GET_BIT(PINA,0)==0); //busy wait                        // if (resume/pause) button pressed while counting , pause counting
						break;
					}					
					
					
					_delay_ms(20);
					
				}				
				
				
				
				
				
				if(num==0){
					SEGMENT_DISPLAY(num);
					button1_flag = 0;
					while(GET_BIT(PINA,1)==1); //busy wait untill reset button is pressed (1 not 0)
					num = adjusted_default;
					SEGMENT_DISPLAY(num);
					while(GET_BIT(PINA,1)==0); //busy wait untill reset button is released (0 not 1)
				}
				
				
				

		  }
		  
		  
		  
		  
		  else if (button1_flag == 0)    // while pausing
		  {
			  
					
					if(GET_BIT(PINA,1)==0){  //button (reset) pressed while pausing
						num = adjusted_default;
						SEGMENT_DISPLAY(num);
						while(GET_BIT(PINA,1)==0); //busy wait                        // if reset button pressed while counting , stop counting , go to reset setting
		
					}
					
					
					
					
					if(GET_BIT(PINA,2)==0){  //button (mode) pressed while pausing >> go to adjust mode
						mode_flag = 0;
						while(GET_BIT(PINA,2)==0); //busy wait                        // if mode button pressed while pausing , go to adjust setting
						
					}		
									  
			  
		  }
		  

		}


        else if (mode_flag == 0)              // adjust mode
        {
			
			
			while(1){
				
				
			SEGMENT_DISPLAY(adjusted_default);
		
		
		
			for(i=0;i<30;i++){   // divided delay to check  mode and both (increment) buttons every 10ms [increment buttons = (resume/pause) & (reset) buttons at normal mode]
				
				
				
				
				if(GET_BIT(PINA,0)==0){  //button (resume/pause) pressed >> increment left digit by 1
					
					
					n1 = adjusted_default%10; //right digit
					n2 = adjusted_default/10; // left digit
					
					n2++;
					
					if(n2>9){
						n2=0;
					}
					
					
					adjusted_default = (n1) + (10*n2);
					
					
					while(GET_BIT(PINA,0)==0); //busy wait                        // if (resume/pause) button pressed while pausing , increment left digit by 1
					
					break;
				}
				
				
				
				
				if(GET_BIT(PINA,1)==0){  //button reset pressed >> increment right digit default by 1
					
					
					n1 = adjusted_default%10; //right digit
					n2 = adjusted_default/10; // left digit
					
					n1++;
					
					if(n1>9){
						n1=0;
					}
					
					
					adjusted_default = (n1) + (10*n2);
					
					while(GET_BIT(PINA,1)==0); //busy wait                        // if (resume/pause) button pressed while pausing , increment right digit  by 1
					
					break;
				}				
				
				
				
				
				if(GET_BIT(PINA,2)==0){  //button mode pressed >> go to normal mode
					mode_flag = 1;
					while(GET_BIT(PINA,2)==0); //busy wait                        // if (mode) button pressed while adjust mode , go to normal mode
					break;
				}
				
				
				
				
				_delay_ms(10);
				
			}
			
			if(mode_flag==1){
				num=adjusted_default;
				break;
			}
			
			
			PORTC = 0x0;
			PORTD = 0x0;
			
			
			
			for(i=0;i<30;i++){   // divided delay to check  both (increment) buttons every 10ms [increment buttons = (resume/pause) & (reset) buttons at normal mode]
				
				
				
				if(GET_BIT(PINA,0)==0){  //button (resume/pause) pressed >> increment left digit by 1
					
					n1 = adjusted_default%10; //right digit
					n2 = adjusted_default/10; // left digit
					
					n2++;
					
					if(n2>9){
						n2=0;
					}
					
					
					adjusted_default = (n1) + (10*n2);
					
					
					while(GET_BIT(PINA,0)==0); //busy wait                        // if (resume/pause) button pressed while pausing , increment left digit by 1
					break;
				}
				
				
				
				if(GET_BIT(PINA,1)==0){  //button reset pressed >> increment right digit default by 1
					
					n1 = adjusted_default%10; //right digit
					n2 = adjusted_default/10; // left digit
					
					n1++;
					
					if(n1>9){
						n1=0;
					}
					
					
					adjusted_default = (n1) + (10*n2);
					
					
					while(GET_BIT(PINA,1)==0); //busy wait                        // if (resume/pause) button pressed while pausing , increment right digit by 1
					
					break;
				}
				
				
				
				if(GET_BIT(PINA,2)==0){  //button mode pressed >> go to normal mode
					mode_flag = 1;
					while(GET_BIT(PINA,2)==0); //busy wait                        // if (mode) button pressed while adjust mode , go to normal mode
					break;
				}
				
				
				
				
				_delay_ms(10);
				
				
			}			
			
			if(mode_flag==1){
				num=adjusted_default;
				SEGMENT_DISPLAY(adjusted_default);
				break;
			}

			}
			
        }
         



	}

	
}