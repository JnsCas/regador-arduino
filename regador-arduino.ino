#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int adc_key_in  = 0;
int idModeRiego;
int valueHumMinSet = 0;
int valueHumMaxSet = 0;
int modeSelected = -1;
int rangeHoursSet;
int timeRegarSet;
unsigned long time;
unsigned long timeRegando;
bool regando = false;
const int motor = 13;
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
char* menuHum[] = {"<Actual        >","<Set           >"};

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
  pinMode(motor,OUTPUT);  
  
}

void loop()
{      

  int lcd_key;

  regador();

  if (!regando) {
    //MENU PRINCIPAL
    lcd.setCursor(0,0);
    lcd.print("MODO DE RIEGO: ");
    lcd.setCursor(0,1);
    lcd.print(modeRiego[idModeRiego]);

    lcd_key = read_LCD_buttons();

    chooseValue(lcd_key, &idModeRiego, 0, 1, 0, 2);

    if(lcd_key == btnSELECT)  {
      subMenuSelected();
    }

  } else {
    lcd.setCursor(0,3);
    lcd.print("REGANDO..");
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

void regador() {

  //TODO: investigar uso de threads
  switch (modeSelected) {
      case modeTime:
        // regar periodicamente

        lcd.setCursor(1,0);
        lcd.print(millis());

        //1min - 60000ms
        //60min - 60000*60ms
        //if (time >= (rangeHoursSet*60000*60))  { //se cumple el tiempo esperado para regar.

        if (time >= rangeHoursSet)  {
          //prender bomba
          digitalWrite(motor, HIGH);
          timeRegando = millis();
          time = 0;
          delay(200);
          regando = true;
        }

        if (regando == true)  {
        lcd.setCursor(12,0);
        lcd.print(timeRegando);
        delay(200);
          if(timeRegando >= (timeRegarSet*60000)){  
            //termina de regar
            digitalWrite(motor, LOW);
            delay(1000);
            regando = false;
            time = millis();  //inicio el time. //TODO: si se va del modeTime poner en 0 esta variable.
            timeRegando = 0;
            delay(200);
          }
        }
        break;
      case modeHum:
        // regar por humedad. Mantiene en un rango de %10 de la humedad elegida.
        break;
      case modeMix:
        // mixto
        break;
  }
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

  int lcd_key = btnNONE;
  int rangeHours  = 1;

  while(lcd_key != btnDOWN) {

    lcd_key = read_LCD_buttons();

    lcd.setCursor(0,0);
    lcd.print("Regar cada: ");
    lcd.setCursor(15,0); lcd.write(byte(0));
    lcd.setCursor(6,1);
    lcd.print("<" + String(rangeHours)); 
    lcd.setCursor(10,1); lcd.print(">");
    lcd.setCursor(12,1); lcd.print("Hs"); 

    chooseValue(lcd_key, &rangeHours, 7, 1, 1, 24);

    if(lcd_key == btnSELECT)  {
      
      int timeRegar = 1;
      //rangeHoursSet = rangeHours;
      rangeHoursSet = 5000;
      lcd.clear();

      while (lcd_key != btnDOWN)  {
          
        lcd_key = read_LCD_buttons();

        lcd.setCursor(0,0);
        lcd.print("Tiempo a regar:");
        lcd.setCursor(15,0); lcd.write(byte(0));
        lcd.setCursor(6,1);
        lcd.print("<" + String(timeRegar));
        lcd.setCursor(10,1); lcd.print(">");
        lcd.setCursor(12,1); lcd.print("Min"); 

        chooseValue(lcd_key, &timeRegar, 7, 1, 1, 10);

        if(lcd_key == btnSELECT)  {

          modeSelected = modeTime;
          timeRegarSet  = timeRegar;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Time Set:");
          lcd.setCursor(0,1);
          lcd.print(String(rangeHoursSet) + "Hs. / " + String(timeRegar) + "min.");
          lcd_key = btnDOWN; //Vuelve al menu principal
          time = millis();  //inicio time
          delay(4000);
              
        }
      }
    }
  }
  lcd.clear();
}

void subMenuHum() {

  int lcd_key;
  int idModeHum = 0;

  do{
  
    lcd.setCursor(0,0);
    lcd.print("Humedad:");
    lcd.setCursor(15,0); lcd.write(byte(0));
    lcd.setCursor(0,1);
    lcd.print(menuHum[idModeHum]);

    lcd_key = read_LCD_buttons();

    chooseValue(lcd_key, &idModeHum, 0, 1, 0, 1);

    if (lcd_key == btnSELECT)  {
      subMenuHumSelected(idModeHum);
    }
      
  } while (lcd_key != btnDOWN);

  lcd.clear();
}

void subMenuHumSelected(int menuSelected)  {

  int lcd_key;
  lcd.clear();

  if (menuSelected == 0)  { //humedad actual

    do{

      lcd.setCursor(0,0);
      lcd.print("Humedad Actual");
      lcd.setCursor(15,0); lcd.write(byte(0));
      lcd.setCursor(5,1); 
      lcd.print("%90"); //hardcodeo ejemplo

      lcd_key = read_LCD_buttons();

    } while (lcd_key != btnDOWN);

  } else  { //setear humedad

    int valueHumMin  = 1;

    do{
      
      lcd.setCursor(0,0);
      lcd.print("Humedad Min.:");
      lcd.setCursor(15,0); lcd.write(byte(0));
      lcd.setCursor(6,1); 
      lcd.print("<%" + String(valueHumMin));
      lcd.setCursor(11,1); lcd.print(">");

      lcd_key = read_LCD_buttons();

      chooseValue(lcd_key, &valueHumMin, 8, 1, 1, 100);

      if (lcd_key == btnSELECT){

        valueHumMinSet = valueHumMin;
        int valueHumMax = 1;
        lcd.clear();

        do{              
          //elegir rango
          lcd.setCursor(0,0);
          lcd.print("Humedad Max.:");
          lcd.setCursor(15,0); lcd.write(byte(0));
          lcd.setCursor(6,1); 
          lcd.print("<%" + String(valueHumMax));
          lcd.setCursor(11,1); lcd.print(">");     
              
          lcd_key = read_LCD_buttons();

          chooseValue(lcd_key, &valueHumMax, 8, 1, 1, 100);
          
          if(lcd_key == btnSELECT)  {

            modeSelected = modeHum;
            valueHumMaxSet  = valueHumMax;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Humedad Set:");
            lcd.setCursor(5,1);
            lcd.print("%" + String(valueHumMinSet) + " - %" + String(valueHumMax));
            delay(4000);
            lcd_key = btnDOWN; //vuelve al menu principal
          }
        } while (lcd_key != btnDOWN);
      }
    } while (lcd_key != btnDOWN);

  }

  lcd.clear();  //limpio menu
}

//modifica un valor de 0 a 100 elegido con cierto rango. initPos posicion del primer digito en el lcd.
void chooseValue(int key_in ,int *value ,int initPos, int range,int rangeMin ,int rangeMax)  {

  int valueAnt  = *value;
  switch (key_in) {
      case btnRIGHT:
        if (*value < rangeMax)  {
          *value+= range;
        }
        else {
          *value = rangeMin;
        }
        break;
      case btnLEFT:
       if (*value != rangeMin)
         *value-= range;
       else
         *value = rangeMax;
        break;
  }
  //limpio caracteres
  int posClear = initPos;
  if (*value == 99) {
    posClear += 2;
    lcd.setCursor(posClear,1);
    lcd.print(" ");
  } else if (*value == 9){
    posClear += 1;
    lcd.setCursor(posClear,1);
    lcd.print(" ");
  } else if (valueAnt == rangeMax && valueAnt != 1 && *value == 1) {  //condiciones para poder usar la funcion para menus
    posClear += 1;
    lcd.setCursor(posClear,1);
    lcd.print("  ");
  }
}