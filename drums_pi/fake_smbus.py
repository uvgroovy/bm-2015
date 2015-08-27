class SMBus(object):
    def __init__(self, dev):
        self._dev = dev
        print "fakesmbus: %s" % self._dev

    def read_byte(self, address):
        print "fakesmbus read from address: %s" % self._dev
        return 1