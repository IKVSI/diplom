SIZE = 16
fout = open("codingtablesx{}".format(SIZE), "w")
dct = [ ( 6694, 0 ), ( 22843, 1 ), ( 14188, 2 ), ( 8168, 3 ), ( 4562, 4 ), ( 2195, 5 ), ( 837, 6 ), ( 246, 7 ), ( 12, 8 ), ( 6761, 17 ), ( 2172, 18 ), ( 697, 19 ), ( 202, 20 ), ( 61, 21 ), ( 37, 22 ), ( 14, 23 ), ( 2894, 33 ), ( 506, 34 ), ( 101, 35 ), ( 16, 36 ), ( 5, 37 ), ( 1723, 49 ), ( 182, 50 ), ( 44, 51 ), ( 8, 52 ), ( 2, 53 ), ( 1064, 65 ), ( 110, 66 ), (33, 67 ), ( 7, 68 ), ( 1, 69 ), ( 918, 81 ), ( 84, 82 ), ( 21, 83 ), ( 4, 84 ), ( 652, 97 ), ( 41, 98 ), ( 6, 99 ), ( 1, 100 ), ( 456, 113 ), ( 22, 114 ), ( 3, 115 ), ( 354, 129 ), ( 18, 130 ), ( 2, 131 ), ( 387, 145 ), ( 24, 146 ), ( 2, 147 ), ( 213, 161 ), ( 10, 162 ), ( 172, 177 ), ( 5, 178 ), ( 108, 193 ), ( 1, 194 ), ( 1, 195 ), ( 82, 209 ), ( 2, 210 ), ( 36, 225 ), ( 50, 240 ), ( 27, 241 ) ]
dct.sort()
dct.reverse()
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
print(D)
print(M)
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
                if K[l1[i]][0] <= t[0]:
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
tablename = "X"
print(tablename, "COUNTS:", sum(counts))
fout.write("{}\n".format(tablename))
counts.extend(symbols)
fout.write("{}\n".format(" ".join(map(str, counts))))