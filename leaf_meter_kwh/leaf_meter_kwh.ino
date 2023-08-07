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

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];

int16_t amp = 0;
bool amp_positive = true;
int16_t ah = 0;
int16_t ah_frac = 0;
int32_t ah_count = 0;
int16_t ah_deci = 0;
uint16_t screen_sleep_counter_ms = 0;
uint16_t screen_sleep_counter_sec = 0;
bool screen_asleep = true;

char buf_int[4];
char buf_frac[4];

void oled_update()
{
  if (screen_asleep)
  {
    screen_asleep = false;
    ssd1306_printFixed2x(88, 16, "Ah", STYLE_BOLD);
    ssd1306_printFixed2x(41, 16, ".", STYLE_BOLD);
  }

  ah_deci = (int16_t)(ah_count / COUNTS_PER_DECI_AH);
  if (ah_count < 0)
  {
    ssd1306_printFixed2x(0, 16, "-", STYLE_BOLD);
    ah_deci = -ah_deci;
  }
  else
  {
    ssd1306_printFixed2x(0, 16, " ", STYLE_BOLD);
  }

  ah = ah_deci / 100;
  ah_frac = ah_deci % 100;
  sprintf(buf_int, "%2d", ah);
  sprintf(buf_frac, "%02d", ah_frac);
  ssd1306_printFixed2x(7, 16, buf_int, STYLE_BOLD);
  ssd1306_printFixed2x(52, 16, buf_frac, STYLE_BOLD);
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
  oled_update();
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
      oled_update();
      screen_sleep_counter_ms = 0;
      screen_sleep_counter_sec = 0;
    }
  }
  else
  {
    screen_sleep_counter_ms++;
    if (screen_sleep_counter_ms > SLEEP_COUNTER_MS)
    {
      screen_sleep_counter_ms = 0;
      screen_sleep_counter_sec++;
      if (screen_sleep_counter_sec > SLEEP_COUNTER_SEC)
      {
        screen_sleep_counter_sec = 0;
        ssd1306_clearScreen();
        screen_asleep = true;
      }
    }
    delay(DELAY_1MS);
  }
}