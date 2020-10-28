#!/usr/bin/python3

import sys

def sequence(seq, graph, size, size2k, seqs=[]):
    if len(seq) == size2k:
        seqs.append(seq.copy())
    else:
        a = tuple(seq[-size:])
        if a not in graph:
            seq.append(0)
            graph[a] = 0
            sequence(seq, graph, size, size2k, seqs)
            seq.pop()
            graph.pop(a)
            seq.append(1)
            graph[a] = 1
            sequence(seq, graph, size, size2k, seqs)
            seq.pop()
            graph.pop(a)
    return seqs

def main():
    size = int(sys.argv[1])
    #size = int(input())
    seq = [0]*size
    graph = {}
    seqs = sequence(seq, graph, size, 2**size+size)
    for i in seqs:
        print(i)
    print("Size seqs:", len(seqs))

if __name__ == "__main__":
    main()