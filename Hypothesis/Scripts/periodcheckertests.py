#!/usr/bin/python3

import os

TEST = [
    "x1 V -x2",
    "-x1 V x2",
    #"x1 V -x2-x3",
    #"x2 V -x1-x3-x4",
    #"x1 V -x2-x3-x4-x5",
    "x1 V -x2-x3-x4-x5-x6",
    "x2 V -x1-x3-x4-x5-x6-x7"
    #"x1 V x2",
    #"x1x2 V x3x4",
    #"x1-x2 V -x3x4 V -x4-x5-x6 V x7-x1x2",
    #"x1Vx2",
    #"x1-x2V   -x3x4V-x4-x5-x6Vx7 -x1 x2"
]

def main():
    for i in TEST:
        os.system("python periodchecker.py {}".format(i))

if __name__ == "__main__":
    main()