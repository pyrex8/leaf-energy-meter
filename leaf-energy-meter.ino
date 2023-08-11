// Nissan Leaf EV-CAN

#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define COUNTS_PER_DECI_AH 7200

#define LINE_SPACING 16
#define X0 0
#define Y0 0
#define Y1 (Y0 + LINE_SPACING)

#define SLEEP_COUNT_MAX 4000

#define TEST_POINT        3
#define TEST_POINT_INIT   pinMode(TEST_POINT, OUTPUT)
#define TEST_POINT_HIGH   digitalWrite(TEST_POINT, HIGH)
#define TEST_POINT_LOW    digitalWrite(TEST_POINT, LOW)

#define KWH_FACTOR 77.5F
#define frac(x) (int(10*(x - int(x))))

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

int16_t amp = 0;
int16_t ah = 0;
int16_t ah_deci = 0;
int16_t ah_frac = 0;
int32_t ah_count = 0;
char ah_sign = ' ';

int16_t ah_trip = 0;
int16_t ah_deci_trip = 0;
int16_t ah_frac_trip = 0;
int32_t ah_count_trip = 0;
char ah_sign_trip = ' ';

uint16_t soc;
float kwh = 0.0f;
float kwh_start = 0.0f;
float kwh_trip = 0.0f;
float miles = 0.0;
float mpkwh = 0.0;
char buf_int[4];
char buf_frac[4];

bool screen_asleep_ah = true;
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
      amp = (rxBuf[0] << 3) | (rxBuf[1] >> 5);
      if (amp & 0x0400)
      {
        amp |= 0xf800;
      }
      amp = -amp;
 
      if ((ah_count < 0) && (amp > 0))
      {
        ah_count = 0;
      }
      else
      {
        ah_count += (int32_t)amp;
      }

      screen_sleep_counter = 0;

      if (screen_asleep_ah && (ah_count >= 0))
      {
        screen_asleep_ah = false;
        ah_count_trip = 0;
      }
      
      ah_count_trip += (int32_t)amp;

      ah_deci = (int16_t)(ah_count / COUNTS_PER_DECI_AH); 
      if (ah_count < 0)
      {
        ah_deci = -ah_deci;
        ah = ah_deci / 100;
        ah_frac = ah_deci % 100;
        ah_sign = '-';
      }
      else
      {
        ah = ah_deci / 100;
        ah_frac = ah_deci % 100;
        ah_sign = ' ';
      }

      ah_deci_trip = (int16_t)(ah_count_trip / COUNTS_PER_DECI_AH); 
      if (ah_count_trip < 0)
      {
        ah_deci_trip = -ah_deci_trip;
        ah_trip = ah_deci_trip / 100;
        ah_frac_trip = ah_deci_trip % 100;
        ah_sign_trip = '-';
      }
      else
      {
        ah_trip = ah_deci_trip / 100;
        ah_frac_trip = ah_deci_trip % 100;
        ah_sign_trip = ' ';
      }
    }
  }
  if (rxId == 0x5bc)
  {
    soc = (rxBuf[0] << 2) | (rxBuf[1] >> 6);
    kwh = (((float)soc) * KWH_FACTOR) / 1000.0F;
    if (screen_asleep_kwh)
    {
      kwh_start = kwh;
      screen_asleep_kwh = false;
    }
    kwh_trip = kwh_start - kwh;
  }

  TEST_POINT_HIGH;
  if ((i == 0) && (j == 0))
  {
    sprintf(&buffer[0],  " %2d.%02d Ah ", ah_trip, ah_frac_trip);
    sprintf(&buffer[10], " %2d.%02d", ah, ah_frac);
    sprintf(&buffer[16], " %2d.%01d  kWh        ", int(kwh_trip), frac(kwh_trip));
    sprintf(&buffer[26], " %2d.%01d ", int(kwh), frac(kwh));
    sprintf(&buffer[32], " %2d.%01d  miles       ", int(miles), frac(miles));
    sprintf(&buffer[48], " %2d.%01d  m/kWh       ", int(mpkwh), frac(mpkwh));
    buffer[0] = ah_sign_trip;
    buffer[10] = ah_sign;
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
  buff_char[0] = buffer[i + j*16];
  ssd1306_printFixed(i*8, j*Y1, buff_char, STYLE_BOLD);
  TEST_POINT_LOW;

  screen_sleep_counter++;
  if (screen_sleep_counter == SLEEP_COUNT_MAX)
  {
    screen_asleep_ah = true;
    screen_asleep_kwh = true; 
    screen_sleep_counter = 0;
  }
}