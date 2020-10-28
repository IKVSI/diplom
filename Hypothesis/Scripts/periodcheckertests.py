#!/usr/bin/python3

import os

TEST = []
#TEST.append("x1 V -x2") #8
#TEST.append("-x1 V x2") #8
#TEST.append("x1 V -x2-x3") #16
#TEST.append("x2 V -x1-x3-x4") #32
#TEST.append("x1 V -x2-x3-x4-x5") #64
#TEST.append("x1 V -x2-x3-x4-x5-x6") #128
#TEST.append("x1 V x2x3 V -x4") #32
#TEST.append("x1 V x2x3 V -x4-x5") #64
#TEST.append("x1 V x2x4x6 V -x3-x5-x7") #231
#TEST.append("x1x2 V -x3-x4") #32
#TEST.append("x1x2 V x3x4 V x5x6 V -x7-x8") #502
#TEST.append("x1x2 V x3x4 V x5x6 V -x7") #216
#TEST.append("x1 V x2 V -x3") #16
TEST.append("x1x3 V x2x4 V x7x5 V -x6-x8")
TEST.append("-x1-x2-x3 V x4 V x5 V x6x7x8")


def genTEST(size):
    pass

def main():
    for i in TEST:
        os.system("python periodchecker.py {}".format(i))

if __name__ == "__main__":
    main()