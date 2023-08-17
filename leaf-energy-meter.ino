// Nissan Leaf CAR-CAN

#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define COUNTS_PER_CENTI_KWH 14400000  // 2 * 2 * 60 * 60 * 1000 for 10ms sample rate
#define SPEED_PER_MPH 316 // from driving test data
#define DISTANCE_PER_MILE 568800 //(SPEED_PER_MPH * 6 * 6 * 50) // 316 * 60 * 60 * 50 for 20ms sample (removed two zero for CENTI) 568800

#define LINE_SPACING 16
#define X0 0
#define Y0 0
#define Y1 (Y0 + LINE_SPACING)

#define TEST_POINT 3
#define TEST_POINT_INIT pinMode(TEST_POINT, OUTPUT)
#define TEST_POINT_HIGH digitalWrite(TEST_POINT, HIGH)
#define TEST_POINT_LOW digitalWrite(TEST_POINT, LOW)

#define KWH_FACTOR 775

long unsigned int rx_id;
unsigned char len = 0;
unsigned char rx_buf[8];

int16_t volt = 0;
int16_t amp = 0;

uint16_t gids = 0;

uint16_t kwh_centi = 0;
uint16_t kwh = 0;
uint16_t kwh_frac = 0;

uint16_t kwh_centi_trip = 0;
uint16_t kwh_trip = 0;
uint16_t kwh_frac_trip = 0;

uint16_t kwh_centi_start = 0;

uint16_t speed = 0;
uint32_t distance = 0;
uint16_t mph = 0;

uint16_t miles_centi = 0;
uint16_t miles = 0;
uint16_t miles_frac = 0;

uint16_t mpkwh_deci = 0;
uint16_t mpkwh = 0;
uint16_t mpkwh_frac = 0;

uint16_t time_500ms = 0;
uint16_t time_minutes = 0;

char buffer[100];
char buff_char[2] = "0";
uint8_t i = 0;
uint8_t j = 0;

void setup()
{
  TEST_POINT_INIT;
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();
  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x02840000);
  CAN0.init_Mask(1, 0, 0x07ff0000);
  CAN0.init_Filt(2, 0, 0x05bc0000);

  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
}

void loop()
{
  if (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rx_id, &len, rx_buf);

    if (rx_id == 0x284) // 20ms
    {
      speed = (rx_buf[0] << 8) | (rx_buf[1]);
      distance += speed; 
      mph = speed / SPEED_PER_MPH;
      miles_centi = distance / DISTANCE_PER_MILE;
      miles = miles_centi / 100;
      miles_frac = miles_centi % 100;

      if (kwh_centi_trip > 0)
      {
        mpkwh_deci = (uint16_t)((uint32_t)(miles_centi * 100) / kwh_centi_trip);
        mpkwh = mpkwh_deci / 100;
        mpkwh_frac = mpkwh_deci % 100;
      }
    }

    if (rx_id == 0x5bc) // 500ms
    {
      gids = (rx_buf[0] << 2) | (rx_buf[1] >> 6);
      kwh_centi = (int16_t)(((int32_t)gids * KWH_FACTOR) / 100);

      kwh = kwh_centi / 100;
      kwh_frac = kwh_centi % 100;

      if (kwh_centi_start == 0)
      {
        kwh_centi_start = kwh_centi;
      }

      if (kwh_centi_start > kwh_centi)
      {
        kwh_centi_trip = kwh_centi_start - kwh_centi;
        kwh_trip = kwh_centi_trip / 100;
        kwh_frac_trip = kwh_centi_trip % 100;
      }

      time_500ms++;
      time_minutes = time_500ms / 120;
    }
  }

  TEST_POINT_HIGH;
  if ((i == 0) && (j == 0))
  {
    sprintf(&buffer[0],  "%2d.%02d kWh        ", kwh, kwh_frac);
    sprintf(&buffer[16], "%2d.%02d kWh %2d min ", kwh_trip, kwh_frac_trip, time_minutes);
    sprintf(&buffer[32], "%2d.%02d miles      ", miles, miles_frac);
    sprintf(&buffer[48], "%2d.%02d miles/kwh  ", mpkwh, mpkwh_frac);
  }

  i++;
  if (i > 15)
  {
    i = 0;
    j++;
    if (j > 3)
    {
      j = 0;
    }
  }
  buff_char[0] = buffer[i + j * 16];
  ssd1306_printFixed(i * 8, j * Y1, buff_char, STYLE_BOLD);
  TEST_POINT_LOW;
}