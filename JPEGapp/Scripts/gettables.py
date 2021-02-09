#!/usr/bin/python3

import psycopg2
import sys

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
            M[-1] = False
        else:
            while (len(L[d]) > 1):
                a = L[d].pop(-1)
                b = L[d].pop(-1)
                t = (K[a][0] + K[b][0], K[a][1].copy())
                t[1].extend(K[b][1])
                for i in range(len(L[d + 1])):
                    if K[L[d + 1][i]][0] < t[0]:
                        break
                    elif (K[L[d + 1][i]][0] == t[0]) and (len(K[L[d + 1][i]][1]) < len(t[1])):
                        break
                L[d + 1].insert(i, len(K))
                K.append(t)
            D.pop(-1)
            M.pop(-1)
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
        for j in P:
            if P[j] == i + 1:
                counts[i] += 1
                symbols.append(j)
    return counts, symbols

def main(cursor, bitlength):
    cursor.execute("""
    SELECT table_name FROM information_schema.tables
    WHERE (table_schema = 'jpeg')
        AND (table_name != 'files')
        AND (table_name != 'filesinstats')
    """)
    tables = {}
    for i in cursor.fetchall():
        i = i[0]
        name = ' '.join(i.upper() for i in i.split('_'))
        name = name[0] + name[1].lower() + name[2:]
        print(name)
        cursor.execute("SELECT frequency, category FROM jpeg.{} ORDER BY frequency DESC".format(i))
        dct = cursor.fetchall()
        tables[name] = coding(dct, bitlength)
    with open("coding{}.table".format(bitlength), "w") as fout:
        fout.write("{}\n".format(bitlength))
        for i in tables:
            print(i, file=fout)
            print(" ".join(map(str, tables[i][0])), file=fout)
            print(" ".join(map(str, tables[i][1])), file=fout)




if __name__ == "__main__":
    with psycopg2.connect(
        dbname='cloud',
        user='kvs',
        password='Fkmnfbh9',
        host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            main(cursor, int(sys.argv[1]))