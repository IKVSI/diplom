#!/usr/bin/python3

import psycopg2
import sys

def main():
    SIZE = int(sys.argv[1])
    if SIZE < 16 or SIZE > 24:
        sys.exit(1)
    fout = open("codingtables{}".format(SIZE), "w")
    with psycopg2.connect(
        dbname='cloud',
        user='kvs',
        password='Fkmnfbh9',
        host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            cursor.execute("SELECT table_name FROM information_schema.tables WHERE (table_schema = 'jpeg') AND (table_name != 'files')")
            tablenames = list(i[0] for i in cursor.fetchall())
            for tablename in tablenames:
                cursor.execute("SELECT * FROM jpeg.\"{}\" ORDER BY frequency DESC".format(tablename))
                dct = [(int(i[1]), i[0]) for i in cursor.fetchall()]
                K = [(i[0], [i[1]])for i in dct]
                S = []
                X = len(dct) - 1
                d0 = 0
                x = 1
                while (x <= X):
                    x <<= 1
                    d0 += 1
                x >>= 1
                d0 -= 1
                D = [i for i in range(d0, -SIZE - 1 , -1)]
                M = []
                tempx = X
                while (tempx != 0):
                    if (tempx >= x):
                        tempx -= x
                        M.append(1)
                    else:
                        M.append(0)
                    x >>= 1
                for i in range(len(M), len(D)):
                    M.append(0)
                L = {}
                temp = [j for j in range(len(K))]
                for i in D:
                    L[i] = []
                    if i < 0:
                        L[i] = temp[:]
                while M[0]:
                    d = D[-1]
                    m = M[-1]
                    l = L[d]
                    if m:
                        S.append(l.pop())
                        M[-1] = 0
                    else:
                        while(len(l) > 1):
                            a = l.pop()
                            b = l.pop()
                            ka = K[a]
                            kb = K[b]
                            t = ka[1].copy()
                            t.extend(kb[1])
                            t = ((ka[0] + kb[0]), t)
                            l1 = L[d+1]
                            for i in range(len(l1)):
                                if K[l1[i]][0] < t[0]:
                                    break
                                elif (K[l1[i]][0] == t[0]) and (len(K[l1[i]][1]) > len(t[1])):
                                    break
                            l1.insert(i, len(K))
                            K.append(t)
                        D.pop()
                        M.pop()
                P = {i[1]: 0 for i in dct}
                for i in S:
                    for j in K[i][1]:
                        P[j] += 1
                counts = [0 for i in range(SIZE)]
                symbols = []
                for i in range(SIZE):
                    for j in P:
                        if P[j] == (i + 1):
                            counts[i] += 1
                            symbols.append(j)

                fout.write("{}\n".format(SIZE))
                tablename = tablename.replace("_", " ")
                print(tablename, "COUNTS:", sum(counts))
                fout.write("{}\n".format(tablename))
                counts.extend(symbols)
                fout.write("{}\n".format(" ".join(map(str, counts))))




if __name__ == "__main__":
    main()