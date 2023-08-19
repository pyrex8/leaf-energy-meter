// Nissan Leaf EV-CAN Energy Meter

#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);

#define SECOND_ROW 32

#define KWH_FACTOR 775

long unsigned int rx_id;
unsigned char len = 0;
unsigned char rx_buf[8];

uint16_t soc = 0;
uint16_t gids = 0;
uint16_t kwh_deci = 0;
uint16_t kwh = 0;
uint16_t kwh_frac = 0;

char buff_kwh[9];
char buff_soc[9];

void display_update()
{
  sprintf(buff_kwh, "%2d.%01d kWh", kwh, kwh_frac);
  sprintf(buff_soc, "  %2d %%  ", soc);
  ssd1306_printFixed2x(0, 0, buff_kwh, STYLE_BOLD);
  ssd1306_printFixed2x(0, SECOND_ROW, buff_soc, STYLE_BOLD);
}

void setup()
{
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();
  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x055b0000);
  CAN0.init_Mask(1, 0, 0x07ff0000);
  CAN0.init_Filt(2, 0, 0x05bc0000);
  CAN0.setMode(MCP_NORMAL);
  pinMode(CAN0_INT, INPUT);
  display_update();
}

void loop()
{
  if (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rx_id, &len, rx_buf);

    if (rx_id == 0x55b) // 100ms
    {
      soc = ((rx_buf[0] << 2) | (rx_buf[1] >> 6)) / 10;
    }

    if (rx_id == 0x5bc) // 500ms
    {
      gids = (rx_buf[0] << 2) | (rx_buf[1] >> 6);
      kwh_deci = (int16_t)(((int32_t)gids * KWH_FACTOR) / 1000);

      kwh = kwh_deci / 10;
      kwh_frac = kwh_deci % 10;

      display_update();
    }
  }
}