#include "led_matrix.h"
#include "delay.h"
#include <stdio.h>
#define nPlanes 4

// LED Matrix parameters
extern RGBmatrixPanel led_matrix;
static Adafruit_GFX ada_gfx;

static uint16_t numPanels;

// GPIO for driving the LED Matrix
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

/*
In updateDisplay() func :
Get the time to next interrupt
duration = dur[numPanels][led_matrix.plane];
 //For 32x64 matrix --> 2nd row of matrix is used (See below numPanels
defintion). ie... 60, 120, 240, 480
*/
static uint16_t dur[4][4] = {{50, 100, 200, 400}, // 1 panel
                             {60, 120, 240, 480},
                             {70, 140, 280, 560},
                             {70, 140, 280, 560}}; // 4 panels

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

int16_t height(void) { return ada_gfx._height; }
int16_t width(void) { return ada_gfx._width; }

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
  uint32_t buffsize =
      width * led_matrix.nRows * 3; // x3 = 3 bytes holds 4 planes "packed"
  uint32_t allocsize = (dbuf == true) ? (buffsize * 2) : buffsize;
  if (NULL == (led_matrix.matrixbuff[0] = (uint8_t *)malloc(allocsize)))
    return;
  memset(led_matrix.matrixbuff[0], 0, allocsize);
  // If not double-buffered, both buffers then point to the same address:
  led_matrix.matrixbuff[1] = (dbuf == true)
                                 ? &led_matrix.matrixbuff[0][buffsize]
                                 : led_matrix.matrixbuff[0];

  // Adjust timing for number of panels (and therefore pixels) wide
  // For 32x64 matrix -->
  // numPanels = (64 - 1)/32 = 1.98 ~ 1 (rounded)
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
void drawPixel(int16_t x, int16_t y, uint16_t c) {
  uint8_t r, g, b, bit, limit, *ptr;

  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // rotation=0 for our use case
  //   switch (ada_gfx.rotation) {
  //   case 1:
  //     swap(x, y);
  //     x = WIDTH - 1 - x;
  //     break;
  //   case 2:
  //     x = WIDTH - 1 - x;
  //     y = HEIGHT - 1 - y;
  //     break;
  //   case 3:
  //     swap(x, y);
  //     y = HEIGHT - 1 - y;
  //     break;
  //   }

  // Adafruit_GFX uses 16-bit color in 5/6/5 format, while matrix needs
  // 4/4/4.  Pluck out relevant bits while separating into R,G,B:
  r = c >> 12;        // RRRRrggggggbbbbb
  g = (c >> 7) & 0xF; // rrrrrGGGGggbbbbb
  b = (c >> 1) & 0xF; // rrrrrggggggBBBBb

  // Loop counter stuff
  bit = 2;
  limit = 1 << nPlanes;

  printf("x=%d, y=%d, index=%d\n", x, y, y * WIDTH * (nPlanes - 1) + x);

  if (y < led_matrix.nRows) {
    // Data for the upper half of the display is stored in the lower
    // bits of each byte.
    ptr = &led_matrix.matrixbuff[led_matrix.backindex]
                                [y * WIDTH * (nPlanes - 1) + x]; // Baseaddr

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
    for (; bit < limit; bit <<= 1) {
      *ptr &= ~0B00011100; // Mask out R,G,B in one op
      if (r & bit)
        *ptr |= 0B00000100; // Plane N R: bit 2
      if (g & bit)
        *ptr |= 0B00001000; // Plane N G: bit 3
      if (b & bit)
        *ptr |= 0B00010000; // Plane N B: bit 4
      ptr += WIDTH;         // Advance to next bit plane
    }
  } else {
    // Data for the lower half of the display is stored in the upper
    // bits, except for the plane 0 stuff, using 2 least bits.
    ptr = &led_matrix
               .matrixbuff[led_matrix.backindex]
                          [(y - led_matrix.nRows) * WIDTH * (nPlanes - 1) + x];
    *ptr &= ~0B00000011; // Plane 0 G,B mask out in one op
    if (r & 1)
      ptr[WIDTH] |= 0B00000010; // Plane 0 R: 32 bytes ahead, bit 1
    else
      ptr[WIDTH] &= ~0B00000010; // Plane 0 R unset; mask out
    if (g & 1)
      *ptr |= 0B00000001; // Plane 0 G: bit 0
    if (b & 1)
      *ptr |= 0B00000010; // Plane 0 B: bit 0
    for (; bit < limit; bit <<= 1) {
      *ptr &= ~0B11100000; // Mask out R,G,B in one op
      if (r & bit)
        *ptr |= 0B00100000; // Plane N R: bit 5
      if (g & bit)
        *ptr |= 0B01000000; // Plane N G: bit 6
      if (b & bit)
        *ptr |= 0B10000000; // Plane N B: bit 7
      ptr += WIDTH;         // Advance to next bit plane
    }
  }
  uint8_t *base =
      led_matrix
          .matrixbuff[led_matrix.backindex][y * WIDTH * (nPlanes - 1) + x];
  printf("Pixel (%d, %d) = %x, %x, %x\n", x, y, *base, base[WIDTH],
         base[2 * WIDTH]);
  updateDisplay();
  delay__ms(1);
  updateDisplay();
  delay__ms(1);
  updateDisplay();
  delay__ms(1);
  updateDisplay();
}

