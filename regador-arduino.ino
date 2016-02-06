#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);


int time;
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


void setup()
{
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
}

void loop()
{      
    lcd.setCursor(0,0);
    lcd.write("Seleccione: ");
    lcd.setCursor(0,1);
    lcd.write("Por tiempo");

    lcd_key = read_LCD_buttons();

    lcd_key = read_LCD_buttons();
    if( lcd_key == btnRIGHT)
            lcd.print("RIGHT ");
    else if ( lcd_key == btnLEFT )
            lcd.print("LEFT   ");
    else if ( lcd_key == btnUP)
            lcd.print("UP    ");
    else if ( lcd_key == btnDOWN) {
            lcd.print("DOWN  ");
    }
    else if ( lcd_key == btnSELECT)
            lcd.print("SELECT");
    else if ( lcd_key == btnNONE)
            lcd.print("NONE  ");
}

int read_LCD_buttons()  
  { adc_key_in = analogRead(0);      // Leemos A0
    //rigth 0, down 254, up 99, left 407, select 638, none 1023
    //comparamos con un margen comodo
    if (adc_key_in > 900) return btnNONE;     // Ningun boton pulsado 
    if (adc_key_in < 50)   return btnRIGHT; 
    if (adc_key_in < 200)  return btnUP;
    if (adc_key_in < 350)  return btnDOWN;
    if (adc_key_in < 500)  return btnLEFT;
    if (adc_key_in < 850)  return btnSELECT; 

    return btnNONE;  // si todo falla
  }

