#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define KWH_FACTOR 77.5F
#define TEN_MSEC 0.01F
#define LINE_SPACING 16
#define X0 32
#define Y0 0
#define Y1 (Y0 + LINE_SPACING)
#define Y2 (Y1 + LINE_SPACING)
#define Y3 (Y2 + LINE_SPACING)

#define frac(x) (int(10*(x - int(x))))

long unsigned int rxId;
unsigned char len = 0;
unsigned char rxBuf[8];
float batt_volts = 0.0;
float bat_amps = 0.0;
float bat_kwh_trip = 0.0;

uint16_t soc;
float kwh = 0.0;
float kwh_start = 0.0;
float kwh_trip = 0.0;
char buffer[10];
char buf_int[4];
char buf_frac[4];

void oled_update(uint8_t x, uint8_t y, float kwhr)
{
  sprintf(buffer, "%2d.%01d kWh", int(kwhr), frac(kwhr));
  ssd1306_printFixed(x, y, buffer, STYLE_BOLD);
}

void oled_update_all()
{
  oled_update(X0, Y0, kwh);
  oled_update(X0, Y2, kwh_trip);
  oled_update(X0, Y3, bat_kwh_trip);
}

void setup()
{
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();    

  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x05bc0000); 
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

    }
    if (rxId == 0x5bc)
    {
      soc = (rxBuf[0] << 2) | (rxBuf[1] >> 6);
      kwh = (((float)soc) * KWH_FACTOR) / 1000.0F;
      if (kwh_start < 0.1)
      {
        kwh_start = kwh;
      }
      kwh_trip = kwh_start - kwh;
      oled_update_all();
    }
  }
}