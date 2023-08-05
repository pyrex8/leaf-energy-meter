#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
uint16_t soc;
float kwh = 10.0f;
char buf_int[4];
char buf_frac[4];

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define KWH_FACTOR 77.5F
#define frac(x) (int(10*(x - int(x))))

void oled_update()
{
  sprintf(buf_int, "%2d", int(kwh));
  sprintf(buf_frac, "%01d", frac(kwh));
  ssd1306_printFixed2x(20, 16, buf_int, STYLE_BOLD);
  ssd1306_printFixed2x(65, 16, buf_frac, STYLE_BOLD);
}

void setup()
{
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();    
  ssd1306_printFixed(90, 26, "kWh", STYLE_BOLD);
  ssd1306_printFixed2x(54, 16, ".", STYLE_BOLD);

  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x05bc0000); 
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);
  oled_update();
}

void loop()
{
  if (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);
    if (rxId == 0x5bc)
    {
      soc = (rxBuf[0] << 2) | (rxBuf[1] >> 6);
      kwh = (((float)soc) * KWH_FACTOR) / 1000.0F;
      oled_update();
    }
  }
}