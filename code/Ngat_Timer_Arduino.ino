#include <Adafruit_MAX31865.h>
#include <openGLCD.h>
#include "MCP320X.h"

#include <Wire.h>
#include <TimeLib.h>
#include <TimerOne.h>
#include <DS1307RTC.h>

#define CS_PIN 35
#define CLOCK_PIN 52
#define MOSI_PIN 51
#define MISO_PIN 50
#define MCP3204 4

#define RREF 430.0
#define RNOMINAL 100.0
#define rl 33
#define butrun 24  //
#define butup 26   //
#define butdown 28 //
#define butset 30  //
typedef enum
{
  OFF,
  ON
} Status;

int adc;
float ma;
float t0 = 0, pt1 = 0;
float t1 = 0, pt2 = 0;
float t2 = 0;
float h = 0;
float p = 0;
Status motor_status;

int ngay, thang, nam, gio, phut, giay;
uint8_t gioset = 0, phutset = 0, giayset = 0;
uint8_t Run = 0, Set = 0;
int giaycu;

float arr[10] = {22.0, -506.5, 50.8, -408.0, 99.7, -100.0, 30.2, -485.6, 20.5};
uint8_t i, j = 0;
Adafruit_MAX31865 pt100_1 = Adafruit_MAX31865(34, 51, 50, 52); // CS, DI, DO, CLK
Adafruit_MAX31865 pt100_2 = Adafruit_MAX31865(36, 51, 50, 52); // CS, DI, DO, CLK

MCP320X mcp3204(MCP3204, 52, 51, 50, 35); // (ic name,clk,mosi,miso,CS)

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000

// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000

void print2so(int so)
{
  if (so >= 0 && so < 10)
  {
    GLCD.print("0");
  }
  GLCD.print(so);
}

void thoigian()
{
  tmElements_t tm;
  RTC.read(tm);
  if ((tm.Second != giaycu) && (Run == 1))
  {
    giaycu = tm.Second;

    if ((gioset != 0) || (phutset != 0) || (giayset != 0))
    {
      motor_status = ON;
      giayset = giayset - 1;
      if (giayset > 59)
      {
        phutset = phutset - 1;
        giayset = 59;

        if ((phutset > 59))
        {
          gioset = gioset - 1;
          phutset = 59;

          if (gioset > 99)
          {
            gioset = 0;
          }
        }
      }
    }
    else
    {

      GLCD.CursorTo(0, 6);
      GLCD.print("                     ");
      for (uint8_t a = 0; a < 8; a++)
      {

        GLCD.CursorTo(2, 6);
        GLCD.print("Please set time !");
        delay(300);
        GLCD.CursorTo(0, 6);
        GLCD.print("                     ");
        delay(300);
      }
    }
  }

  if (((gioset != 0) || (phutset != 0) || (giayset != 0)) && (Run == 1))
  {
    digitalWrite(rl, HIGH);
    // GLCD.CursorTo(0, 7);
    // GLCD.print("STOP");
    // GLCD.CursorTo(17, 7);
    // GLCD.print("   ");
  }
  else
  {
    digitalWrite(rl, LOW);
    // GLCD.CursorTo(0, 7);
    // GLCD.print("RUN ");
    // GLCD.CursorTo(17, 7);
    // GLCD.print("SET");
    motor_status = OFF;
    Run = 0;
  }

  GLCD.CursorTo(1, 6);
  GLCD.print("Time: ");

  GLCD.CursorTo(11, 6);
  print2so(gioset);
  GLCD.print(":");
  print2so(phutset);
  GLCD.print(":");
  print2so(giayset);

  // giaycu = tm.Second;

  if (digitalRead(butrun) == LOW)
  {
    delay(100);
    while (digitalRead(butrun) == LOW)
      ;

    if (Run == 0)
    {
      Run = 1;
    }
    else
    {
      Run = 0;
      motor_status = OFF;
    }
  }

  if (Run == 0 && digitalRead(butset) == LOW)
  {
    // GLCD.CursorTo(0, 7);
    // GLCD.print("    ");
    // GLCD.CursorTo(6, 7);
    // GLCD.print("UP");
    // GLCD.CursorTo(11, 7);
    // GLCD.print("DOWN");

    while (digitalRead(butset) == LOW)
    {

      Set = 1;
      GLCD.CursorTo(11, 6);
      print2so(gioset);
      delay(300);
      GLCD.CursorTo(11, 6);
      GLCD.print("  ");
      delay(100);
    }
    settime();
  }
}

