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
#define butmove_stop 24 //
#define butup 26        //
#define butdown 28      //
#define butset_run 30   //
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
            GLCD.CursorTo(0, 7);
            GLCD.print("                     ");
            for (uint8_t a = 0; a < 8; a++)
            {

                GLCD.CursorTo(2, 7);
                GLCD.print("Please set time !");
                delay(300);
                GLCD.CursorTo(0, 7);
                GLCD.print("                     ");
                delay(300);
            }
        }
    }

    if (((gioset != 0) || (phutset != 0) || (giayset != 0)) && (Run == 1))
    {
        digitalWrite(rl, HIGH);
        GLCD.CursorTo(0, 7);
        GLCD.print("STOP");
        GLCD.CursorTo(17, 7);
        GLCD.print("   ");
    }
    else
    {
        digitalWrite(rl, LOW);
        GLCD.CursorTo(0, 7);
        GLCD.print("MOVE ");
        GLCD.CursorTo(17, 7);
        GLCD.print("SET");
        motor_status = OFF;
        Run = 0;
    }

    GLCD.CursorTo(1, 5);
    GLCD.print("time: ");

    GLCD.CursorTo(11, 5);
    print2so(gioset);
    GLCD.print(":");
    print2so(phutset);
    GLCD.print(":");
    print2so(giayset);

    // giaycu = tm.Second;

    if (Set==4 && digitalRead(butmove_stop) == LOW) // Nhan Stop
    {
        delay(10);
        while (digitalRead(butmove_stop) == LOW)
            ;
        Run = 0;
        motor_status = OFF;
    }

    if (Set == 4 && digitalRead(butset_run) == LOW) // Nhan Run
    {
        delay(10);
        while (digitalRead(butset_run) == LOW)
            ;
        Run = 1;
        motor_status = ON;
    }

    settime();
}

