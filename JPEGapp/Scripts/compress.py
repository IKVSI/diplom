#!/usr/bin/python3

import sys
import os
from shutil import copyfile

def compressfile(filename, fs, fcs):
    name = "temp.jpg"
    copyfile(filename, name)
    curentfs = os.path.getsize(name)
    E = os.system("./jpegcompress coding24.table temp.jpg >> /dev/null")
    if (E):
        print("Bad File!")
    else:
        curentfcs = os.path.getsize(name+".compressed")
        fs += curentfs
        fcs += curentfcs
        print("File:", curentfs, "CFile:", curentfcs, "All:", fs, "CAll:", fcs)
    return fs, fcs

def main():
    folder = os.path.abspath(sys.argv[1])
    print(folder)
    fs = 0
    fcs = 0
    if os.path.exists(folder):
        if os.path.isfile(folder):
            fs, fcs = compressfile(folder, fs, fcs)
        elif os.path.isdir(folder):
            d = os.listdir(folder)
            for i in range(len(d)):
                print("{} / {}".format(i, len(d)))
                filepath = "{}/{}".format(folder, d[i])
                if os.path.isfile(filepath):
                    fs, fcs = compressfile(filepath, fs, fcs)


if __name__ == "__main__":
    main()