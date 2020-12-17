#!/usr/bin/python3

import sys
import argparse

def zigzag(matrix, step=False):
    x = 0
    y = 0
    vector = []
    while((x != 7) or (y != 7)):
        vector.append(matrix[y][x])
        if step:
            x -= 1
            y += 1
            if (y == 8):
                y -= 1
                x += 2
                step = not step
            elif (x == -1):
                x += 1
                step = not step
        else:
            x += 1
            y -= 1
            if (x == 8):
                x -= 1
                y += 2
                step = not step
            elif (y == -1):
                y += 1
                step = not step
    vector.append(matrix[y][x])
    for i in range(len(vector)-1, -1, -1):
        if vector[i]:
            break
    vector = vector[:i+1]
    return vector

def antizigzag(vector, step=False):
    matrix = [[0 for i in range(8)] for j in range(8)]
    x = 0
    y = 0
    for i in vector:
        matrix[y][x] = i
        if step:
            x -= 1
            y += 1
            if (y == 8):
                y -= 1
                x += 2
                step = not step
            elif (x == -1):
                x += 1
                step = not step
        else:
            x += 1
            y -= 1
            if (x == 8):
                x -= 1
                y += 2
                step = not step
            elif (y == -1):
                y += 1
                step = not step
    return matrix

def defaultgolomb(num):
    if num == 0:
        return '0'
    minus = '0' if (num < 0) else '1'
    num = abs(num)
    num = bin(num)[2:]
    return '1' * len(num) + '0' + minus + num[1:]

def encode():
    matrix = [list(map(int, input().split())) for i in range(8)]
    vector1 = zigzag(matrix)
    vector2 = zigzag(matrix, True)
    s = []
    if len(vector1) < len(vector2):
        s.append("0")
        vector = vector1
    else:
        s.append("1")
        vector = vector2
    length = bin(len(vector)-1)[2:]
    length = (6-len(length)) * '0' + length
    s.append(length)
    for i in vector:
        s.append(defaultgolomb(i))
    s = ''.join(s)
    print(s)

def antidefaultgolomb(size, s):
    vector = []
    while (size):
        if (s[0] == '0'):
            vector.append(0)
            s = s[1:]
        else:
            i = 0
            while (s[i] != '0'):
                i += 1
            minus = -1 if (s[i+1] == '0') else 1
            num = int('1'+s[i+2:i+i+1], base=2) * minus
            vector.append(num)
            s = s[i+i+1:]
        size -= 1
    return vector

def decode():
    s = input()
    step = False if s[0] == "0" else True
    s = s[1:]
    length = int(s[:6], base = 2) + 1
    vector = antidefaultgolomb(length, s[6:])
    matrix = antizigzag(vector, step)
    for i in matrix:
        for j in i:
            print(j, end=' ')
        print()



def main():
    if 'e' in sys.argv:
        encode()
    if 'd' in sys.argv:
        decode()

if __name__ == "__main__":
    main()