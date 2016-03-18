#include <LiquidCrystal.h>
#include <StopWatch.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int adc_key_in  = 0;
int idModeRiego;
int valueHumMinSet = 0;
int valueHumMaxSet = 0;
int modeSelected = -1;
int rangeHoursSet;
int timeRegarSet;
StopWatch time(StopWatch::SECONDS);
StopWatch timeRegando(StopWatch::SECONDS);
//StopWatch time_screensaver(StopWatch::SECONDS); //TODO
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
#define modeManual   2

char* modeRiego[] = {"<Tiempo        >","<Humedad       >","<Manual        >"};
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

byte point[8] = {
  B00000,
  B00000,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000
};

void setup()
{
  lcd.createChar(0, arrow);
  lcd.createChar(1, point);
  lcd.clear(); 
  lcd.begin(16, 2);
  lcd.setCursor(2,0); 
  lcd.print("BIENVENIDO!!");
  lcd.setCursor(0,1); 
  lcd.print("REGADOR ARDUINO");

  delay(4000);
  analogWrite(10,80); //setea intensidad del lcd
  idModeRiego = modeTime; //inicializo modo de riego 
  pinMode(motor,OUTPUT);
  lcd.clear();
  
}

void loop()
{      

  int lcd_key;

  regador();

  //MENU PRINCIPAL
  lcd.setCursor(0,0);
  lcd.print("MODO DE RIEGO: ");
  printModeSet();
  lcd.setCursor(0,1);
  lcd.print(modeRiego[idModeRiego]);

  lcd_key = read_LCD_buttons();

  chooseValue(lcd_key, &idModeRiego, 0, 1, 0, 2);

  if(lcd_key == btnSELECT)  {
    subMenuSelected();
  }



}

void printModeSet() {
  if (modeSelected == modeTime) {
    lcd.print("T");
  } else if (modeSelected == modeHum) {
    lcd.print("H");
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
        if (time.elapsed() >= rangeHoursSet*60*60)  { //se cumple el tiempo esperado para regar.
          //prender bomba
          digitalWrite(motor, HIGH);
          time.stop();
          regando = true;
          lcd.clear();
          timeRegando.start();
        }

        while (regando == true)  {

          if(timeRegando.elapsed() >= (timeRegarSet)){  
            //termina de regar
            lcd.setCursor(5,1);
//            lcd.print(String(timeRegando.elapsed()) + "/" + String(timeRegarSet) + "s") ;
            digitalWrite(motor, LOW);
            delay(1000);
            regando = false;
            time.reset();  
            time.start();   //inicio el time.
            timeRegando.reset();
          }

          lcd.setCursor(4,0);
          lcd.print("REGANDO..");
          lcd.setCursor(5,1);
          lcd.print(String(timeRegando.elapsed()) + "/" + String(timeRegarSet) + "s") ;
          delay(10);
        }

        break;
      case modeHum:
        // regar por humedad. Mantiene en un rango de %10 de la humedad elegida.
        int sensorValue = getSensorValue();
        if (sensorValue < valueHumMinSet){
          //Regar hasta llegar al promedio rangos humedad definidos
          int promHum = (valueHumMinSet + valueHumMaxSet) / 2;
          //bool flagTolerance = true;  //le doy un margen de tolerancia al riego.
          while (sensorValue < promHum){
            digitalWrite(motor, HIGH);
            sensorValue = getSensorValue();

            lcd.setCursor(4,0);
            lcd.print("REGANDO..");
            lcd.setCursor(5,1);
            //FIXME: BORRAR solo test
            lcd.print(String(sensorValue) + "/" + String(promHum)) ;
            delay(200);   
          }
          digitalWrite(motor, LOW);
          //flagTolerance = false;
        }
        break;
  }
}

int getSensorValue()  {
  //en caso de agregar mas sensores, pasar por parametro.
  return analogRead(A1) / 10;
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
    case modeManual:
      //subMenuMix();
      subMenuManual();
      break;
  }
}

void subMenuManual()  {
  int lcd_key = btnNONE;
  StopWatch time_manual(StopWatch::SECONDS);

  while (lcd_key != btnDOWN)  {

    lcd_key = read_LCD_buttons();

    digitalWrite(motor, LOW);

    lcd.setCursor(4,0);
    lcd.print("Manual: ");
    lcd.setCursor(2,1);
    lcd.print(" ");
    lcd.print("On");
    lcd.setCursor(9,1);lcd.write(byte(1));
    lcd.print("Off");

    if (lcd_key == btnSELECT) {
      lcd.setCursor(13,0);
      lcd.print("   ");
    }

    while(lcd_key == btnSELECT){
        lcd_key = read_LCD_buttons();

        digitalWrite(motor, HIGH);

        lcd.setCursor(13,0);
        time_manual.start();
        lcd.print(time_manual.elapsed());
        lcd.setCursor(9,1); 
        lcd.print(" ");
        lcd.setCursor(2,1);
        lcd.write(byte(1)); 
    }
    time_manual.reset();

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
      
      rangeHoursSet = rangeHours;

      int timeRegar = 1;
      lcd.clear();

      while (lcd_key != btnDOWN)  {
          
        lcd_key = read_LCD_buttons();

        lcd.setCursor(0,0);
        lcd.print("Tiempo a regar:");
        lcd.setCursor(15,0); lcd.write(byte(0));
        lcd.setCursor(6,1);
        lcd.print("<" + String(timeRegar));
        lcd.setCursor(10,1); lcd.print(">");
        lcd.setCursor(12,1); lcd.print("seg"); 

        // chooseValue(lcd_key, &timeRegar, 7, 1, 1, 10);  //por min
        chooseValue(lcd_key, &timeRegar, 7, 1, 1, 10*60); //por seg

        if(lcd_key == btnSELECT)  {

          modeSelected = modeTime;
          timeRegarSet  = timeRegar;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Time Set:");
          lcd.setCursor(0,1);
          lcd.print(String(rangeHoursSet) + "Hs. / " + String(timeRegar) + "seg.");
          lcd_key = btnDOWN; //Vuelve al menu principal
          time.start();  //inicio time
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

    int sensorValue = analogRead(A1);
    lcd.setCursor(0,0);
    lcd.print("Humedad Actual");
    lcd.setCursor(6,1); 
    lcd.print("%" + String(sensorValue/10));

    delay(4000);

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
        int valueHumMax = valueHumMin;
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

          chooseValue(lcd_key, &valueHumMax, 8, 10, 1, 100);
          
          if(lcd_key == btnSELECT)  {

            lcd.clear();
            lcd.setCursor(0,0);

            if (valueHumMin >= valueHumMax) {
              
              lcd.print("values bad");
              lcd.setCursor(0,1);
              lcd.print("try again");
            
            } else {

              modeSelected = modeHum;
              valueHumMaxSet  = valueHumMax;
              lcd.print("Humedad Set:");
              lcd.setCursor(5,1);
              lcd.print("%" + String(valueHumMinSet) + " - %" + String(valueHumMax));
            }

            delay(4000);
            lcd_key = btnDOWN; //vuelve al menu principal

          }
        } while (lcd_key != btnDOWN);
      }
    } while (lcd_key != btnDOWN);

  }

  lcd.clear();  //limpio menu
}

//modifica un valor de X a XXX elegido con cierto rango. initPos posicion del primer digito en el lcd.
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