void settime()
{

    if (Run == 0 && digitalRead(butset_run) == LOW)
    {

        GLCD.CursorTo(0, 7);
        GLCD.print("    ");
        GLCD.CursorTo(6, 7);
        GLCD.print("UP");
        GLCD.CursorTo(11, 7);
        GLCD.print("DOWN");

        while (digitalRead(butset_run) == LOW)
        {
            Dich_Hang:
            Set = 1;
            /********** Blink gio *********/
            GLCD.CursorTo(11, 5);
            print2so(gioset);
            delay(300);
            GLCD.CursorTo(11, 5);
            GLCD.print("  ");
            delay(100);
        }
        // settime();
    }

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
                GLCD.CursorTo(11, 5);
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

                GLCD.CursorTo(11, 5);
                print2so(gioset);
                delay(250);

            } while (digitalRead(butdown) == LOW);
        }

        GLCD.CursorTo(11, 5);
        GLCD.print("  ");
        delay(100);

        GLCD.CursorTo(11, 5);
        print2so(gioset);
        delay(300);

        if (digitalRead(butmove_stop) == LOW) // change mode băng button MOVE
        {

            while (digitalRead(butmove_stop) == LOW)
            {
                Set = 2;
                GLCD.CursorTo(14, 5);
                print2so(phutset);
                delay(300);
                GLCD.CursorTo(14, 5);
                GLCD.print("  ");
                delay(100);
            }
        }

        if (digitalRead(butset_run) == LOW)
        {
            do
            {
                GLCD.CursorTo(0, 7);
                GLCD.print("RUN ");
                GLCD.CursorTo(6, 7);
                GLCD.print("  ");
                GLCD.CursorTo(11, 7);
                GLCD.print("    ");
                Set = 0;
                break;
            } while (digitalRead(butset_run) == LOW);
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
                GLCD.CursorTo(14, 5);
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
                GLCD.CursorTo(14, 5);
                print2so(phutset);
                delay(250);

            } while (digitalRead(butdown) == LOW);
        }

        GLCD.CursorTo(14, 5);
        GLCD.print("  ");
        delay(100);
        GLCD.CursorTo(14, 5);
        print2so(phutset);
        delay(300);

        if (digitalRead(butmove_stop) == LOW)
        {

            while (digitalRead(butmove_stop) == LOW)
            {
                Set = 3;
                GLCD.CursorTo(17, 5);
                print2so(giayset);
                delay(300);
                GLCD.CursorTo(17, 5);
                GLCD.print("  ");
                delay(100);
            }
        }

        if (digitalRead(butset_run) == LOW)
        {
            do
            {
                GLCD.CursorTo(0, 7);
                GLCD.print("RUN ");
                GLCD.CursorTo(6, 7);
                GLCD.print("  ");
                GLCD.CursorTo(11, 7);
                GLCD.print("    ");
                Set = 0;
                break;
            } while (digitalRead(butset_run) == LOW);
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
                GLCD.CursorTo(17, 5);
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
                GLCD.CursorTo(17, 5);
                delay(250);
                print2so(giayset);

            } while (digitalRead(butdown) == LOW);
        }
        GLCD.CursorTo(17, 5);
        GLCD.print("  ");
        delay(100);
        GLCD.CursorTo(17, 5);
        print2so(giayset);
        delay(300);

        if (digitalRead(butmove_stop) == LOW)
        {

            while (digitalRead(butmove_stop) == LOW)
            {
                Set = 0;
                goto Dich_Hang;
            }
        }

        if (digitalRead(butset_run) == LOW)
        {
            do
            {
                GLCD.CursorTo(0, 7);
                GLCD.print("RUN ");
                GLCD.CursorTo(6, 7);
                GLCD.print("  ");
                GLCD.CursorTo(11, 7);
                GLCD.print("    ");

                Set = 0;
                break;
            } while (digitalRead(butset_run) == LOW);
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
    pinMode(butmove_stop, INPUT_PULLUP);
    pinMode(butset_run, INPUT_PULLUP);
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
    // GLCD.DrawRect(GLCD.Left, GLCD.Top, GLCD.Width , GLCD.Height-55);
    //   delay(100);
    GLCD.DrawRect(GLCD.Left, GLCD.Top + 2, GLCD.Width, GLCD.Height - 30);
    delay(100);

    GLCD.DrawVLine(65, 2, GLCD.Height - 30);
    delay(100);
    GLCD.CursorTo(3, 0);
    GLCD.print("Liqid");
    GLCD.CursorTo(14, 0);
    GLCD.print("Air");
    GLCD.DrawHLine(0, 19, GLCD.Width - 63);
    GLCD.CursorTo(2, 2);
    GLCD.print("Status");
    GLCD.CursorTo(1, 3);
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
    GLCD.CursorTo(12, 1);
    GLCD.print("t1:");
    GLCD.print(t1, 1);
    delay(100);
    if (t1 == 0)
    {
        GLCD.CursorTo(18, 1);
        GLCD.print("  ");
    }
    GLCD.DrawRect(GLCD.Left + 115, GLCD.Top + 8, GLCD.Width - 125, GLCD.Height - 60); // ky hieu do C
    GLCD.CursorTo(20, 1);
    GLCD.print("C"); //

    GLCD.CursorTo(1, 1);
    GLCD.print("t2:");
    GLCD.print(t2, 1);
    if (t2 == 0)
    {
        GLCD.CursorTo(7, 1);
        GLCD.print("  ");
    }
    GLCD.DrawRect(GLCD.Left + 50, GLCD.Top + 8, GLCD.Width - 125, GLCD.Height - 60); // ky hieu do C
    GLCD.CursorTo(9, 1);
    GLCD.print("C");

    GLCD.CursorTo(12, 2);
    GLCD.print("h:");
    GLCD.CursorTo(15, 2);
    GLCD.print(int(h));
    GLCD.CursorTo(20, 2);
    GLCD.print("%");

    GLCD.CursorTo(12, 3);
    GLCD.print("P:");
    GLCD.print(p, 2);
    GLCD.print("bar");
    if (motor_status == ON)
    {
        GLCD.CursorTo(7, 3);
        GLCD.print("   ");
        GLCD.CursorTo(7, 3);
        GLCD.print("On");
    }
    else if (motor_status == OFF)
    {
        GLCD.CursorTo(7, 3);
        GLCD.print("   ");
        GLCD.CursorTo(7, 3);
        GLCD.print("Off");
    }

    interrupts(); // Cho phép ngắt

    thoigian();
}
