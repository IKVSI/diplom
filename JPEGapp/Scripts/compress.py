#!/usr/bin/python3

import sys
import os
import shutil

def coding(dct, bitlength):
    K = []
    for i in dct:
        f = int(i[0])
        f = f << 1 if f else 1
        s = i[1]
        K.append((f, [s]))
    K.append((0, [256]))
    X = len(K) - 1
    d0 = 0
    x = 1
    while (x <= X):
        d0 += 1
        x <<= 1
    d0 -= 1
    x >>= 1
    D = [i for i in range(d0, -(bitlength + 1), -1)]
    M = []
    tempx = X
    while (tempx):
        if (tempx >= x):
            tempx -= x
            M.append(1)
        else:
            M.append(0)
        x >>= 1
    for i in range(len(M), len(D)):
        M.append(0)
    L = {}
    temp = [i for i in range(len(K))]
    for i in D:
        if i < 0:
            L[i] = temp.copy()
        else:
            L[i] = []
    S = []
    while M[0]:
        d = D[-1]
        m = M[-1]
        if m:
            S.append(L[d][-1])
            L[d].pop()
            M[-1] = False
        else:
            while (len(L[d]) > 1):
                a = L[d].pop()
                b = L[d].pop()
                t = (K[a][0] + K[b][0], K[a][1].copy())
                t[1].extend(K[b][1])
                for i in range(len(L[d + 1])):
                    if K[ L[d + 1][i] ][0] < t[0]:
                        break
                    elif (K[ L[d + 1][i] ][0] == t[0]) and (len(K[ L[d + 1][i] ][1]) <= len(t[1])):
                        break
                L[d + 1].insert(i, len(K))
                K.append(t)
            D.pop()
            M.pop()
    P = {}
    for i in dct:
        P[i[1]] = 0
    for i in S:
        weight, sym = K[i]
        for i in sym:
            if i in P:
                P[i] += 1
    counts = [0 for i in range(bitlength)]
    symbols = []
    for i in range(bitlength):
        for j in sorted(P.items(), key=lambda item: (item[1], item[0])):
            if j[1] == i + 1:
                counts[i] += 1
                symbols.append(j[0])
    return counts, symbols

def compressFile(file, codingfile, compressfolder):
    E = os.system("jpegcompress.exe {} {} > NUL".format(codingfile, file))
    if E:
        print("Something goes wrong!")
        sys.exit(2)
    else:
        shutil.move(file+'.compressed', "{}/{}".format(compressfolder, os.path.basename(file+'.compressed')))


def getStats(file):
    E = os.system("jpegstats.exe {} > NUL".format(file))
    if E:
        print("\tBad File!")
        return None
    else:
        stats = {}
        with open("{}.stats".format(file), "r") as fin:
            key = fin.readline()
            while key:
                stats[key[:-1]] = eval(fin.readline())
                key = fin.readline()
        os.remove("{}.stats".format(file))
        return stats


def main(merge = False):
    try:
        folder = os.path.abspath(sys.argv[1])
        sys.argv[2] = int(sys.argv[2])
    except ValueError:
        pass
    if (not os.path.isdir(folder)) or (sys.argv[2] not in [16, 24]):
        print("Usage: python3 compress.py [folder] 16/24")
        sys.exit(1)
    jpegfolder = "{}/{}".format(folder, "JPEG")
    compressfolder = "{}/{}".format(folder, "COMPRESS")
    if os.path.exists(jpegfolder):
        shutil.rmtree(jpegfolder)
    if os.path.exists(compressfolder):
        shutil.rmtree(compressfolder)
    os.mkdir(jpegfolder)
    os.mkdir(compressfolder)
    filecounter = 0
    allstats = {}
    p = 0
    flist = os.listdir(folder)
    for i in flist:
        p += 1
        file = "{}/{}".format(folder, i)
        if os.path.isfile(file):
            print("{} / {} Check file \"{}\"".format(p, len(flist), file))
            jpegfile = "{}/{}.jpg".format(jpegfolder, str(filecounter))
            shutil.copyfile(file, jpegfile)
            stats = getStats(jpegfile)
            if stats:
                filecounter += 1
                for i in stats:
                    if i not in allstats:
                        allstats[i] = {}
                    for j in stats[i]:
                        if j not in allstats[i]:
                            allstats[i][j] = 0
                        allstats[i][j] += stats[i][j]
    jflist = os.listdir(jpegfolder)
    print("Number of files: ", len(jflist))
    codingtable = {}
    with open("{}/{}".format(compressfolder, "coding.stats"), "w") as fout:
        for i in allstats:
            print(i, file=fout)
            print(allstats[i], file=fout)
    if merge:
        for i in allstats:
            if "Cb" in i:
                if "DC" in i:
                    cbdc = allstats[i]
                else:
                    cbac = allstats[i]
            if "Cr" in i:
                if "DC" in i:
                    crdc = allstats[i]
                else:
                    crac = allstats[i]
        dc = {}
        for i in crdc:
            if i not in dc:
                dc[i] = 0
            dc[i] += crdc[i]
        for i in cbdc:
            if i not in dc:
                dc[i] = 0
            dc[i] += cbdc[i]
        ac = {}
        for i in crac:
            if i not in ac:
                ac[i] = 0
            ac[i] += crac[i]
        for i in cbac:
            if i not in ac:
                ac[i] = 0
            ac[i] += cbac[i]
        for i in allstats:
            if ("Cb" in i) or ("Cr" in i):
                if "DC" in i:
                    allstats[i] = dc
                else:
                    allstats[i] = ac
    for i in allstats:
        dct = [(i[1], i[0]) for i in sorted(allstats[i].items(), key=lambda item: item[1])]
        dct.reverse()
        codingtable[i] = coding(dct, sys.argv[2])
    codingfile = "{}/{}".format(compressfolder, "coding.table")
    with open(codingfile, "w") as fout:
        print(sys.argv[2], file=fout)
        for i in codingtable:
            print(i, file=fout)
            print(' '.join(map(str, codingtable[i][0])), file=fout)
            print(' '.join(map(str, codingtable[i][1])), file=fout)

    counter = 0
    for i in os.listdir(jpegfolder):
        file = "{}/{}".format(jpegfolder, i)
        compressFile(file, codingfile, compressfolder)
        counter += 1
        print("{} / {}".format(counter, len(jflist)))


if __name__ == "__main__":
    main()