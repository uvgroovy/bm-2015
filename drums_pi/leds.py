import time
from buttons import Buttons

try:
    from neopixel import *
except ImportError:
    from fake_neopixel import *

ROWS = 3
PIXELS_PER_ROW = 16
LED_STRIP_PIN = 7
NUM_PIXELS = PIXELS_PER_ROW*ROWS
PIN = 18

buttons_to_leds = {
    Buttons.TOP_RIGHT:  (0, 3),
    Buttons.TOP_MIDV:   (4, 7),
    Buttons.TOP_LEFT:   (8, 12),
    Buttons.MID_LEFT: (18, 21),
    Buttons.MID_MID: (23, 25),
    Buttons.MID_RIGHT: (27, 30),
    Buttons.BUTT_RIGHT: (34, 36),
    Buttons.BUTT_MID: (37, 40),
    Buttons.BUTT_LEFT: (42, 45),
}

class LEDS(object):
    def __init__(self):
        self._neo = Adafruit_NeoPixel(NUM_PIXELS, PIN)

    def test(self):
        for row in xrange(ROWS):
            self._neo.begin()
            print 'testing row %d' % row
            for col in xrange(PIXELS_PER_ROW):
                self._neo.setPixelColor(row * PIXELS_PER_ROW + col, 0xFF << (8*row))

            self._neo.show()
            time.sleep(1)

        self.set_color_all(0)

        print 'testing done'

    def set_color_for_button(self, button, color):
        start, end = buttons_to_leds[button]

        self._neo.begin()
        for i in xrange(start, end + 1):
            self._neo.setPixelColor(i, color)
        self._neo.show()

    def set_color_all(self, color):
        self._neo.begin()
        for i in xrange(self._neo.numPixels()):
            self._neo.setPixelColor(i, color)
        self._neo.show()
