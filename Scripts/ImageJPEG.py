#!/usr/bin/python3
import os

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


def main():
    pass

if __name__ == "__main__":
    main()