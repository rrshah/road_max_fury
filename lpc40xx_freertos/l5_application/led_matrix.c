#include "led_matrix.h"

extern RGBmatrixPanel led_matrix;
static Adafruit_GFX ada_gfx;
static uint16_t numPanels;

static gpio_s P_addrA; // addrA
static gpio_s P_addrB; // addrB
static gpio_s P_addrC; // addrC
static gpio_s P_addrD; // addrD
static gpio_s P_LATCH; // LATCH
static gpio_s P_OE;    // OE
static gpio_s P_CLOCK; // CLOCK
static gpio_s P_R1;    // R1
static gpio_s P_G1;    // G1
static gpio_s P_B1;    // B1
static gpio_s P_R2;    // R2
static gpio_s P_G2;    // G2
static gpio_s P_B2;    // B2

#define nPlanes 4
static void setup_led_matrix_pins() {
  P_addrA = gpio__construct_as_output(GPIO__PORT_0, 26);
  P_addrB = gpio__construct_as_output(GPIO__PORT_1, 31);
  P_addrC = gpio__construct_as_output(GPIO__PORT_1, 20);
  P_addrD = gpio__construct_as_output(GPIO__PORT_1, 28);
  P_LATCH = gpio__construct_as_output(GPIO__PORT_2, 2);
  P_OE = gpio__construct_as_output(GPIO__PORT_2, 5);
  P_CLOCK = gpio__construct_as_output(GPIO__PORT_2, 0);
  P_R1 = gpio__construct_as_output(GPIO__PORT_1, 0);
  P_G1 = gpio__construct_as_output(GPIO__PORT_1, 1);
  P_B1 = gpio__construct_as_output(GPIO__PORT_1, 4);
  P_R2 = gpio__construct_as_output(GPIO__PORT_4, 28);
  P_G2 = gpio__construct_as_output(GPIO__PORT_0, 6);
  P_B2 = gpio__construct_as_output(GPIO__PORT_0, 8);
}

void Adafruit_GFX_init(int16_t w, int16_t h) {
  ada_gfx._width = w;
  ada_gfx._height = h;
  ada_gfx.rotation = 0;
  ada_gfx.cursor_y = ada_gfx.cursor_x = 0;
  ada_gfx.textsize = 1;
  ada_gfx.textcolor = ada_gfx.textbgcolor = 0xFFFF;
  ada_gfx.wrap = true;
  // Default to GLCDFONT to be compatible with existing code
  // TODO setFont(GLCDFONT); // May also be set to TIMESNR_8, CENTURY_8,
  // COMICS_8 or TEST (for testing candidate fonts)
}

// Code common to both the 16x32 and 32x32 constructors:
static void init(uint8_t rows, bool dbuf, uint16_t width) {

  led_matrix.nRows =
      rows; // Number of multiplexed rows; actual height is 2X this

  // Allocate and initialize matrix buffer:
  uint32_t buffsize = width * led_matrix.nRows *
                      3, // x3 = 3 bytes holds 4 planes "packed"
      allocsize = (dbuf == true) ? (buffsize * 2) : buffsize;
  if (NULL == (led_matrix.matrixbuff[0] = (uint8_t *)malloc(allocsize)))
    return;
  memset(led_matrix.matrixbuff[0], 0, allocsize);
  // If not double-buffered, both buffers then point to the same address:
  led_matrix.matrixbuff[1] = (dbuf == true)
                                 ? &led_matrix.matrixbuff[0][buffsize]
                                 : led_matrix.matrixbuff[0];

  // Adjust timing for number of panels (and therefore pixels) wide
  numPanels = (width - 1) / 32;

  // Save pin numbers for use by begin() method later.
  //  _a     = a;
  //  _b     = b;
  //  _c     = c;
  //  _sclk  = sclk;
  //  _latch = latch;
  //  _oe    = oe;

  led_matrix.plane = nPlanes - 1;
  led_matrix.row = led_matrix.nRows - 1;
  led_matrix.swapflag = false;
  led_matrix.backindex = 0; // Array index of back buffer
}

// Constructor for 32x32 or 32x64 panel:
void RGBmatrixPanel_init(bool dbuf, uint16_t width) {

  Adafruit_GFX_init(width, 32);
  init(16, dbuf, width);
  setup_led_matrix_pins();
  // Init a few extra 32x32-specific elements:
  // _d        = d;
}

