#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
int time;
int lcd_key     = 0;
int adc_key_in  = 0;
int idModeRiego;
int idModeHum;  //TODO: cambiar a variable local?
int idMenu;
//teclado
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
//modos de riego
#define modeTime  0
#define modeHum   1
#define modeMix   2
//menus
#define idMenuPrinc 0
#define idMenuTime  1
#define idMenuHum   2
#define idMenuMix   3
#define idMenuHumAct  4

char* menus[] = {"Modo de riego: ","menuTime","Humedad:"};
char* modeRiego[] = {"<Tiempo        >","<Humedad       >","<Mixto         >"};
char* menuHum[] = {"<Actual       >","<Set          >"};

byte arrow[8] = {
  B00000,
  B01110,
  B01110,
  B01110,
  B11111,
  B01110,
  B00100,  
};

void setup()
{
  lcd.createChar(0, arrow);
  lcd.clear(); 
  lcd.begin(16, 2);
  lcd.setCursor(2,0); 
  lcd.print("BIENVENIDO!!");
  lcd.setCursor(0,1); 
  lcd.print("REGADOR ARDUINO");

  time = millis();
  delay(4000);
  lcd.clear();
  //analogWrite(10,50); setea intensidad del lcd
  idModeRiego = modeTime; //inicializo modo de riego 
  idMenu = idMenuPrinc; //inicializo menu
  idModeHum = 0;  //inicializo menu Hum. NO CAMBIAR
}

void loop()
{      
    lcd.setCursor(0,0);
    lcd.print(menus[idMenu]);//lcd.print(idModeRiego);

    switch(idMenu) {
      case idMenuPrinc:
        menuPrincipal();
        break;
      case idMenuTime:
        subMenuTime();
        break;
      case idMenuHum:
        subMenuHum();
    }
}

int read_LCD_buttons()  //FIXME
  { adc_key_in = analogRead(0);      // Leemos A0
    //Valores de medicion: rigth 0, down 254, up 99, left 407, select 638, none 1023.
   
    //delay para que no tome el valor repetidas veces en poco tiempo.
  
    //comparamos con un margen comodo
    if (adc_key_in > 900) return btnNONE;     // Ningun boton pulsado 
    if (adc_key_in < 50)  {delay(200);return btnRIGHT;}
    if (adc_key_in < 200) return btnUP;
    if (adc_key_in < 350) {delay(200);return btnDOWN;}
    if (adc_key_in < 500) {delay(200);return btnLEFT;}
    if (adc_key_in < 850) {delay(200);return btnSELECT;}
    
    return btnNONE;  // si todo falla
  }

void menuPrincipal() 
{
  lcd.setCursor(0,1);
  lcd.print(modeRiego[idModeRiego]);
  lcd_key = read_LCD_buttons();
  switch (lcd_key) {
    case btnRIGHT:
      if (idModeRiego <2)
      idModeRiego++;
    else
      idModeRiego = 0;
      //idModeRiego = modeMix;
      break;
    case btnLEFT:
      if (idModeRiego != 0)
        idModeRiego--;
      else
        idModeRiego = 2;
      break;
    case btnUP:
      lcd.print("UP    ");
      break;
    case btnDOWN:
      break;
    case btnSELECT:
      subMenuSelected();
      break;
    default:
      break;
  }  
}

void subMenuSelected() {

  switch (idModeRiego) {
    case modeTime:
      idMenu = idMenuTime;
      break;
    case modeHum:
      idMenu = idMenuHum;
      break;
    case modeMix:
      idMenu = idMenuMix;
      break;
  }
  lcd.clear(); //Limpio menú anterior
}

void subMenuTime() {
  
  lcd.setCursor(10,1);
  lcd.print("salir"); lcd.write(byte(0));

  //FIXME
  lcd_key = read_LCD_buttons();
  if (lcd_key == btnDOWN){
    idMenu = idMenuPrinc;
  } 
}

void subMenuHum() {
  lcd.setCursor(0,1);
  lcd.print(menuHum[idModeHum]); lcd.write(byte(0));

  //FIXME
  lcd_key = read_LCD_buttons();
  if (lcd_key == btnDOWN){
    idMenu = idMenuPrinc;
  } 

  if (lcd_key == btnRIGHT)  {
    if (idModeHum < 1)
      idModeHum++;
    else
      idModeHum = 0;
  } else if (lcd_key == btnLEFT)  {
    if (idModeHum !=0)
      idModeHum--;
    else
      idModeHum = 1;
  } else if (lcd_key == btnSELECT)  {
    idMenu = idMenuHumAct;  //TODO!!!    
  }
}

