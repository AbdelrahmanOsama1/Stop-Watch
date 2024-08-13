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

#define SEG_PORT  PORTA
#define ENABLE_PORT  PORTC
#define ENABLE_LEFT  7
#define ENABLE_RIGHT  6


#define  DEFAULT_NUM  10

void SEGMENT_DISPLAY_MULTIPLEXED(unsigned char n){
	unsigned char num_1;
	unsigned char num_2;

	const char arr[10] = {0x3f , 0x06 , 0x5B ,0x4F,0X66,0X6D,0X7D,0X07,0X7F,0X6F};     // common cathode 1st 7 bins
	num_1 = n%10;
	num_2 = n/10;
	
	SET_BIT(ENABLE_PORT,ENABLE_RIGHT);
	CLR_BIT(ENABLE_PORT,ENABLE_LEFT);
	
	SEG_PORT = arr[num_1] << 1;
	
	_delay_ms(10);
	
	SET_BIT(ENABLE_PORT,ENABLE_LEFT);
	CLR_BIT(ENABLE_PORT,ENABLE_RIGHT);
	
	SEG_PORT = arr[num_2] << 1;
	
	_delay_ms(10);
	
}


int main(void)
{
	
	DDRC=0XFF;//ENABLE OUT
	DDRA=0XFF;//7-SEG OUT
	
	DDRD=0X0;//IN
	PORTD=0XFF;//PULL UP RESIS. ACTIVE



	unsigned char num , adjusted_default , i  , button1_flag =0 , mode_flag = 1 , n1 ,n2 ;

	num = DEFAULT_NUM ;
	adjusted_default = DEFAULT_NUM ;
	
	while (1)
	{
		
		
		if(mode_flag == 1 ){		// normal mode
			
			SEGMENT_DISPLAY_MULTIPLEXED(num);
			
			if(GET_BIT(PIND,2)==0){  //button (resume/pause) pressed
				button1_flag = 1 ;
				while(GET_BIT(PIND,2)==0){ //busy wait
					SEGMENT_DISPLAY_MULTIPLEXED(num);
				}
				_delay_ms(20);   // avoid bouncing in hardware
			}
			
			
			
			
			if(button1_flag==1){   //button (resume/pause) pressed once
				
				for(i=0;i<50;i++){
					
					
					SEGMENT_DISPLAY_MULTIPLEXED(num);
					
					
					if(GET_BIT(PIND,3)==0){  //button (reset) pressed
						button1_flag = 0 ;
						num = adjusted_default;
						while(GET_BIT(PIND,3)==0){ //busy wait                        // if reset button pressed while counting , stop counting , go to reset setting
							SEGMENT_DISPLAY_MULTIPLEXED(num);
						}
						_delay_ms(20);   // avoid bouncing in hardware
						break;
					}
					
					
					
					
					if(GET_BIT(PIND,2)==0){  //button (resume/pause) pressed once again
						button1_flag = 0 ;
						while(GET_BIT(PIND,2)==0){ //busy wait                        // if (resume/pause) button pressed while counting , pause counting
							SEGMENT_DISPLAY_MULTIPLEXED(num);
						}
						_delay_ms(20);   // avoid bouncing in hardware
						break;
					}
					
					
				}
				
				if(button1_flag==1){
					num--;
				}
				
				
				
				if(num==0){
					SEGMENT_DISPLAY_MULTIPLEXED(num);
					button1_flag = 0;
					while(GET_BIT(PIND,3)==1){ //busy wait untill reset button is pressed (1 not 0)
						SEGMENT_DISPLAY_MULTIPLEXED(num);
					}
					num = adjusted_default;
					
					SEGMENT_DISPLAY_MULTIPLEXED(num);
					while(GET_BIT(PIND,3)==0){ //busy wait untill reset button is released (0 not 1)
						SEGMENT_DISPLAY_MULTIPLEXED(num);
					}
					_delay_ms(20);   // avoid bouncing in hardware
				}
				
				
				

			}
			
			
			
			
			else if (button1_flag == 0)    // while pausing
			{
				
				
				if(GET_BIT(PIND,3)==0){  //button (reset) pressed while pausing
					num = adjusted_default;
					SEGMENT_DISPLAY_MULTIPLEXED(num);
					while(GET_BIT(PIND,3)==0){ //busy wait                        // if reset button pressed while counting , stop counting , go to reset setting
						SEGMENT_DISPLAY_MULTIPLEXED(num);
					}
					_delay_ms(20);   // avoid bouncing in hardware
					
				}
				
				
				
				
				if(GET_BIT(PIND,4)==0){  //button (mode) pressed while pausing >> go to adjust mode
					mode_flag = 0;
					while(GET_BIT(PIND,4)==0){ //busy wait                        // if mode button pressed while pausing , go to adjust setting
						SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
					}
					_delay_ms(20);   // avoid bouncing in hardware
					
				}
				
				
			}
			

		}


		else if (mode_flag == 0)              // adjust mode
		{
			
			
			while(1){
				
				
				
				
				for(i=0;i<15;i++){   // divided delay while 7-seg on to check  mode and both (increment) buttons every 10ms [increment buttons = (resume/pause) & (reset) buttons at normal mode]
					
					SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
					
					
					if(GET_BIT(PIND,2)==0){  //button (resume/pause) pressed >> increment left digit by 1
						
						
						n1 = adjusted_default%10; //right digit
						n2 = adjusted_default/10; // left digit
						
						n2++;
						
						if(n2>9){
							n2=0;
						}
						
						
						adjusted_default = (n1) + (10*n2);
						
						
						while(GET_BIT(PIND,2)==0){ //busy wait                        // if (resume/pause) button pressed while pausing , increment left digit by 1
							SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
						}
						_delay_ms(200);   // avoid bouncing in hardware
						
						break;
					}
					
					
					
					
					if(GET_BIT(PIND,3)==0){  //button reset pressed >> increment right digit default by 1
						
						
						n1 = adjusted_default%10; //right digit
						n2 = adjusted_default/10; // left digit
						
						n1++;
						
						if(n1>9){
							n1=0;
						}
						
						
						adjusted_default = (n1) + (10*n2);
						
						while(GET_BIT(PIND,3)==0){ //busy wait                        // if (resume/pause) button pressed while pausing , increment right digit  by 1
							SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
						}
						_delay_ms(200);   // avoid bouncing in hardware
						break;
					}
					
					
					
					
					if(GET_BIT(PIND,4)==0){  //button mode pressed >> go to normal mode
						mode_flag = 1;
						while(GET_BIT(PIND,4)==0){ //busy wait                        // if (mode) button pressed while adjust mode , go to normal mode
							SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
						}
						_delay_ms(200);   // avoid bouncing in hardware
						break;
					}
					
					
					
					
					
				}
				
				if(mode_flag==1){
					num=adjusted_default;
					break;
				}
				
				
				
				
				PORTA = 0x0;        // BLINKING 7-SEG IN ADJUST MODE
				
				
				
				
				
				for(i=0;i<30;i++){   // divided delay while 7-seg off to check  both (increment) buttons every 10ms [increment buttons = (resume/pause) & (reset) buttons at normal mode]
					
					
					
					if(GET_BIT(PIND,2)==0){  //button (resume/pause) pressed >> increment left digit by 1
						
						n1 = adjusted_default%10; //right digit
						n2 = adjusted_default/10; // left digit
						
						n2++;
						
						if(n2>9){
							n2=0;
						}
						
						
						adjusted_default = (n1) + (10*n2);
						
						
						while(GET_BIT(PIND,0)==0){ //busy wait                        // if (resume/pause) button pressed while pausing , increment left digit by 1
							SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
						}
						_delay_ms(200);   // avoid bouncing in hardware
						break;
					}
					
					
					
					if(GET_BIT(PIND,3)==0){  //button reset pressed >> increment right digit default by 1
						
						n1 = adjusted_default%10; //right digit
						n2 = adjusted_default/10; // left digit
						
						n1++;
						
						if(n1>9){
							n1=0;
						}
						
						
						adjusted_default = (n1) + (10*n2);
						
						
						while(GET_BIT(PIND,3)==0){ //busy wait                        // if (resume/pause) button pressed while pausing , increment right digit by 1
							SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
						}
						_delay_ms(200);   // avoid bouncing in hardware
						break;
					}
					
					
					
					if(GET_BIT(PIND,4)==0){  //button mode pressed >> go to normal mode
						mode_flag = 1;
						while(GET_BIT(PIND,4)==0){ //busy wait                        // if (mode) button pressed while adjust mode , go to normal mode
							SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
						}
						_delay_ms(200);   // avoid bouncing in hardware
						break;
					}
					
					
					_delay_ms(10);
					
				}
				
				if(mode_flag==1){
					num=adjusted_default;
					SEGMENT_DISPLAY_MULTIPLEXED(adjusted_default);
					break;
				}
				
				
			}
			
		}
		



	}

	
}