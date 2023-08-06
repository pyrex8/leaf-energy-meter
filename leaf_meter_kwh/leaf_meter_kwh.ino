// Nissan Leaf CAR-CAN

#include <ssd1306.h>
#include <mcp_can.h>
#include <SPI.h>

#define CAN0_INT 2
MCP_CAN CAN0(10);
#define KWH_FACTOR 77.5F
#define TEN_MSEC_PER_HOUR 360000.0F
#define LINE_SPACING 16
#define X0 32
#define Y0 0
#define Y1 (Y0 + LINE_SPACING)
#define Y2 (Y1 + LINE_SPACING)
#define Y3 (Y2 + LINE_SPACING)

#define frac(x) (int(10*(x - int(x))))

long unsigned int rx_id;
unsigned char len = 0;
unsigned char rx_buf[8];

unsigned long raw_odo;
float odo_km_start;
float odo_km;

byte raw_battery_pack_temp;    // raw battery pack temperature
byte raw_battery_soh;          // battery State of Health %
byte raw_gids;                 // raw Gids from Vehicle CAN bus x5b3
byte raw_gids2;                // raw Gids from Vehicle CAN bus x5b3


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
  oled_update(X0, Y1, odo_km);
  oled_update(X0, Y2, kwh_trip);
  oled_update(X0, Y3, kwh);
}

void setup()
{
  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  ssd1306_clearScreen();    

  CAN0.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ);
  CAN0.init_Mask(0, 0, 0x07ff0000);
  CAN0.init_Filt(0, 0, 0x05b30000);
  CAN0.init_Filt(1, 0, 0x05c50000); 
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);
  oled_update_all();
}

// CAR-CAN
// 0x5b3 temp, soh, soc 
// 0x5c5 odometer

void loop()
{
  if (!digitalRead(CAN0_INT))
  {
    CAN0.readMsgBuf(&rx_id, &len, rx_buf);
    if (rx_id == 0x5b3)
    {
      raw_battery_pack_temp = (rx_buf[0]);
      raw_battery_soh = (rx_buf[1]) >> 1;

      soc = constrain((rx_buf[5]), 0, 255); // contrained to fit in one byte
      kwh = (((float)soc) * KWH_FACTOR) / 1000.0F;
      if (kwh_start < 0.1)
      {
        kwh_start = kwh;
      }
      kwh_trip = kwh_start - kwh;
      oled_update_all();
    }
    if (rx_id == 0x5c5)
    {
      raw_odo = (rx_buf[1] << 16 | rx_buf[2] << 8 | rx_buf[3]);
      if (odo_km_start > 0.0)
      {
        odo_km = ((float)raw_odo) - odo_km_start;
        
      } else
      {
        odo_km_start = (float)raw_odo;
      }
    }
  }
}