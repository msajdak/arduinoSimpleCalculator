/*
*  LCM1602 & Arduino Uno
*  VCC - > 5 V
*  GND - GND
*  SCL -> A5
*  SDA -> A4
*/
#define BUFFOR 10 
#include <Button.h>
#include <LedControl.h>
#include <EEPROM.h>

LedControl lc=LedControl(12,11,10,1); 
Button m = Button(2,BUTTON_PULLUP_INTERNAL);
Button s = Button(3,BUTTON_PULLUP_INTERNAL);
Button d = Button(4,BUTTON_PULLUP_INTERNAL);
Button g = Button(5,BUTTON_PULLUP_INTERNAL);

Button back = Button(6,BUTTON_PULLUP_INTERNAL);
Button clear = Button(7,BUTTON_PULLUP_INTERNAL);



int counter[BUFFOR][4];
int price[5]={180,250,380,200,1};//last one is intensity



void print(int l){

for(int i=0;i<4;i++){
 lc.setDigit(0, 4+i,l%BUFFOR,i==2);
  l=l/10;
}

}

void setup()  
{
   for (int k = 0; k < 5; k++)
  {
    int temp=0;
    
     EEPROM.get(k*4, temp );

     if (temp>0){
        price[k]=temp;
     }else{
        EEPROM.put(k*4, price[k] );
     }
  }
  
  memset(counter, 0, sizeof(counter[0][0]) * 4 * BUFFOR);
  lc.shutdown(0, false);
  lc.clearDisplay(0);

  lc.setIntensity(0, price[4]);
  
}


void animation(){
   lc.clearDisplay(0);
   for(int i=0;i<1;i++){
    for(int col=1;col<7;col++) {
      for(int row=0;row<8;row++) {
        lc.setLed(0,row,col,true);
    }
    delay(50);
     for(int row=0;row<8;row++) {
       lc.setLed(0,row,col,false);
    }        
  }
   }
}


void animation2(){
   lc.clearDisplay(0);

    for(int row=0;row<8;row++) {
    for(int col=1;col<7;col++) {
        lc.setLed(0,row,col,true);
    }
    delay(50);
    for(int col=1;col<7;col++) {
       lc.setLed(0,row,col,false);
    }        
  }
   
}

void setPrices(){
  for(int x=0;x<8;x++){
     lc.setChar(0,x,'-' ,false);
  }
  delay(2000);

  for (int i=0;i<5;i++){
    
    bool next=false;
    while(!next){
       if(m.uniquePress()){
            price[i]-=1;
        }else if(s.uniquePress()){
            price[i]+=1;
        }else if(d.uniquePress()){
           price[i]-=10;
        }else if(g.uniquePress()){
           price[i]+=10;
        }else if (clear.uniquePress()){
           next=true;
        }


        if (i==4){
            lc.setIntensity(0, price[4]);
        }
        print(price[i]);
    }
  }

   for (int k = 0; k < 5; k++)
  {
     EEPROM.put(k*4,  price[k]);
  }
  

  animation2();

}


void loop() {
 animation();
  int i=0;
  bool refresh=true;



if (clear.isPressed()){
      setPrices();
       i=0;
       memset(counter, 0, sizeof(counter[0][0]) * 4 * BUFFOR);
       refresh=true;
    }



long  toSleep=0;
int sleepAnimation=0;
 int currentPrice=0;
  while(1){
  
  
  
    int l=-1;
  
    if(m.uniquePress()){
       l=0;
    }else if(s.uniquePress()){
        l=1;
    }else if(d.uniquePress()){
       l=2;
    }else if(g.uniquePress()){
      l=3;
    }else if (back.uniquePress()){
       memset(counter[i], 0, sizeof(counter[0][0]) * 4);
       i--;
       if (i<0){
          i=BUFFOR-1;
       }
       refresh=true;
    }else if (clear.uniquePress()){
       i=0;
       memset(counter, 0, sizeof(counter[0][0]) * 4 * BUFFOR);
       refresh=true;
    } 

  
    if (l!=-1){
      int nextI=(i+1) % BUFFOR;

      memset(counter[nextI], 0, sizeof(counter[0][0]) * 4);

      counter[nextI][l]++;
      
      for(int x=0;x<4;x++){
        counter[nextI][x]+=counter[i][x];
      }
     

      refresh=true;
      
      i=nextI;
    
    }

    if (refresh){
      currentPrice=0;
      for(int x=0;x<4;x++){
        currentPrice+=counter[i][x]*price[x];
        if (counter[i][x]==0){
          lc.setChar(0,x,' ' ,false);
        }else  if (counter[i][x]<10){
          lc.setDigit(0, x,counter[i][x],false);
        }else{
         lc.setChar(0,x,'-' ,false);
        }
      }
      print(currentPrice);
      refresh=false;
      delay(100);//wait 200 for next one   

      toSleep=0;
     
      sleepAnimation=0;
    }



    if (toSleep>=120 *142800){//120= 120 s
      
      if (sleepAnimation==1){
         lc.clearDisplay(0);
         lc.setLed(0,7,0,true);
      }else if (sleepAnimation==10000){
          lc.clearDisplay(0);
      }else if (sleepAnimation==20000){
          sleepAnimation=0;
      }

      sleepAnimation++;
      
    }else {
     if (currentPrice==0){
         toSleep+=10;
      }else{
         toSleep+=2;
      }
     
      delayMicroseconds(10);
    }
        
  
  }//end while
  
  
}
