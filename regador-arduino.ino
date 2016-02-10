#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

int adc_key_in  = 0;
int idModeRiego;
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
      // lcd.print("<%" + lcd_key + ">");  TODO: convertir lcd_key a string.
      lcd.setCursor(15,1);lcd.write(byte(0));

      lcd_key = read_LCD_buttons();

    } while (lcd_key != btnDOWN);

  } else  { //setear humedad

    int valueHum  = 0;

    do{
      
      lcd.setCursor(0,0);
      lcd.print("Humedad Deseada:");
      lcd.setCursor(6,1); 
      lcd.print("<");lcd.print("%");lcd.print(valueHum);lcd.setCursor(11,1);lcd.print(">");
      lcd.setCursor(15,1);lcd.write(byte(0));      

      lcd_key = read_LCD_buttons();

      if (lcd_key == btnRIGHT)  {
        
        if (valueHum  < 100)  {

          valueHum++;

        } else {

          valueHum = 0;
          lcd.setCursor(9,1); lcd.print("  ");  //limpio caracter. FIXME

        }
      
      } else if(lcd_key == btnLEFT) {
      
        if (valueHum  != 0)  {

          valueHum--; 

          if(valueHum == 99)  {

            lcd.setCursor(10,1); lcd.print(" ");  //limpio caracter. FIXME

          } else if(valueHum == 9){

            lcd.setCursor(9,1); lcd.print(" ");  //limpio caracter . FIXME 
              
          }

        } else {

          valueHum = 100;

        }         

      }

    } while (lcd_key != btnDOWN);
  }

  lcd.clear();  //limpio menu

}
