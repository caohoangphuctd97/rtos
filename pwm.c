#include <16F877A.h>


//#FUSES NOWDT, XT, NOPROTECT,  NOBROWNOUT,PUT,NOLVP                      //No Watch Dog Timer


#use delay(crystal=20000000)
#define LCD_ENABLE_PIN PIN_D2
#define LCD_RS_PIN PIN_D0
#define LCD_RW_PIN PIN_D1
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7

#include <lcd.c>
#use rs232(baud=9600,xmit=pin_c6,rcv=pin_c7,bits =8) 

int setpoint;
float Error,pre_Error,P_part,Sum_err,delta_err,I_part,D_part; 
int data[6],dem,pwm1,pwm2,tf,zone,right_left;
unsigned int count;
float angle,out;  
float Kp=0.0;
float Ki=0.135;
float Kd=5.0;

#INT_RDA
void nhan_uart()
{
   tf = 1;
   data[dem] = getc();
   dem++;
   zone = data[5];
   if(dem==6) {
      dem=0;
      
   }
   
}
#INT_EXT     
void ISR() 
{ 
  right_left = right_left +1; // Chân RB1 =1;
  

} 
// Chuong trinh ngat

#use rtos(timer=0, minor_cycle=1ms)
//
// Declare TASK 1 - called every 250ms
//bam xung 2 dong co
#task(rate=1ms, max=1ms)
void task_B0()
{
if(zone==2) {
  
   count +=1;
   set_pwm1_duty(75);
   if (count == 6) {
   setpoint = 49;
   count = 5;
   zone = 3;
  
   }
 //  if (count > 3){
 //     count = 4;
 //     set_pwm1_duty(0);
 //     }
 //  if(count<3) {     
 //     set_pwm1_duty(75);}
 //  else {
 //  setpoint = 90;
 //  set_pwm1_duty(pwm1);
 //  }
}
else
{ 
  if(tf ==1)
    set_pwm1_duty(pwm1); 
  else     set_pwm1_duty(0); 
}
}
#task(rate=1ms, max=1ms)
void task_B1()
{
if(zone==2)// {
//   if (count > 3) set_pwm2_duty(0);
//   if(count<3) {
      
      set_pwm2_duty(0);//}
//   else {  
//   set_pwm2_duty(pwm2);
//   }
//}
else
{
   if(tf==1)
      set_pwm2_duty(pwm2);
   else     set_pwm2_duty(0); 
}
}

// thuat toan PID dong bo dong co
#task(rate=1ms, max=1ms)
void PID()
{
      
      angle = data[1]*10+data[2]+ data[3]*0.1+data[4]*0.01;
      if(data[0] == 45) angle = angle*-1 ;
      Error = setpoint - angle ;
      P_part = Kp*Error;
      Sum_err += Error;
      if(Sum_err>50) Sum_err=50;
      if(Sum_err<-50) Sum_err=-50;
      
      delta_err = Error - pre_Error;
      I_part = Ki*Sum_err;
      D_part = Kd*delta_err;
      out =  P_part + I_part + D_part ;
      pre_Error = Error;
      if(out>40) out=40;
      if(out<-40) out=-40;      
     pwm1 =67 + out;
     pwm2 =67 - out;
     if (pwm1<57) pwm1 = 57;
     if (pwm2<57) pwm2 = 57;
    
    
      printf(lcd_putc, "\fANGLE:%c%d%d.%d%d ",data[0],data[1],data[2],data[3],data[4]);
      
      lcd_gotoxy(1,2);
      printf(lcd_putc, "ZONE:%d", data[5]);
      if(right_left<10){
         lcd_gotoxy(10,2);
         printf(lcd_putc, "right");
         }
      else {
         
         lcd_gotoxy(10,2);
         printf(lcd_putc, "left");
      }
      delay_ms(100);
}

void main()
{
 setup_timer_2(T2_DIV_BY_16,255,1);      //819 us overflow, 819 us interrupt
 right_left = 0;
 setup_ccp1(CCP_PWM);
 setup_ccp2(CCP_PWM);
 set_tris_b(0x01);
 delay_us(10);

  ext_int_edge(L_TO_H);   
  enable_interrupts(INT_EXT);  //cho phép ng?t RB0
  
 enable_interrupts(INT_RDA);
 enable_interrupts(GLOBAL);
 setpoint=0;  
 count = 0;  
 lcd_init();
 lcd_putc("\fdientuspider.com");
 dem=0;  
 delay_ms(1000);
 tf = 0;
 rtos_run();

}