void settime()
{

  // set gio------------------------------------------------------------
  while (Set == 1)
  {
    if (digitalRead(butup) == LOW)
    {
      do
      {
        gioset++;
        while (gioset > 99)
        {
          gioset = 0;
          break;
        }
        GLCD.CursorTo(11, 6);
        print2so(gioset);
        delay(250);
      } while (digitalRead(butup) == LOW);
    }

    if (digitalRead(butdown) == LOW)
    {
      do
      {
        gioset--;
        while (gioset > 99)
        {
          gioset = 99;

          break;
        }

        GLCD.CursorTo(11, 6);
        print2so(gioset);
        delay(250);

      } while (digitalRead(butdown) == LOW);
    }

    GLCD.CursorTo(11, 6);
    GLCD.print("  ");
    delay(100);

    GLCD.CursorTo(11, 6);
    print2so(gioset);
    delay(300);

    if (digitalRead(butset) == LOW)
    {

      while (digitalRead(butset) == LOW)
      {
        Set = 2;
        GLCD.CursorTo(14, 6);
        print2so(phutset);
        delay(300);
        GLCD.CursorTo(14, 6);
        GLCD.print("  ");
        delay(100);
      }
    }
  }

  // set phut------------------------------------------------------------

  while (Set == 2)
  {
    if (digitalRead(butup) == LOW)
    {
      do
      {
        phutset++;
        while (phutset > 59)
        {
          phutset = 0;

          break;
        }
        GLCD.CursorTo(14, 6);
        print2so(phutset);
        delay(250);

      } while (digitalRead(butup) == LOW);
    }

    if (digitalRead(butdown) == LOW)
    {
      do
      {
        phutset--;
        while (phutset > 59)
        {
          phutset = 59;
          break;
        }
        GLCD.CursorTo(14, 6);
        print2so(phutset);
        delay(250);

      } while (digitalRead(butdown) == LOW);
    }

    GLCD.CursorTo(14, 6);
    GLCD.print("  ");
    delay(100);
    GLCD.CursorTo(14, 6);
    print2so(phutset);
    delay(300);

    if (digitalRead(butset) == LOW)
    {

      while (digitalRead(butset) == LOW)
      {
        Set = 3;
        GLCD.CursorTo(17, 6);
        print2so(giayset);
        delay(300);
        GLCD.CursorTo(17, 6);
        GLCD.print("  ");
        delay(100);
      }
    }
  }

  // set giay------------------------------------------------------------

  while (Set == 3)
  {
    if (digitalRead(butup) == LOW)
    {
      do
      {
        giayset++;
        while (giayset > 59)
        {
          giayset = 0;
          break;
        }
        GLCD.CursorTo(17, 6);
        print2so(giayset);
        delay(250);

      } while (digitalRead(butup) == LOW);
    }

    if (digitalRead(butdown) == LOW)
    {

      do
      {

        giayset--;
        while (giayset > 59)
        {
          giayset = 59;
          break;
        }
        GLCD.CursorTo(17, 6);
        delay(250);
        print2so(giayset);

      } while (digitalRead(butdown) == LOW);
    }
    GLCD.CursorTo(17, 6);
    GLCD.print("  ");
    delay(100);
    GLCD.CursorTo(17, 6);
    print2so(giayset);
    delay(300);

    if (digitalRead(butset) == LOW)
    {
      do
      {
        // GLCD.CursorTo(0, 7);
        // GLCD.print("RUN ");
        // GLCD.CursorTo(6, 7);
        // GLCD.print("  ");
        // GLCD.CursorTo(11, 7);
        // GLCD.print("    ");

        Set = 0;
      } while (digitalRead(butset) == LOW);
    }
  }
}

