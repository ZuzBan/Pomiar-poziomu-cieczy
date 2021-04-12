#include <Wire.h>                               //Biblioteka odpowiedzialna za komunikację przez I2C
#include <LiquidCrystal_I2C.h>                  //Biblioteka do obsługi LCD za pomocą I2C

LiquidCrystal_I2C lcd(0x27, 16, 2);             //Ustawienie adresu układu na 0x27

#define trigger 12                              //Dla łatwiejszego rozpoznania najważniejszych pinów zamiast podawania numerów zdefiniowane zostały ich nazwy
#define echo 11 

int roznica;
int wartosc_zadana;                    
int poziom_cieczy;

void setup() 
{
  pinMode(trigger, OUTPUT);                     //Poniżej określono tryby wykorzystanych pinów oraz ich stany
  pinMode(echo, INPUT);                         
  
  pinMode(9, OUTPUT); digitalWrite(9, LOW);
  pinMode(10, OUTPUT); digitalWrite(10, LOW);

  lcd.init();                                   //Incjalizacja LCD 16X2
  lcd.backlight();                              //Podświetlenie ekranu
  lcd.setCursor(0, 0);                          //Ustawienie kursora w pozycji (0, 0), czyli pierwszy wiersz i pierwsza kolumna
  lcd.print("Poziom cieczy:");                  //Wyświetlanie domyślnego napisu
  delay(500);                                   //Czas na rozruch systemu
}

int pomiar_poziomu_cieczy();                    //Deklaracja funkcji mierzącej poziom cieczy
void LEDy_zawory();                             //Deklaracja funkcji wykonawczej, odpowiedzialnej za LEDy pozorujace zawory

void loop()
{
  LEDy_zawory();                                //Wywołanie funkcji ustawiajacej odpowiednią jasność na LEDach, w zależności od tego jak bardzo miałby być uchylony dany zawór przy określonym poziomie cieczy
  delay(50);                                    //Małe opoznienie, by program mógł zareagować na działanie funkcji

  lcd.clear();                                  //Blok kodu odpowiadający za wyświetlanie na ekranie LCD zmierzonego poziomu cieczy w cm
  lcd.setCursor(0, 0);
  lcd.print("Poziom cieczy:");
  lcd.setCursor(0, 1);
  lcd.print(pomiar_poziomu_cieczy());
  lcd.setCursor(13, 1);
  lcd.print(" cm");
  delay(50);
  
}

int pomiar_poziomu_cieczy()                      //Funkcja mierząca odległość od tafli cieczy za pomocą czujnika ultradźwiękowego
{
  unsigned long czas;                            //Unsigned long to ta sama zmienna, co unsigned int, o duzym zakresie od 0 do 2 147 483 647, ale w starszych kompilatorach miala 32 bity, a unsigned int tylko 16
  int odleglosc;                                 //Zmienna int, aby nie przekroczyć rozdzielczości czujnika ultradźwiękowego
  int poziom_cieczy;
  int odleglosc_czujnik_dno = 22;                 //Tutaj należy podać głębokość zbiornika
  
  digitalWrite(trigger, LOW);                    //Wyzwalacz ustawiony w stan niski
  delayMicroseconds(2);                          //Krótka przerwa
  digitalWrite(trigger, HIGH);                   //Stan wysoki - wyzwolenie fali ultradzwiękowej do pomiaru
  delayMicroseconds(10);                         //Stan wysoki panuje 10 mikrosekund - taki minimalny czas pomiaru podaje producent
  digitalWrite(trigger, LOW);                    //Zakończenie pomiaru stanem niskim
  
  czas = pulseIn(echo, HIGH);                    //Za pomocą funkcji pulseIn(pin, stan) można w prosty sposób odczytać czas, przez który na pinie echo wystepował stan wysoki; czas ten jest proporcjonalny do zmierzonej odleglości
  odleglosc = czas/58;                           //Zmierzoną odległość można obliczyć dzieląc odczytany czas przez stałą 58; wynika ona ze wzoru s=v*t; 
                                                 //Czas należy pomnożyć razy prędkość dźwięku wyrażoną w m/ms (0.34), podzielić przez 2 (fala dociera do obiektu, odbija się i wraca) oraz przez 10, by otrzymać wynik w cm; (t*0.34)/(2*10)=t*0.017=t/58

  poziom_cieczy = odleglosc_czujnik_dno - odleglosc; //W ten sposób ustalono odległość poziomu cieczy od dna                                             
                                                  
  return poziom_cieczy;                          //Funkcja zwraca poziom cieczy
}


void LEDy_zawory()                              
{                             
 wartosc_zadana = 10;                             //Zadany poziom cieczy, który ma być utrzymywany w zbiorniku, wyrazony w [cm]
 poziom_cieczy = pomiar_poziomu_cieczy();         //Zapis wartości funkcji pomiarowej do zmiennej
 roznica = wartosc_zadana - poziom_cieczy;        //Obliczanie różnicy między zadaną wartością poziomu cieczy a aktualnym poziomem cieczy

 if (roznica > 0.00)
 {
  int wypelnienie = roznica/wartosc_zadana*255;   //Określono jaki ułamek wartości zadanej stanowi obliczona różnica i pomnożono tę wartość razy maksymalne wypełnienie, czyli 255
  analogWrite(9, wypelnienie);                    //Dioda czerwona o jasności równej wartości zmiennej wypelnienie
  digitalWrite(10, LOW);                          //Zgaszenie diody niebieskiej 
 }
 if (roznica < 0.00)
 {
  int wypelnienie = abs(roznica)/wartosc_zadana*255;  //W przypadku ujemnej różnicy, przy określaniu wypełnienia obliczono jej wartość bezwzględną
  analogWrite(10, wypelnienie);                       //Dioda niebieska o jasności równej wartości zmiennej wypelnienie
  digitalWrite(9, LOW);                               //Zgaszenie diody czerwonej
 }

 if (roznica == 0.00)                                 //Gdy różnica jest równa 0, obie diody zostają zgaszone
  {
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
 } 
 
}
