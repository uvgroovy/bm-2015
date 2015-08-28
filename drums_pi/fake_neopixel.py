def Color(red, green, blue):
    """Convert the provided red, green, blue color to a 24-bit color value.
    Each color component should be a value 0-255 where 0 is the lowest intensity
    and 255 is the highest intensity.
    """
    return (red << 16) | (green << 8) | blue

class Adafruit_NeoPixel(object):
    def __init__(self, num, pin, freq_hz=800000, dma=5, invert=False, brightness=255, channel=0):
        self._num = num

    def begin(self):
        print 'Adafruit_NeoPixel begin'

    def show(self):
        print 'Adafruit_NeoPixel show'

    def setPixelColor(self, n, color):
        print 'Adafruit_NeoPixel setPixelColor(%s, %08x)' % (n, color)

    def numPixels(self):
        return self._num
