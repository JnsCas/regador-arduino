#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

int adc_key_in  = 0;
int idModeRiego;
int valueHumMinSet = 0;
int valueHumMaxSet = 0;
int modeSelected = -1;
//int idModeHum;  //TODO: cambiar a variable local?
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

  delay(4000);
  lcd.clear();
  analogWrite(10,80); //setea intensidad del lcd
  idModeRiego = modeTime; //inicializo modo de riego 
}

void loop()
{      
  int lcd_key;  //inicializo?

  //MENU PRINCIPAL
  lcd.setCursor(0,0);
  lcd.print("MODO DE RIEGO: ");//lcd.print(idModeRiego);
  lcd.setCursor(0,1);
  lcd.print(modeRiego[idModeRiego]);

  lcd_key = read_LCD_buttons();

  chooseValue(lcd_key, &idModeRiego, 1, 0, 2);

  if(lcd_key == btnSELECT)  {
    subMenuSelected();
  }
    
  //TODO: investigar uso de threads
  switch (modeSelected) {
      case modeTime:
        // regar periodicamente
        break;
      case modeHum:
        // regar por humedad. Mantiene en un rango de %10 de la humedad elegida.
        break;
      case modeMix:
        // mixto
        break;
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

void subMenuSelected() {

  lcd.clear();  //Limpio menú principal

  switch (idModeRiego) {
    case modeTime:
      subMenuTime();
      break;
    case modeHum:
      subMenuHum();
      break;
    case modeMix:
      //subMenuMix();
      break;
  }
}

void subMenuTime() {  

  int lcd_key;

  do{

    lcd_key = read_LCD_buttons();

    lcd.setCursor(0,0);
    lcd.print("menuTime");
    lcd.setCursor(10,1);
    lcd.print("salir"); lcd.write(byte(0));
      
  } while (lcd_key != btnDOWN); 
}

void subMenuHum() {

  int lcd_key;
  int idModeHum = 0;

  do{
  
    lcd.setCursor(0,0);
    lcd.print("Humedad:");
    lcd.setCursor(0,1);
    lcd.print(menuHum[idModeHum]); lcd.write(byte(0));

    lcd_key = read_LCD_buttons();

    chooseValue(lcd_key, &idModeHum, 1, 0, 1);

    if (lcd_key == btnSELECT)  {
      subMenuHumSelected(idModeHum);
    }
      
  } while (lcd_key != btnDOWN);
}

void subMenuHumSelected(int menuSelected)  {

  int lcd_key;
  lcd.clear();

  if (menuSelected == 0)  { //humedad actual

    do{

      lcd.setCursor(0,0);
      lcd.print("Humedad Actual");
      lcd.setCursor(5,1); 
      lcd.print("%90"); //hardcodeo ejemplo
      lcd.setCursor(15,1);lcd.write(byte(0));

      lcd_key = read_LCD_buttons();

    } while (lcd_key != btnDOWN);

  } else  { //setear humedad

    int valueHumMin  = 0;

    do{
      
      lcd.setCursor(0,0);
      lcd.print("Humedad Min.:");
      lcd.setCursor(6,1); 
      lcd.print("<%" + String(valueHumMin) + ">");
      lcd.setCursor(15,1);lcd.write(byte(0));      

      lcd_key = read_LCD_buttons();

      chooseValue(lcd_key, &valueHumMin, 1, 0, 100);

      if (lcd_key == btnSELECT){

        int valueHumMax = 0;
        lcd.clear();

        do{              
          //elegir rango
          lcd.setCursor(0,0);
          lcd.print("Humedad Max.:");
          lcd.setCursor(6,1); 
          lcd.print("<%" + String(valueHumMax) + ">");
          lcd.setCursor(15,1);lcd.write(byte(0));      
              
          lcd_key = read_LCD_buttons();

          chooseValue(lcd_key, &valueHumMax, 1, 0, 100);
        } while (lcd_key != btnDOWN);

        // if(lcd_key != btnDOWN){
        //   //seteo variables globales
        //   valueHumMinSet = valueHumMin;
        //   modeSelected  = modeHum;
        // }
      }
    } while (lcd_key != btnDOWN);
  }

  lcd.clear();  //limpio menu
}

//modifica un valor de 0 a 100 elegido con cierto rango.
void chooseValue(int key_in ,int *value ,int range,int rangeMin ,int rangeMax)  {
  if (key_in == btnRIGHT)  {
      if (*value < rangeMax)
        *value+= range;
      else
        *value = rangeMin;
        //limpio caracteres
        lcd.setCursor(10,1);
        lcd.print("  ");
  } else if (key_in == btnLEFT)  {
       if (*value != rangeMin)
         *value-= range;
       else
         *value = rangeMax;
      //limpio caracter
      if (*value == 9)  {
        lcd.setCursor(10,1);
        lcd.print(" ");
      }
      if (*value == 99) {
        lcd.setCursor(11,1);
        lcd.print(" ");
      }
    }
}