// Original RGBmatrixPanel library used 3/3/3 color.  Later version used
// 4/4/4.  Then Adafruit_GFX (core library used across all Adafruit
// display devices now) standardized on 5/6/5.  The matrix still operates
// internally on 4/4/4 color, but all the graphics functions are written
// to expect 5/6/5...the matrix lib will truncate the color components as
// needed when drawing.  These next functions are mostly here for the
// benefit of older code using one of the original color formats.

// Promote 3/3/3 RGB to Adafruit_GFX 5/6/5
uint16_t Color333(uint8_t r, uint8_t g, uint8_t b) {
  // RRRrrGGGgggBBBbb
  return ((r & 0x7) << 13) | ((r & 0x6) << 10) | ((g & 0x7) << 8) |
         ((g & 0x7) << 5) | ((b & 0x7) << 2) | ((b & 0x6) >> 1);
}

// void updateDisplay(void) {
//   uint8_t i, *ptr;
//   uint16_t duration;

//   gpio__set(P_OE);      // Disable LED output during row/plane switchover
//   gpio__set(P_LATCH);   // Latch data loaded during *prior* interrupt
//   gpio__reset(P_CLOCK); // Start the clock LOW

//   // Get the time to next interrupt
//   duration = dur[numPanels][led_matrix.plane];

//   // Borrowing a technique here from Ray's Logic:
//   // www.rayslogic.com/propeller/Programming/AdafruitRGB/AdafruitRGB.htm
//   // This code cycles through all four planes for each scanline before
//   // advancing to the next line.  While it might seem beneficial to
//   // advance lines every time and interleave the planes to reduce
//   // vertical scanning artifacts, in practice with this panel it causes
//   // a green 'ghosting' effect on black pixels, a much worse artifact.

//   if (++led_matrix.plane >= nPlanes) { // Advance plane counter.  Maxed out?
//     led_matrix.plane = 0;              // Yes, reset to plane 0, and
//     if (++led_matrix.row >=
//         led_matrix.nRows) {              // advance row counter.  Maxed out?
//       led_matrix.row = 0;                // Yes, reset row counter, then...
//       if (led_matrix.swapflag == true) { // Swap front/back buffers if
//       requested
//         led_matrix.backindex = 1 - led_matrix.backindex;
//         led_matrix.swapflag = false;
//       }
//       led_matrix.buffptr =
//           led_matrix
//               .matrixbuff[1 - led_matrix.backindex]; // Reset into front
//               buffer
//     }
//   } else if (led_matrix.plane == 1) {
//     // Plane 0 was loaded on prior interrupt invocation and is about to
//     // latch now, so update the row address lines before we do that:

//     (led_matrix.row & 0x1) ? gpio__set(P_addrA) : gpio__reset(P_addrA);
//     (led_matrix.row & 0x2) ? gpio__set(P_addrB) : gpio__reset(P_addrB);
//     (led_matrix.row & 0x4) ? gpio__set(P_addrC) : gpio__reset(P_addrC);
//     if (led_matrix.nRows > 8) {
//       (led_matrix.row & 0x8) ? gpio__set(P_addrD) : gpio__reset(P_addrD);
//     }
//   }

//   // buffptr, being 'volatile' type, doesn't take well to optimization.
//   // A local register copy can speed some things up:
//   ptr = (uint8_t *)led_matrix.buffptr;

//   // RESET timer duration
//   // refreshTimer.resetPeriod_SIT(duration, uSec);

//   gpio__reset(P_OE);    // Re-enable output
//   gpio__reset(P_LATCH); // Latch down

//   if (led_matrix.plane > 0) {

//     // Planes 1-3 must be unpacked and bit-banged
//     for (i = 0; i < WIDTH; i++) {

//       (ptr[i] & 0x04) ? gpio__set(P_R1) : gpio__reset(P_R1); // R1
//       (ptr[i] & 0x08) ? gpio__set(P_G1) : gpio__reset(P_G1); // G1
//       (ptr[i] & 0x10) ? gpio__set(P_B1) : gpio__reset(P_B1); // B1
//       (ptr[i] & 0x20) ? gpio__set(P_R2) : gpio__reset(P_R2); // R2
//       (ptr[i] & 0x40) ? gpio__set(P_G2) : gpio__reset(P_G2); // G2
//       (ptr[i] & 0x80) ? gpio__set(P_B2) : gpio__reset(P_B2); // B2
//       gpio__set(P_CLOCK);                                    // hi
//       gpio__reset(P_CLOCK);                                  // lo
//     }

//     led_matrix.buffptr += WIDTH;
//   } else {
//     // Plane 0 has its data packed into the 2 least bits not
//     // used by the other planes.  This works because the unpacking and
//     // output for plane 0 is handled while plane 3 is being displayed...
//     // because binary coded modulation is used (not PWM), that plane
//     // has the longest display interval, so the extra work fits.

