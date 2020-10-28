#!/usr/bin/python3

import os

SIZE = 7
TEST = []
#TEST.append("x1 V -x2") # 8
#TEST.append("-x1 V x2") # 8
#TEST.append("x1 V -x2-x3") # 16
#TEST.append("x2 V -x1-x3-x4") # 32
#TEST.append("x1 V -x2-x3-x4-x5") # 64
#TEST.append("x1 V -x2-x3-x4-x5-x6") # 128
#TEST.append("x1 V x2x3 V -x4") # 32
#TEST.append("x1 V x2x3 V -x4-x5") # 64
#TEST.append("x1 V x2x4x6 V -x3-x5-x7") # 231
#TEST.append("x1x2 V -x3-x4") # 32
#TEST.append("x1x2 V x3x4 V x5x6 V -x7-x8") # 502
#TEST.append("x1x2 V x3x4 V x5x6 V -x7") # 216
#TEST.append("x1 V x2 V -x3") # 16
#TEST.append("x1x3 V x2x4 V x7x5 V -x6-x8")
#TEST.append("-x1-x2-x3 V x4 V x5 V x6x7x8")
#TEST.append("x1 V x3x5 V -x2-x4-x6") #128
#TEST.append("x2 V x6 V -x5-x4-x3-x1") #128
TEST.append("x3 V x7 V x5 V -x6-x4-x2-x1")

def partitions(size, minimum = 0):
    minimum = minimum if minimum else size
    parts = []
    for i in range(1, minimum + 1):
        a = [i]
        if (size - i):
            for j in partitions(size-i, min(size-i, i)):
                j.extend(a)
                parts.append(j)
        else:
            parts.append(a)
    return parts

def perm(mass):
    permmass = []
    if len(mass) == 2:
        permmass.append(mass.copy())
        permmass.append(mass.copy())
        permmass[0].reverse()
    for i in range(len(mass)):
        a = [mass[i]]
        mass.pop(i)
        for j in perm(mass):
            j.extend(a)
            permmass.append(j)
        mass.insert(i, a[0])
    return permmass

def genTEST(part, perm):
    s = []
    k = 0
    for i in range(len(part)):
        a = []
        if i == len(part) - 1:
            for j in range(part[i]):
                a.append('-x{}'.format(perm[k]))
                k += 1
        else:
            for j in range(part[i]):
                a.append('x{}'.format(perm[k]))
                k += 1
        s.append(' '+''.join(a)+' ')
    s = 'V'.join(s)
    s = s.lstrip()
    s = s.rstrip()
    return s

def genTESTs(size):
    parts = partitions(size)
    permutation = perm([i for i in range(1, size+1)])
    for i in parts:
        for j in permutation:
            TEST.append(genTEST(i, j))


def main():
    genTESTs(SIZE)
    for i in range(len(TEST)):
        temp = '({}/{}) '.format(i, len(TEST))+TEST[i]
        print(temp+" :\n", end='')
        os.system("echo {}:  >> TEST{}.txt".format(temp, SIZE))
        #print("python periodchecker.py {} >> TEST{}.txt".format(TEST[i], SIZE))
        os.system("python periodchecker.py {} >> TEST{}.txt".format(TEST[i], SIZE))
        os.system("echo. >> TEST{}.txt".format(SIZE))

def main2():
    for i in range(len(TEST)):
        os.system(
            "python periodchecker.py {}".format(TEST[i]))

if __name__ == "__main__":
    main()