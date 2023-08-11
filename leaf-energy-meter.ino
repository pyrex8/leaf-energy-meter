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

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

int16_t amp = 0;
int16_t ah = 0;
int16_t ah_frac = 0;
int32_t ah_count = 0;
int32_t ah_count_trip = 0;
int16_t ah_deci = 0;

bool screen_asleep = true;
uint16_t screen_sleep_counter = 0;

char buffer[17];
char buff_char[2] = "0";
uint8_t i = 0;

void setup()
{
  TEST_POINT_INIT;
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();    
  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x01db0000);
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);
  sprintf(buffer, "-%2d.%02d Ah ", ah, ah_frac);
  sprintf(&buffer[10], "-%2d.%02d", ah, ah_frac);
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

      if (screen_asleep && (ah_count >= 0))
      {
        screen_asleep = false;
        ah_count_trip = 0;
      }
      
      ah_count_trip += (int32_t)amp;

      ah_deci = (int16_t)(ah_count / COUNTS_PER_DECI_AH); 
      if (ah_count < 0)
      {
        ah_deci = -ah_deci;
        ah = ah_deci / 100;
        ah_frac = ah_deci % 100;
        sprintf(buffer, "-%2d.%02d Ah", ah, ah_frac);
      }
      else
      {
        ah = ah_deci / 100;
        ah_frac = ah_deci % 100;
        sprintf(buffer, " %2d.%02d Ah ", ah, ah_frac);
      }

      ah_deci = (int16_t)(ah_count_trip / COUNTS_PER_DECI_AH); 
      if (ah_count_trip < 0)
      {
        ah_deci = -ah_deci;
        ah = ah_deci / 100;
        ah_frac = ah_deci % 100;
        sprintf(&buffer[10], "-%2d.%02d", ah, ah_frac);
      }
      else
      {
        ah = ah_deci / 100;
        ah_frac = ah_deci % 100;
        sprintf(&buffer[10], " %2d.%02d", ah, ah_frac);
      }
    }
  }
  else
  {
    TEST_POINT_HIGH;
    i++;
    if (i > 15)
    {
      i = 0;
    }
    buff_char[0] = buffer[i];
    ssd1306_printFixed(i*8, Y0, buff_char, STYLE_BOLD);
    TEST_POINT_LOW;

    screen_sleep_counter++;
    if (screen_sleep_counter == SLEEP_COUNT_MAX)
    {
      screen_asleep = true; 
      screen_sleep_counter = 0;
    }
  }
}