//     for (i = 0; i < WIDTH; i++) {
//       uint8_t bits = (ptr[i] << 6) | ((ptr[i + WIDTH] << 4) & 0x30) |
//                      ((ptr[i + WIDTH * 2] << 2) & 0x0C);

//       (bits & 0x04) ? gpio__set(P_R1) : gpio__reset(P_R1); // R1
//       (bits & 0x08) ? gpio__set(P_G1) : gpio__reset(P_G1); // G1
//       (bits & 0x10) ? gpio__set(P_B1) : gpio__reset(P_B1); // B1
//       (bits & 0x20) ? gpio__set(P_R2) : gpio__reset(P_R2); // R2
//       (bits & 0x40) ? gpio__set(P_G2) : gpio__reset(P_G2); // G2
//       (bits & 0x80) ? gpio__set(P_B2) : gpio__reset(P_B2); // B2
//       gpio__set(P_CLOCK);                                  // hi
//       gpio__reset(P_CLOCK);                                // lo
//     }
//   }
// }

void updateDisplay(void) {
  uint8_t i, *ptr;

  gpio__set(P_OE);      // Disable LED output during row/plane switchover
  gpio__set(P_LATCH);   // Latch data loaded during *prior* interrupt
  gpio__reset(P_CLOCK); // Start the clock LOW

  if (++led_matrix.plane >= nPlanes) { // Advance plane counter.  Maxed out?
    led_matrix.plane = 0;              // Yes, reset to plane 0, and
    if (++led_matrix.row >=
        led_matrix.nRows) { // advance row counter.  Maxed out?
      led_matrix.row = 0;   // Yes, reset row counter, then...
      led_matrix.buffptr =
          led_matrix
              .matrixbuff[1 - led_matrix.backindex]; // Reset into front buffer
    }
  } else if (led_matrix.plane == 1) {
    // Plane 0 was loaded on prior interrupt invocation and is about to
    // latch now, so update the row address lines before we do that:

    (led_matrix.row & 0x1) ? gpio__set(P_addrA) : gpio__reset(P_addrA);
    (led_matrix.row & 0x2) ? gpio__set(P_addrB) : gpio__reset(P_addrB);
    (led_matrix.row & 0x4) ? gpio__set(P_addrC) : gpio__reset(P_addrC);
    if (led_matrix.nRows > 8) {
      (led_matrix.row & 0x8) ? gpio__set(P_addrD) : gpio__reset(P_addrD);
    }
  }

  // buffptr, being 'volatile' type, doesn't take well to optimization.
  // A local register copy can speed some things up:
  ptr = (uint8_t *)led_matrix.buffptr;

  // RESET timer duration
  // refreshTimer.resetPeriod_SIT(duration, uSec);

  gpio__reset(P_OE);    // Re-enable output
  gpio__reset(P_LATCH); // Latch down

  if (led_matrix.plane > 0) {

    // Planes 1-3 must be unpacked and bit-banged
    for (i = 0; i < 1; i++) {

      (ptr[i] & 0x04) ? gpio__set(P_R1) : gpio__reset(P_R1); // R1
      (ptr[i] & 0x08) ? gpio__set(P_G1) : gpio__reset(P_G1); // G1
      (ptr[i] & 0x10) ? gpio__set(P_B1) : gpio__reset(P_B1); // B1
      (ptr[i] & 0x20) ? gpio__set(P_R2) : gpio__reset(P_R2); // R2
      (ptr[i] & 0x40) ? gpio__set(P_G2) : gpio__reset(P_G2); // G2
      (ptr[i] & 0x80) ? gpio__set(P_B2) : gpio__reset(P_B2); // B2
      gpio__set(P_CLOCK);                                    // hi
      gpio__reset(P_CLOCK);                                  // lo
    }

    led_matrix.buffptr += WIDTH;
  } else {
    // Plane 0 has its data packed into the 2 least bits not
    // used by the other planes.  This works because the unpacking and
    // output for plane 0 is handled while plane 3 is being displayed...
    // because binary coded modulation is used (not PWM), that plane
    // has the longest display interval, so the extra work fits.

    for (i = 0; i < 1; i++) {
      uint8_t bits = (ptr[i] << 6) | ((ptr[i + WIDTH] << 4) & 0x30) |
                     ((ptr[i + WIDTH * 2] << 2) & 0x0C);

      (bits & 0x04) ? gpio__set(P_R1) : gpio__reset(P_R1); // R1
      (bits & 0x08) ? gpio__set(P_G1) : gpio__reset(P_G1); // G1
      (bits & 0x10) ? gpio__set(P_B1) : gpio__reset(P_B1); // B1
      (bits & 0x20) ? gpio__set(P_R2) : gpio__reset(P_R2); // R2
      (bits & 0x40) ? gpio__set(P_G2) : gpio__reset(P_G2); // G2
      (bits & 0x80) ? gpio__set(P_B2) : gpio__reset(P_B2); // B2
      gpio__set(P_CLOCK);                                  // hi
      gpio__reset(P_CLOCK);                                // lo
    }
  }
}