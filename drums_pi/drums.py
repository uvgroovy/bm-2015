from datetime import datetime
import time
from buttons import Buttons
from leds import LEDS

STOP_PLAYING_THRESHOLD = 60

class SequenceNode(object):
    def __init__(self):
        pass

class Drums(object):
    def __init__(self):
        print 'Starting up drums!'
        self._buttons = Buttons()
        self._led = LEDS()
        self._sequenceQueue = []
        self._play = False
        self._last_key_time = datetime.now()

        self._led.test()

    def run(self):
        while True:
            now = datetime.now()
            if not self._buttons.has_keys() and (now - self._last_key_time).total_seconds() > STOP_PLAYING_THRESHOLD:
                print 'sequence ended. going to sleep (kinda!)'
                self._play = False

            if self._buttons.has_keys():
                if not self._play:
                    print 'start playing!'

                self._play = True
                self._last_key_time = now
                self._process_key(self._buttons.get_key())

            if not self._play:
                time.sleep(1)
                continue

            time.sleep(0.05)


    def _process_key(self, key):
        print 'processing key %s' % key
        self._led.set_color_for_button(key, 0xFF)


drums = Drums()

drums.run()



