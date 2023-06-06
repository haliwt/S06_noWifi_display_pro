#include "run.h"
#include "smg.h"
#include "gpio.h"
#include "cmd_link.h"
#include "led.h"
#include "display.h"
#include "single_mode.h"

RUN_T run_t;

uint8_t power_on_off_flag;

void Power_Off(void);

static void Receive_Wifi_Cmd(uint8_t cmd);



/**********************************************************************************************************
**
*Function Name:static void notifyStatusToHost(uint8_t lightNum,uint8_t filterNum,uint8_t unionNum)
*Function : 
*Input Ref:lightNum--LED ,filterNum -filter number, unionNum - smart menu number
*Return Ref:NO
*
*********************************************************************************************************/
void Decode_Handler(void)
{
   if(run_t.decodeFlag ==1){
   run_t.decodeFlag =0;
   run_t.process_run_guarantee_flag =1;
   Receive_MainBoard_Data_Handler(run_t.wifi_orderByMainboard_label);

   }
  
}



/**********************************************************************
*
*Functin Name: void Display_DHT11_Value(void)
*Function : Timer of key be pressed handle
*Input Ref:  key of value
*Return Ref: NO
*
**********************************************************************/
void Power_Off(void)
{
    	 if(run_t.gPower_On ==POWER_OFF){

			Smg_AllOff();
            SMG_POWER_OFF()	;
			run_t.gPlasma=0;
			run_t.gDry=0;
			run_t.gUltrasonic =0;
			

				
		}
	
		
		
		
	      
}


/**********************************************************************
*
*Functin Name: void Receive_MainBoard_Data_Handler(run_t.single_data)
*Function : run order from main command 
*Input Ref:  run_t.single_dat is command 
*Return Ref: NO
*
**********************************************************************/
void Receive_MainBoard_Data_Handler(uint8_t cmd)
{
	static uint8_t m,n,p,q;
	static uint8_t hum1,hum2,temp1,temp2; 
	
    switch(cmd){


	 case WIFI_CMD:
	 	 
	    Receive_Wifi_Cmd(run_t.wifiCmd[0]);
	 
	 break;

	 case WIFI_TEMP: //set temperature value
	       if(run_t.gPower_On ==1){
		   	   run_t.set_temperature_flag=1;
			  
			   
           	run_t.gTimer_key_temp_timing=0;
        	 m= run_t.wifi_set_temperature /10 ;
			 n= run_t.wifi_set_temperature %10;
		   
	        TM1639_Write_2bit_SetUp_TempData(m,n,0);
			}
    
	 break;

	 case PANEL_DATA://0X02
	   
        if(run_t.gPower_On ==1){
        hum1 =  run_t.gReal_humtemp[0]/10 %10;  //Humidity 
        hum2 =  run_t.gReal_humtemp[0]%10;
        
        temp1 = run_t.gReal_humtemp[1]/10 %10;  // temperature
        temp2 = run_t.gReal_humtemp[1]%10;

         //temperature 
          TM1639_Write_2bit_TempData(temp1,temp2);
	      TM1639_Write_2bit_HumData(hum1,hum2);
	     HAL_Delay(2);
        }
	  

      break;

       case WIFI_BEIJING_TIME: 
         if(run_t.wifi_link_cloud_flag ==1 && run_t.gPower_On==1){
          
               
			 m=(run_t.dispTime_hours ) /10;
	        n = (run_t.dispTime_hours ) %10;;
			
            p= (run_t.dispTime_minutes )/10;
            q = (run_t.dispTime_minutes )%10;
			run_t.hours_two_bit = n;
            run_t.minutes_one_bit = p;
	       TM1639_Write_4Bit_Time(m,run_t.hours_two_bit,run_t.minutes_one_bit,q,0) ; // timer   mode  "H0: xx"
			
		 }
         
		    
       
 
      break;

      case WIFI_SET_TIMING:
        
       
            run_t.dispTime_minutes = 0;
             
            run_t.temp_set_timer_timing_flag= 1;
	   
			run_t.gTimer_key_timing=0;
		    
            
             m=(run_t.dispTime_hours ) /10;
	       

			 n= (run_t.dispTime_hours ) %10;;
			
	         run_t.hours_two_bit = n;
			 run_t.minutes_one_bit = 0;
			 
             TM1639_Write_4Bit_Time(m,run_t.hours_two_bit,run_t.minutes_one_bit,0,0) ; // timer   mode  "H0: xx"
            
	   

      break;

	}


}
/**********************************************************************
*
*Functin Name: void Receive_ManiBoard_Cmd(uint8_t cmd)
*Function :  wifi recieve data
*Input Ref:  receive wifi send order
*Return Ref: NO
*
**********************************************************************/
static void Receive_Wifi_Cmd(uint8_t cmd)
{
	switch(cmd){



          case WIFI_POWER_ON_NORMAL:

                run_t.wifi_power_on_flag = RUN_WIFI_NORMAL_POWER_ON;
				run_t.wifi_send_buzzer_sound = WIFI_POWER_ON_ITEM;
		        run_t.gRunCommand_label = RUN_POWER_ON;
				run_t.wifi_link_cloud_flag =WIFI_CLOUD_SUCCESS;
			break;

		   case WIFI_POWER_ON: //turn on 
		 	
				
				run_t.wifi_send_buzzer_sound = WIFI_POWER_ON_ITEM;
                run_t.wifi_power_on_flag = RUN_POWER_ON;
				run_t.gRunCommand_label = RUN_POWER_ON;
				run_t.wifi_link_cloud_flag =WIFI_CLOUD_SUCCESS;
				

	         break;

			 
			 

			 case WIFI_POWER_OFF: //turn off 
                
			   run_t.wifi_send_buzzer_sound = WIFI_POWER_OFF_ITEM;
			   run_t.gRunCommand_label = RUN_POWER_OFF;
			   run_t.power_on_recoder_times++;
			   run_t.wifi_link_cloud_flag =WIFI_CLOUD_SUCCESS;
				
            

			 break;

			 case WIFI_KILL_ON: //kill turn on plasma
			  if(run_t.gPower_On==1){
               	run_t.gPlasma = 1;
			        
             } 
			 break;

			 case WIFI_KILL_OFF: //kill turn off
                if(run_t.gPower_On==1){
			 	  run_t.gPlasma =0;
				  
		          
                }
			 break;

			 case WIFI_PTC_ON://dry turn on
                if(run_t.gPower_On==1){
			        run_t.gDry =1;
                    
                 
                }
			 break;

			 case WIFI_PTC_OFF: //dry turn off
               
			 	if(run_t.gPower_On==1){
					run_t.gDry=0;
                 
		           
			 	}

			 break;

			 case WIFI_SONIC_ON:  //drive bug
		
				 if(run_t.gPower_On==1){		   
				  run_t.gUltrasonic =1; //turn on 
			
				 
			    }

			 break;

			 case WIFI_SONIC_OFF: //drive bug turn off
			 	if(run_t.gPower_On==1){
				    run_t.gUltrasonic=0;
					
			   }
			 break;



	         default :
                  cmd =0;
			 break;

			 
        }
   
}

