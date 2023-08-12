// Nissan Leaf EV-CAN

#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define COUNTS_PER_CENTI_KWH 14400000  // 2 * 2 * 60 * 60 * 1000 for 10ms sample rate

#define LINE_SPACING 16
#define X0 0
#define Y0 0
#define Y1 (Y0 + LINE_SPACING)

#define SLEEP_COUNT_MAX 4000

#define TEST_POINT 3
#define TEST_POINT_INIT pinMode(TEST_POINT, OUTPUT)
#define TEST_POINT_HIGH digitalWrite(TEST_POINT, HIGH)
#define TEST_POINT_LOW digitalWrite(TEST_POINT, LOW)

#define KWH_FACTOR 775
#define frac(x) (int(10 * (x - int(x))))

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

int16_t volt = 0;
int16_t amp = 0;

int16_t kwhr = 0;
int16_t kwhr_centi = 0;
int16_t kwhr_frac = 0;
int32_t kwhr_count = 0;
char kwhr_sign = ' ';

int16_t kwhr_trip = 0;
int16_t kwhr_centi_trip = 0;
int16_t kwhr_frac_trip = 0;
int32_t kwhr_count_trip = 0;
char kwhr_sign_trip = ' ';

uint16_t soc;

int16_t kwh = 0;
int16_t kwh_centi = 0;
int16_t kwh_frac = 0;
char kwh_sign = ' ';

int16_t kwh_trip = 0;
int16_t kwh_centi_trip = 0;
int16_t kwh_frac_trip = 0;
char kwh_sign_trip = ' ';

int16_t kwh_centi_start = 0;

float miles = 0.0;
float mpkwh = 0.0;

bool screen_asleep_kwhr = true;
bool screen_asleep_kwh = true;
uint16_t screen_sleep_counter = 0;

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
  CAN0.init_Filt(0, 0, 0x01db0000);
  CAN0.init_Filt(1, 0, 0x05bc0000);
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
}

void loop()
{
  if (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    if (rxId == 0x1db)
    {
      amp = (rxBuf[0] << 3) | (rxBuf[1] >> 5);  // 0.5A/count 11 bits
      if (amp & 0x0400)
      {
        amp |= 0xf800;
      }
      amp = -amp;

      volt = (rxBuf[2] << 2) | (rxBuf[3] >> 6);  // 0.5V/count 10 bits

      if ((kwhr_count < 0) && (amp > 0))
      {
        kwhr_count = 0;
      } else {
        kwhr_count += (int32_t)volt * (int32_t)amp;
      }

      screen_sleep_counter = 0;

      if (screen_asleep_kwhr && (kwhr_count >= 0))
      {
        screen_asleep_kwhr = false;
        kwhr_count_trip = 0;
      }

      kwhr_count_trip += (int32_t)volt * (int32_t)amp;

      kwhr_centi = (int16_t)(kwhr_count / COUNTS_PER_CENTI_KWH);
      if (kwhr_count < 0)
      {
        kwhr_centi = -kwhr_centi;
        kwhr = kwhr_centi / 100;
        kwhr_frac = kwhr_centi % 100;
        kwhr_sign = '-';
      } else
      {
        kwhr = kwhr_centi / 100;
        kwhr_frac = kwhr_centi % 100;
        kwhr_sign = ' ';
      }

      kwhr_centi_trip = (int16_t)(kwhr_count_trip / COUNTS_PER_CENTI_KWH);
      if (kwhr_count_trip < 0)
      {
        kwhr_centi_trip = -kwhr_centi_trip;
        kwhr_trip = kwhr_centi_trip / 100;
        kwhr_frac_trip = kwhr_centi_trip % 100;
        kwhr_sign_trip = '-';
      } else {
        kwhr_trip = kwhr_centi_trip / 100;
        kwhr_frac_trip = kwhr_centi_trip % 100;
        kwhr_sign_trip = ' ';
      }
    }
  }
  if (rxId == 0x5bc)
  {
    soc = (rxBuf[0] << 2) | (rxBuf[1] >> 6);
    kwh_centi = (int16_t)(((int32_t)soc * KWH_FACTOR) / 100);

    if (screen_asleep_kwh)
    {
      kwh_centi_start = kwh_centi;
      screen_asleep_kwh = false;
    }
    kwh_centi_trip = kwh_centi_start - kwh_centi;

    if (kwh_centi < 0)
    {
      kwh_centi = -kwh_centi;
      kwh = kwh_centi / 100;
      kwh_frac = kwh_centi % 100;
      kwh_sign = '-';
    } else
    {
      kwh = kwh_centi / 100;
      kwh_frac = kwh_centi % 100;
      kwh_sign = ' ';
    }

    if (kwh_centi_trip < 0)
    {
      kwh_centi_trip = -kwh_centi_trip;
      kwh_trip = kwh_centi_trip / 100;
      kwh_frac_trip = kwh_centi_trip % 100;
      kwh_sign_trip = '-';
    } else
    {
      kwh_trip = kwh_centi_trip / 100;
      kwh_frac_trip = kwh_centi_trip % 100;
      kwh_sign_trip = ' ';
    }
  }

  TEST_POINT_HIGH;
  if ((i == 0) && (j == 0))
  {
    sprintf(&buffer[0], " %2d.%02d kWh", kwhr_trip, kwhr_frac_trip);
    sprintf(&buffer[10], " %2d.%02d", kwhr, kwhr_frac);
    sprintf(&buffer[16], " %2d.%02d kWh", kwh_trip, kwh_frac_trip);
    sprintf(&buffer[26], " %2d.%02d", kwh, kwh_frac);

    sprintf(&buffer[32], " %2d.%01d  miles       ", int(miles), frac(miles));
    sprintf(&buffer[48], " %2d.%01d  m/kWh       ", int(mpkwh), frac(mpkwh));
    buffer[0] = kwhr_sign_trip;
    buffer[10] = kwhr_sign;
    buffer[16] = kwh_sign_trip;
    buffer[26] = kwh_sign;
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

  screen_sleep_counter++;
  if (screen_sleep_counter == SLEEP_COUNT_MAX)
  {
    screen_asleep_kwhr = true;
    screen_asleep_kwh = true;
    screen_sleep_counter = 0;
  }
}