try:
    import smbus
except ImportError:
    import fake_smbus as smbus

# for RPI version 1, use "bus = smbus.SMBus(0)"

# This is the address we setup in the Arduino Program
address = 0x04


class Buttons(object):
    BUTTON_NONE = 0
    TOP_RIGHT = 1
    BUTT_RIGHT = 2
    MID_MID = 3
    ARR_DOWN = 4
    ARR_UP = 5
    MID_LEFT = 6
    BUTT_LEFT = 7
    TOP_MIDV = 8
    TOP_LEFT = 9
    MID_RIGHT = 10
    BUTT_MID = 11

    def __init__(self):
        self._bus = smbus.SMBus(1)
        self._key = 0

    def key_string(self, keycode):
        return {
            self.BUTTON_NONE: 'BUTTON_NONE',
            self.TOP_RIGHT: 'TOP_RIGHT',
            self.BUTT_RIGHT: 'BUTT_RIGHT',
            self.MID_MID: 'MID_MID',
            self.ARR_DOWN: 'ARR_DOWN',
            self.ARR_UP: 'ARR_UP',
            self.MID_LEFT: 'MID_LEFT',
            self.BUTT_LEFT: 'BUTT_LEFT',
            self.TOP_MIDV: 'TOP_MIDV',
            self.TOP_LEFT: 'TOP_LEFT',
            self.MID_RIGHT: 'MID_RIGHT',
            self.BUTT_MID: 'BUTT_MID',
        }[keycode]

    def has_keys(self):
        self._poll_key_if_needed()
        return self._key != 0

    def _poll_key_if_needed(self):
        if not self._key:
            try:
                self._key = self._poll_key()
            except IOError:
                self._key = 0
                print 'error polling'

    def _poll_key(self):
        return self._bus.read_byte(address)

    def get_key(self):
        self._poll_key_if_needed()
        key = self._key
        self._key = None
        return key


if __name__ == '__main__':
    buttons = Buttons()
    print buttons.get_key()
