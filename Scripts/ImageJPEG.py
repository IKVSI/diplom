#!/usr/bin/python3
import os
import struct

def strbytes(b):
    r = ""
    for i in b:
        s = hex(i)[2:]
        s = s if (len(s)-1) else "0" + s
        r += "\\x" + s
    return r

class ReadFile():
    def __init__(self, filename):
        self.filename = filename
        self.buffers = [
            b"",
            b"",
            b""
        ]
        self.buffersize = 512
        self.curentblock = -3
        with open(filename, "rb") as fin:
            fin.seek(0, os.SEEK_END)
            self.size = fin.tell()
        self.lastblock = self.size // self.buffersize

    def loadBlock(self, nb):
        if self.curentblock != nb:
            for i in range(3):
                if -1 <= nb + i - 1 - self.curentblock <= 1:
                    self.buffers[i] = self.buffers[nb + i - self.curentblock]
                else:
                    temp = min(max(0, nb + i - 1), self.lastblock)
                    with open(self.filename, "rb") as fin:
                        fin.seek(temp * self.buffersize, os.SEEK_SET)
                        self.buffers[i] = fin.read(self.buffersize)
        self.curentblock = nb

    def getBytes(self, start, length):
        start = max(start, 0)
        end = min(self.size, start + length)
        block = start // self.buffersize
        endblock = end // self.buffersize
        end -= self.buffersize * endblock
        start -= self.buffersize * block
        self.loadBlock(block)
        if block == endblock:
            r = self.buffers[1][start:end]
        else:
            r = self.buffers[1][start:]
            block += 1
            while block != endblock:
                self.loadBlock(block)
                r += self.buffers[1]
                block += 1
            r += self.buffers[2][:end]
        return r


class JPEG():
    MARKERS = {
        b"\xFF\xD8": lambda x: x.markerSOI(),
        b"\xFF\xD9": lambda x: x.markerEOI(),
        b"\xFF\xC4": lambda x: x.markerDHT()
    }
    SKIPMARKERS = [
        b"\x00",
        b"\xD0",
        b"\xD1",
        b"\xD2",
        b"\xD3",
        b"\xD4",
        b"\xD5",
        b"\xD6",
        b"\xD7"
    ]
    def __init__(self, filename):
        self.filename = filename
        self.file = ReadFile(self.filename)
        self.start = False
        self.end = False
        self.cursor = 0
        self.decode()
    
    def decode(self):
        self.cursor = 0
        self.savecursor = self.cursor
        marker = self.file.getBytes(self.cursor, 2)
        if marker in JPEG.MARKERS:
            JPEG.MARKERS[marker](self)
        if not self.start:
            raise ValueError("Marker \"Start of Image\" not found!")
        while not self.end:
            marker = self.file.getBytes(self.cursor, 2)
            if marker in JPEG.MARKERS:
                JPEG.MARKERS[marker](self)
            elif marker[:1] == b"\xFF":
                self.ignoreMarker()
            else:
                self.findNextMarker()

    def markerSOI(self):
        print("Marker \"Start of Image\" found! on {}".format(hex(self.cursor)))
        self.start = True
        self.cursor += 2
    
    def markerEOI(self):
        print("Marker \"End of Image\" found! on {}".format(hex(self.cursor)))
        self.end = True
        self.cursor += 2

    def ignoreMarker(self):
        print("Marker ({}) found! on {}".format(strbytes(self.file.getBytes(self.cursor, 2)), hex(self.cursor)))
        self.cursor += 2
        self.savecursor = self.cursor
        length = struct.unpack(">H", self.file.getBytes(self.cursor, 2))[0]
        print("\tLength: {}".format(length))
        self.cursor += length

    def findNextMarker(self):
        self.cursor = self.savecursor
        while (self.file.getBytes(self.cursor, 1) != b'\xFF') or (self.file.getBytes(self.cursor + 1, 1) in JPEG.SKIPMARKERS):
            self.cursor += 1

    def markerDHT(self):
        print("Marker \"Define Huffman Table\" found! on {}".format(hex(self.cursor)))
        self.cursor += 2
        self.savecursor = self.cursor
        length = struct.unpack(">H", self.file.getBytes(self.cursor, 2))[0]
        endcursor = self.cursor + length
        self.cursor += 2
        while self.cursor != endcursor:
            inform = self.file.getBytes(self.cursor, 1)[0]
            inform = ( inform & 0x0F,"AC" if (0x10 & inform) else "DC")
            print("\tNum: {} Type: {}".format(*inform))
            self.cursor += 1
            amount = struct.unpack("B" * 16, self.file.getBytes(self.cursor, 16))
            print("\tAmount: {}".format(amount))
            self.cursor += 16
            symbols = []
            s = symbols
            i = 0
            p = 0
            while i < len(amount):
                if amount[i] != p:
                    self.cursor += 1
                    p += 1
                else:
                    i += 1
                    p = 0




def main():
    JPEG("test2.jpg")

if __name__ == "__main__":
    main()