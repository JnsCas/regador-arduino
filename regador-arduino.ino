#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);


int time;
int lcd_key     = 0;
int adc_key_in  = 0;
int idModeRiego;
int idMenu;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define modeTime  0
#define modeHum   1
#define modeMix   2
#define menuPrinc 0
#define menuTime  1

char* menus[] = {"Seleccione: ","LLEGASTE!"};
char* modeRiego[] = {"<Tiempo        >","<Humedad       >","<Mixto         >"};

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
  analogWrite(10,50);
  idModeRiego = modeTime; //inicializo modo de riego 
  idMenu = menuPrinc; //inicializo menu
}

void loop()
{      
    lcd.setCursor(0,0);
    lcd.print(menus[idMenu]);//lcd.print(idModeRiego);

    if (idMenu == menuPrinc) {
      menuPrincipal();
    } else if (idMenu == menuTime) {
      subMenuTime();
    }
    

}

int read_LCD_buttons()  
  { adc_key_in = analogRead(0);      // Leemos A0
    //Valores de medicion: rigth 0, down 254, up 99, left 407, select 638, none 1023.
    //delay para que no tome el valor repetidas veces en poco tiempo.
    //delay(200); 
    //comparamos con un margen comodo
    if (adc_key_in > 900) return btnNONE;     // Ningun boton pulsado 
    if (adc_key_in < 50)  {delay(200);return btnRIGHT;}
    if (adc_key_in < 200) return btnUP;
    if (adc_key_in < 350) return btnDOWN;
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
      subMenu(idModeRiego);
      break;
    default:
      break;
  }  
}

void subMenu(int idRiego) {

  switch (idRiego) {
    case modeTime:
      idMenu = menuTime;
      break;
    case modeHum:
      break;
    case modeMix:
      break;
  }
}

void subMenuTime() {
  
//  bool salir = false;
//  while (!salir) {
  lcd.clear();
  lcd.print("VOLVISTE!!");
  delay(4000);
    
}


