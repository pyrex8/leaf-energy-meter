// Nissan Leaf EV-CAN

// Battery current Amp
// 0.5A resolution
// 10ms
// 2 counts = 1 A
// 2 x 360000 = 720000 = 1 Ah
// 40 Ah x 2 counts/A x 100 Hz x 60 sec/min 60min/h = 28.8 x 10e6
// int32_t

#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define COUNTS_PER_DECI_AH 7200

#define DELAY_1MS 1
#define SLEEP_TIMEOUT_MINUTES 1
#define SLEEP_COUNTER_MS 1000
#define SLEEP_COUNTER_SEC (SLEEP_TIMEOUT_MINUTES * 60)

#define SLEEP_COUNT_OFF 1000
#define SLEEP_COUNT_ON 4000

#define LINE_SPACING 32
#define X0 0
#define Y0 0
#define Y1 (Y0 + LINE_SPACING)

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

int16_t amp = 0;
int16_t ah = 0;
int16_t ah_frac = 0;
int32_t ah_count = 0;
int32_t ah_count_start = 0;
int32_t ah_count_trip = 0;
int16_t ah_deci = 0;

bool screen_asleep = true;
uint16_t screen_sleep_counter = 0;

char buffer[10];
char buf_int[4];
char buf_frac[4];


void oled_update(uint8_t x, uint8_t y, int32_t ah_cnt)
{
  ah_deci = (int16_t)(ah_cnt / COUNTS_PER_DECI_AH); 
  if (ah_cnt < 0)
  {
    ah_deci = -ah_deci;
    ah = ah_deci / 100;
    ah_frac = ah_deci % 100;
    sprintf(buffer, "-%2d.%02dAh", ah, ah_frac);
  }
  else
  {
    ah = ah_deci / 100;
    ah_frac = ah_deci % 100;
    sprintf(buffer, " %2d.%02dAh", ah, ah_frac);
  }
  ssd1306_printFixed2x(x, y, buffer, STYLE_BOLD);
}

void oled_update_all()
{
  oled_update(X0, Y0, ah_count);
  oled_update(X0, Y1, ah_count_trip);
}

void setup()
{
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();    
  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x01db0000);
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);
  oled_update_all();
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
        ah_count_start = ah_count;
      }
      
      ah_count_trip = ah_count - ah_count_start;
      oled_update_all();
    }
  }
  else
  {
    screen_sleep_counter++;
    if (screen_sleep_counter == SLEEP_COUNT_OFF)
    {
      ssd1306_clearScreen();
      screen_asleep = true; 
    }
    if (screen_sleep_counter == SLEEP_COUNT_ON)
    {
      oled_update_all();
      screen_sleep_counter = 0;
    }
    delay(DELAY_1MS);
  }
}