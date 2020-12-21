#!/usr/bin/python3

import argparse
import os

ZIGZAG = [
    [ 0,  1,  5,  6, 14, 15, 27, 28],
    [ 2,  4,  7, 13, 16, 26, 29, 42],
    [ 3,  8, 12, 17, 25, 30, 41, 43],
    [ 9, 11, 18, 24, 31, 40, 44, 53],
    [10, 19, 23, 32, 39, 45, 52, 54],
    [20, 22, 33, 38, 46, 51, 55, 60],
    [21, 34, 37, 47, 50, 56, 59, 61],
    [35, 36, 48, 49, 57, 58, 62, 63]
]

def strbytes(b):
    r = []
    for i in b:
        s = bin(i)[2:]
        s = "0"*(8 - len(s)) + s
        r.append(s)
    return ' '.join(r)

def zigzag(table):
    vector = [0 for i in range(64)]
    for i in range(8):
        for j in range(8):
            vector[ZIGZAG[i][j]] = table[i][j]
    return vector

def readTables(filename):
    tables = []
    with open(filename, "r") as fin:
        print(fin.readline())
        table = []
        for i in fin:
            if i != "\n":
                i = i.rstrip().lstrip()
                table.append(list(map(int, i.split())))
            if len(table) == 8:
                tables.append(table)
                table = []
    return tables

def is_valid_file(parser, arg):
    if not os.path.exists(arg):
        parser.error("The file {} does not exist!".format(arg))
    else:
        return arg

def golombnum(num):
    bts = bytearray()
    bts.append(0)
    if num == 0:
        return '0'
    minus = '0' if (num < 0) else '1'
    num = abs(num)
    num = bin(num)[2:]
    return '1' * len(num) + '0' + minus + num[1:]
    return bts, bits

def setBit(bit, bts, bits):
    if bit:
        bts[-1] += 1 << (7 - bits)
    bits += 1
    if (bits == 8):
        bts.append(0)
        bits = 0
    return bits

def golomb(tables):
    bts = bytearray()
    bts.append(0)
    bits = 0
    t = 0
    for i in map(zigzag, tables):
        print("Num: {}".format(t))
        t += 1
        for j in range(len(i) - 1, -1, -1):
            if i[j]:
                break
        i = i[:j + 1]
        j = bin(j)[2:]
        j = "0"*(6 - len(j))+j
        for k in j:
            if k == '0':
                bits = setBit(0, bts, bits)
            else:
                bits = setBit(1, bts, bits)
        for num in i:
            if num:
                minus, num = (0, bin(-num)) if (num < 0) else (1, bin(num))
                for j in range(len(num) - 2):
                    bits = setBit(1, bts, bits)
                bits = setBit(0, bts, bits)
                bits = setBit(minus, bts, bits)
                for j in range(3, len(num)):
                    if num[j] == '0':
                        bits = setBit(0, bts, bits)
                    else:
                        bits = setBit(1, bts, bits)
            else:
                bits = setBit(0, bts, bits)
    print("Size in bytes: {}".format(len(bts)))

def main():
    parser = argparse.ArgumentParser(description="JPEG parsing Module")
    parser.add_argument(
        "filename",
        help = "Tables file",
        type = lambda x: is_valid_file(parser, x)
        )
    args = parser.parse_args()
    tables = readTables(args.filename)
    golomb(tables)

if __name__ == "__main__":
    main()