void led_matrix__begin(void) {

  led_matrix.backindex = 0; // Back buffer
  led_matrix.buffptr =
      led_matrix.matrixbuff[1 - led_matrix.backindex]; // -> front buffer
  // TODO activePanel = this;                                        // For
  // interrupt hander

  // Enable all comm & address pins as outputs, set default states:
  gpio__reset(P_CLOCK);
  gpio__reset(P_LATCH);
  gpio__set(P_OE);
  gpio__reset(P_addrA);
  gpio__reset(P_addrB);
  gpio__reset(P_addrC);
  if (led_matrix.nRows > 8) {
    gpio__reset(P_addrD);
  }

  gpio__reset(P_R1);
  gpio__reset(P_G1);
  gpio__reset(P_B1);
  gpio__reset(P_R2);
  gpio__reset(P_G2);
  gpio__reset(P_B2);

  // TODO rit_enable(refresh, 1);
  // TODO refreshTimer.begin(refreshISR, 200, uSec);
}

// Init/alloc code common to both constructors:

/*
void drawPixel(int16_t x, int16_t y, uint16_t c)
{
    uint8_t r, g, b, bit, limit, *ptr;

    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
        return;

    switch (gfx.rotation)
    {
    case 1:
        swap(x, y);
        x = WIDTH - 1 - x;
        break;
    case 2:
        x = WIDTH - 1 - x;
        y = HEIGHT - 1 - y;
        break;
    case 3:
        swap(x, y);
        y = HEIGHT - 1 - y;
        break;
    }

    // Adafruit_GFX uses 16-bit color in 5/6/5 format, while matrix needs
    // 4/4/4.  Pluck out relevant bits while separating into R,G,B:
    r = c >> 12;        // RRRRrggggggbbbbb
    g = (c >> 7) & 0xF; // rrrrrGGGGggbbbbb
    b = (c >> 1) & 0xF; // rrrrrggggggBBBBb

    // Loop counter stuff
    bit = 2;
    limit = 1 << nPlanes;

    if (y < nRows)
    {
        // Data for the upper half of the display is stored in the lower
        // bits of each byte.
        ptr = &matrixbuff[backindex][y * WIDTH * (nPlanes - 1) + x]; // Base
addr
        // Plane 0 is a tricky case -- its data is spread about,
        // stored in least two bits not used by the other planes.
        ptr[WIDTH * 2] &= ~0B00000011; // Plane 0 R,G mask out in one op
        if (r & 1)
            ptr[WIDTH * 2] |= 0B00000001; // Plane 0 R: 64 bytes ahead, bit 0
        if (g & 1)
            ptr[WIDTH * 2] |= 0B00000010; // Plane 0 G: 64 bytes ahead, bit 1
        if (b & 1)
            ptr[WIDTH] |= 0B00000001; // Plane 0 B: 32 bytes ahead, bit 0
        else
            ptr[WIDTH] &= ~0B00000001; // Plane 0 B unset; mask out
        // The remaining three image planes are more normal-ish.
        // Data is stored in the high 6 bits so it can be quickly
        // copied to the DATAPORT register w/6 output lines.
        for (; bit < limit; bit <<= 1)
        {
            *ptr &= ~0B00011100; // Mask out R,G,B in one op
            if (r & bit)
                *ptr |= 0B00000100; // Plane N R: bit 2
            if (g & bit)
                *ptr |= 0B00001000; // Plane N G: bit 3
            if (b & bit)
                *ptr |= 0B00010000; // Plane N B: bit 4
            ptr += WIDTH;           // Advance to next bit plane
        }
    }
    else
    {
        // Data for the lower half of the display is stored in the upper
        // bits, except for the plane 0 stuff, using 2 least bits.
        ptr = &matrixbuff[backindex][(y - nRows) * WIDTH * (nPlanes - 1) + x];
        *ptr &= ~0B00000011; // Plane 0 G,B mask out in one op
        if (r & 1)
            ptr[WIDTH] |= 0B00000010; // Plane 0 R: 32 bytes ahead, bit 1
        else
            ptr[WIDTH] &= ~0B00000010; // Plane 0 R unset; mask out
        if (g & 1)
            *ptr |= 0B00000001; // Plane 0 G: bit 0
        if (b & 1)
            *ptr |= 0B00000010; // Plane 0 B: bit 0
        for (; bit < limit; bit <<= 1)
        {
            *ptr &= ~0B11100000; // Mask out R,G,B in one op
            if (r & bit)
                *ptr |= 0B00100000; // Plane N R: bit 5
            if (g & bit)
                *ptr |= 0B01000000; // Plane N G: bit 6
            if (b & bit)
                *ptr |= 0B10000000; // Plane N B: bit 7
            ptr += WIDTH;           // Advance to next bit plane
        }
    }
}
*/