// OLED
#include <SPI.h>

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#define OLED_ADDR   0x3C
#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

volatile byte slaveget, slavesend;
volatile boolean process;

byte priority[4] ={0,1,2,3};
int state[4] = {1,1,1,1};
int load[4] = {7,6,5,4};
byte slaveindex ;
int inverter = 1 ;

int table[24] = {1234,1243,1324,1342,1423,1432,2134,2143,2314,2341,2413,2431,3124,3142,3214,3241,3412,3421,4123,4132,4213,4231,4312,4321};

#define PAGE_TIME 2000
#define PAGE_MID_TIME 400
#define ANIMATION_TIME 800

ISR (SPI_STC_vect)
{ 
    slaveget = SPDR; // read byte from SPI Data Register
    process = true;
}

void setup()
{
  Serial.begin (9600);
  
  pinMode(MISO, OUTPUT); // have to send on master in so it set as output
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  SPCR |= _BV(SPE); // turn on SPI in slave mode
  process = false;
  SPI.attachInterrupt(); // turn on interrupt

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.display();
  delay(1000);
}
void refreshData()
{
  if (process) {
    slaveindex = slaveget;
    process = false; 
  }
  
  if(slaveindex > 30) {
    inverter = slaveindex - 50;
  } else {
    findpri(slaveindex);
  }
  
  for(int z = 0 ; z < 4 ; z++) {
    state[z] = digitalRead(load[z]);
  }
}
void loop()
{
  refreshData();

  display.clearDisplay();
  display.drawRect(8, 12, 114, 36, WHITE);
  display.setFont(&FreeSans18pt7b);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(12, 42);
  display.print("H");
  display.display();
  delay(PAGE_MID_TIME);
  display.print("I");
  display.display();
  delay(PAGE_MID_TIME);
  display.print("S");
  display.display();
  delay(PAGE_MID_TIME);
  display.print("C");
  display.display();
  delay(PAGE_MID_TIME);
  display.print("S");
  display.display();
  delay(PAGE_TIME);
  display.setFont();

  refreshData();

  display.clearDisplay();
  display.drawLine(0, 10, 128, 10, WHITE);
  display.drawLine(0, 58, 128, 58, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(20, 20);
  display.print("Home & Inverter");
  display.setCursor(1, 40);
  display.print("Supply Control System");
  display.display();
  delay(PAGE_TIME);

  refreshData();

  display.clearDisplay();
  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, INVERSE);
    display.display();
    delay(1);
  }
  delay(ANIMATION_TIME);

  display.clearDisplay();
  set_outline();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(16, 12);
  display.print("INVERTER");
  display.display();
  delay(400);
  display.setTextSize(2);
  if(inverter == 1) {
    display.fillRect(44 ,30 , 40, 24, INVERSE);
    display.setTextColor(BLACK);
    display.setCursor(54,36);
    display.print("ON");
  } else if(inverter == 0) {
    display.drawRect(44 ,30 , 40, 24, WHITE);
    display.setTextColor(WHITE);
    display.setCursor(47,36);
    display.print("OFF");
  }
  display.display();
  delay(PAGE_TIME);

  refreshData();

  writeLogo();
  for(int16_t i = 0; i < display.height() / 2 - 8; i += 2) {
    display.fillRoundRect(i, i, 
      display.width()-2*i, 
      display.height()-2*i,
      display.height()/4, 
      INVERSE);
    display.display();
    delay(1);
  }
  delay(ANIMATION_TIME);

  display.clearDisplay();
  set_outline();
  display.drawLine(4, 16, 124, 16, WHITE);
  display.drawLine(4, 40, 124, 40, WHITE);
  display.drawLine(63, 16, 63, 60, WHITE);
  display.setCursor(4, 6);
  display.setTextSize(1);
  display.println("      PRIORITY");
  display.setTextSize(2);
  display.setCursor(30, 22);
  display.println(priority[0]);
  display.display();
  delay(PAGE_MID_TIME);
  display.setCursor(91, 22);
  display.println(priority[1]);
  display.display();
  delay(PAGE_MID_TIME);
  display.setCursor(30, 44);
  display.println(priority[2]);
  display.display();
  delay(PAGE_MID_TIME);
  display.setCursor(91, 44);
  display.println(priority[3]);
  display.display();
  delay(PAGE_MID_TIME);
  display.display();
  delay(PAGE_TIME);

  refreshData();

  writeLogo();
  for(int16_t i = 0; i < display.height() / 2 - 6; i += 3) {
    display.fillRect(i, i, 
      display.width() - i * 2, 
      display.height()- i * 2, 
      INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }
  delay(ANIMATION_TIME);

  display.clearDisplay();
  set_outline();
  display.drawLine(4, 16, 124, 16, WHITE);
  display.drawLine(4, 40, 124, 40, WHITE);
  display.drawLine(63, 16, 63, 60, WHITE);
  display.display();
  delay(PAGE_MID_TIME);
  
  display.setCursor(4, 6);
  display.setTextSize(1);
  display.println("     LOAD STATES");
  display.display();
  delay(PAGE_MID_TIME);
  drawLoadState(0, 28, 34, 34, 28);
  drawLoadState(1, 90, 33, 95, 27);
  drawLoadState(2, 28, 57, 34, 51);
  drawLoadState(3, 90, 57, 95, 51);
  delay(PAGE_TIME);

  refreshData();

  writeLogo();
  for(int16_t i = max(display.width(), display.height()) / 2; i > 30; i -= 3) {
    display.fillCircle(display.width() / 2, display.height() / 2, i, INVERSE);
    display.display();
    delay(1);
  }
  delay(ANIMATION_TIME);
}

void drawLoadState(int load_index, int c_x, int c_y, int x, int y)
{
  state[load_index] ? display.fillCircle(x, y, 10, INVERSE) : display.drawCircle(x, y, 10, WHITE);
  display.setTextColor(state[load_index] ? BLACK : WHITE);
  display.setFont(&FreeSans9pt7b);
  display.setTextSize(1);
  display.setCursor(c_x, c_y);
  display.println((char)(load_index + 'A'));
  display.display();
  display.setFont();
  delay(PAGE_MID_TIME);
}

void writeLogo()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(36,25);
  display.print("HISCS");
  display.display();
  delay(50);
}

void findpri(int index)
{
  int num = table[index] ;
  int k = 0 ;
  for(int count = 1000 ; count > 0 ; count = count/10)
  {
    priority[k++] = num/count ;
    num = num % count ;
  }
}

void set_outline()
{
  display.drawRect(1, 1, 126, 62, WHITE);
  display.display();
  delay(1);
}