void Power_On_Fun(void)
{
                
   static uint8_t hour_decade,hour_unit,minutes_one,minutes_two;

   if(run_t.wifi_power_on_flag !=RUN_POWER_ON){
	
		run_t.gPlasma=1;
		run_t.gDry =1;
		run_t.gBug =1;
	   	run_t.gUltrasonic =1;
    }
    run_t.gPower_On=1;

    run_t.time_led_flag=1;
	
   
	run_t.wifi_send_buzzer_sound=0xff;

	
     Power_ON_Led();

	 if(run_t.timer_counter_to_zero ==1){

	       run_t.timer_counter_to_zero++;
		   run_t.dispTime_hours=0;
		   run_t.dispTime_minutes =0;
		   run_t.send_app_timer_total_minutes_data=0;
		   run_t.timer_timing_define_flag=timing_donot;
		   run_t.dispTime_hours = 0;
		   run_t.dispTime_minutes = 0;
		   run_t.send_app_wokes_total_minutes_data =0;
		   run_t.send_app_wokes_minutes_one=0;
		   run_t.send_app_wokes_minutes_two=0;
		   run_t.works_dispTime_hours=0;
		   run_t.works_dispTime_minutes=0;
		   SendData_Time_Data(0); 

		}
	    else if(run_t.timer_timing_define_flag==timing_success || run_t.temp_set_timer_timing_flag== 1){ //power on remeber last powr off of reference

	     run_t.dispTime_hours = run_t.define_initialization_timer_time_hours ;
         run_t.send_app_timer_total_minutes_data = run_t.define_initialization_timer_time_hours * 60;
		 run_t.gTimer_minute_Counter =0;
		 run_t.dispTime_minutes =0;
		 run_t.send_app_timer_minutes_one = run_t.send_app_timer_total_minutes_data >> 8;
		 run_t.send_app_timer_minutes_two = run_t.send_app_timer_total_minutes_data & 0x00ff;
		 SendData_Remaining_Time(run_t.send_app_timer_minutes_one, run_t.send_app_timer_minutes_two);
       }
       else{
	         run_t.dispTime_hours = 0;
			 run_t.dispTime_minutes = 0;
		     run_t.send_app_wokes_total_minutes_data =0;
			 run_t.send_app_wokes_minutes_one=0;
			 run_t.send_app_wokes_minutes_two=0;
			 run_t.works_dispTime_hours=0;
			 run_t.works_dispTime_minutes=0;
			 SendData_Works_Time(run_t.send_app_wokes_minutes_one, run_t.send_app_wokes_minutes_two);
			 HAL_Delay(200);
		}
         
        

	  hour_decade=(run_t.dispTime_hours ) /10;
	  hour_unit=(run_t.dispTime_hours ) %10;
    
	  minutes_one = run_t.dispTime_minutes /10;
      minutes_two = run_t.dispTime_minutes %10;
	  
	   SMG_POWER_ON(); //WT.EDIT 2023.03.02
       HAL_Delay(50);  

//      if(run_t.first_power_on_times == 1){
//          run_t.first_power_on_times++;
//          run_t.gReal_humtemp[0]=0;
//           run_t.gReal_humtemp[1]=0;
//              Display_DHT11_Value();
//      }
	  run_t.hours_two_bit = hour_unit;
	  run_t.minutes_one_bit =  minutes_one;
      
	 TM1639_Write_4Bit_Time(hour_decade,run_t.hours_two_bit,run_t.minutes_one_bit,minutes_two,0);
      Display_DHT11_Value();
    
}

/************************************************************************
	*
	*Function Name: static void Power_Off_Fun(void)
	*
	*
	*
	*
************************************************************************/
void Power_Off_Fun(void)
{
	
 
		run_t.gPlasma=0;
		run_t.gDry =0;
		run_t.gBug =0;
		
		run_t.wifi_led_fast_blink_flag=0;
		run_t.gTimer_set_temp_times=0; //conflict with send temperatur value 
        run_t.gWifi =0;
      
	
	    run_t.gPower_On=POWER_OFF;
		
		power_on_off_flag=1;
		
        Power_Off_Led_Off();

  
} 