void setup()
{

  while (!Serial)
    ;
  Serial.begin(9600);
  GLCD.Init();
  GLCD.SelectFont(System5x7);

  pinMode(rl, OUTPUT);
  pinMode(butrun, INPUT_PULLUP);
  pinMode(butset, INPUT_PULLUP);
  pinMode(butup, INPUT_PULLUP);
  pinMode(butdown, INPUT_PULLUP);

  pt100_1.begin(MAX31865_3WIRE); // set to 2WIRE or 4WIRE as necessary
  pt100_2.begin(MAX31865_3WIRE);

  Timer1.initialize(100000);                 // Khởi động ngắt, thời gian đặt cho nó là 150000us=0.15s.
  Timer1.attachInterrupt(Read_value_sensor); // Khi xảy ra ngắt chương trình sẽ gọi hàm blinkLED().

  //    GLCD.CursorTo(4,2);
  //    GLCD.print(" INT WELCOME ");
  //    GLCD.CursorTo(3,4);
  //    GLCD.print("Copyright by INT");
  //    delay(5000);
  GLCD.ClearScreen();

  GLCD.DrawRect(GLCD.Left, GLCD.Top, GLCD.Width, GLCD.Height - 20);
  GLCD.DrawVLine(64, 0, GLCD.Height - 20);
  GLCD.CursorToXY(18, 2);
  GLCD.print("Liqid");
  GLCD.CursorToXY(85, 2);
  GLCD.print("Air");
  for (uint8_t i = 0; i < 3; i++)
  {
    GLCD.DrawHLine(64, 32 - (11 * i), GLCD.Width - 63); // colum truoc , row sau
    if (i >= 1)
      GLCD.DrawHLine(0, 32 - (11 * i), GLCD.Width - 63); // colum, row}
  }

  // GLCD.CursorTo(2, 4);
  // GLCD.print("Status");
  GLCD.CursorToXY(6, 28);
  GLCD.print("Motor:");
}

void Read_value_sensor()
{
  // pt2 = pt100_2.temperature(RNOMINAL, RREF) * 0.96515679443;
  // t1=pt2;
  // t2 = (mcp3204.readADC(0) * 0.02597977778 - 20) * 0.96864111498;
  h = mcp3204.readADC(1) * 0.02442002442;
  adc = mcp3204.readADC(2);
  p = adc * 0.0009765625;
  if (t2 < 0)
    t2 = 0;
  if (t1 < 0)
    t1 = 0;
  if (h < 0)
    h = 0;
  if (adc < 0)
    adc = 0;
  if (p < 0)
    p = 0;
  i++;
  if (i >= 80)
  {
    j++;
    t1 = arr[j];
    t2 = arr[j + 1];
    if (j >= 10)
      j = 0;
    i = 0;
  }
}

void loop()
{

  noInterrupts();
  GLCD.CursorToXY(69, 13);
  GLCD.print("t1:");
  if (t1 > 0)
  {
    GLCD.print(t1, 1);
  }
  while (t1 <= 0)
  {
    t1 = 0;
    GLCD.print(t1, 2);
    break;
  }

  GLCD.DrawRect(GLCD.Left + 114, GLCD.Top + 13, GLCD.Width - 125, GLCD.Height - 60); // ky hieu do C
  GLCD.CursorToXY(119, 13);
  GLCD.print("C"); //

  GLCD.CursorToXY(5, 13);
  GLCD.print("t2:");
  if (t2 > 0)
  {
    GLCD.print(t2, 1);
  }
  while (t2 <= 0)
  {
    t2 = 0;
    GLCD.print(t2, 2);
    break;
  }
  GLCD.DrawRect(GLCD.Left + 49, GLCD.Top + 13, GLCD.Width - 125, GLCD.Height - 60); // ky hieu do C
  GLCD.CursorToXY(54, 13);
  GLCD.print("C");

  GLCD.CursorToXY(69, 24);
  GLCD.print("h:");
  // GLCD.CursorTo(15, 30);
  GLCD.print(h, 1);
  // GLCD.CursorToXY(120, 22);
  GLCD.print(" %");

  // adc = mcp3204.readADC(2);
  // p = adc * 0.0009765625;
  GLCD.CursorToXY(69, 35);
  GLCD.print("P:");
  GLCD.print(p, 1);
  GLCD.CursorToXY(100, 35);
  GLCD.print(" bar");
  if (motor_status == ON)
  {
    // GLCD.CursorTo(7, 3);
    // GLCD.print("   ");
    GLCD.CursorToXY(45, 28);
    GLCD.print("ON ");
  }
  else if (motor_status == OFF)
  {
    // GLCD.CursorTo(7, 3);
    // GLCD.print("   ");
    GLCD.CursorToXY(45, 28);
    GLCD.print("OFF");
  }

  interrupts(); // Cho phép ngắt

  thoigian();
}
