import sys
import select


class SMBus(object):
    def __init__(self, dev):
        self._dev = dev
        print "fakesmbus: %s" % self._dev


    def read_byte(self, address):
        i,o,e = select.select([sys.stdin],[],[],0.0001)
        for s in i:
            if s == sys.stdin:
                print 'getting input'
                result = sys.stdin.readline()
                return int(result)
        return 0
