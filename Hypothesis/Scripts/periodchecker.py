#!/usr/bin/python3

import sys
import re

class Function():
    __patern = re.compile(r"-?x\d+")
    __replace = {'0': '₀', '1': '₁', '2': '₂', '3': '₃', '4': '₄',
    '5': '₅', '6': '₆', '7': '₇', '8': '₈','9': '₉', 'y': 'x¯'}

    def __init__(self, strform):
        self.__size = 0
        for i in range(len(strform)):
            strform[i] = self.__patern.findall(strform[i])
            strform[i] = map(
                lambda x: int(x[1:]) if x[0] != '-' else -int(x[2:]),
                strform[i]
            )
            strform[i] = tuple(strform[i])
            self.__size = max(max(map(abs, strform[i])), self.__size)
        self.__generation = strform
        self.__genstr()

    def __genstr(self):
        self.__strform = "G( "+"{}, "*self.__size
        self.__strform = self.__strform.format(
            *('x'+str(i) for i in range(1, self.__size + 1))
        )
        self.__strform = self.__strform[:-2] + " ) = "
        temp = []
        for i in self.__generation:
            product = ['y' + str(-j) if j < 0 else 'x' + str(j) for j in i]
            temp.append('&'.join(product))
        self.__strform += " V ".join(temp)
        for i in self.__replace:
            self.__strform = self.__strform.replace(i, self.__replace[i])

    def __call__(self, *args, **kwargs):
        disjunc = 0
        for i in self.__generation:
            product = 1
            for j in i:
                product &= 1 ^ args[-j-1] if j < 0 else args[j-1]
            disjunc |= product
        return disjunc

    def __str__(self):
        return self.__strform

    def getSize(self):
        return self.__size

    def checkperiod(self, *target):
        target = tuple(target)
        sequence = list(target)
        graph = {}
        start = target
        while start not in graph:
            sequence.append(start[0] ^ self(*start[1:]))
            nxt = tuple(sequence[-len(target):])
            graph[start] = nxt
            start = nxt
        period = 1
        end = start
        period = ''.join(map(str, start))
        start = graph[start]
        while start != end:
            period += str(start[-1])
            start = graph[start]
        period = period[:-self.__size]
        result = ''.join(map(str, sequence[:-self.__size-1]))+' ('+period+')'
        return len(period), result

    def genTable(self):
        spacevarlen = len(str(self.__size)) + 1
        spaceformlen = len(str(self))
        table = '┌'
        for i in range(self.__size+1):
            table += '─' * (spacevarlen + 2) + '┬'
        table += '─' * (spaceformlen + 2) + "┐\n"
        table += '│'
        table += ' '*(spacevarlen) +"№ │"
        for i in range(1, self.__size + 1):
            vr = ('x'+str(i))
            table += ' ' * (spacevarlen - len(vr) + 1) + vr + " │"
        for i in self.__replace:
            table = table.replace(i, self.__replace[i])
        table += ' ' + str(self) + " │\n"
        table += '├'
        for i in range(self.__size + 1):
            table += '─' * (spacevarlen + 2) + '┼'
        table += '─' * (spaceformlen + 2) + "┤\n"
        j = 0
        for i in permutation(self.__size):
            table += '│'
            table += ' ' * (spacevarlen - len(str(j)) + 1) + str(j) + " │"
            for k in range(self.__size):
                vr = str(i[k])
                table += ' ' * (spacevarlen - len(vr) + 1) + vr + " │"
            result = ' ' * spaceformlen
            result = result[:len(result) // 2] + str(self(*i)) + result[len(result) // 2+1:]
            table += ' ' + result + " │\n"
            j += 1
        table += '└'
        for i in range(self.__size + 1):
            table += '─' * (spacevarlen + 2) + '┴'
        table += '─' * (spaceformlen + 2) + "┘\n"
        return table

def permutation(size):
    for i in range(2**size):
        i = format(i, 'b')
        i = tuple(map(int, '0'*(size - len(i)) + i))
        yield i

def main(argv):
    strform = ''.join(argv[1:]).replace(' ', '').split('V')
    funcG = Function(strform)
    #fout = open('output.txt', 'w', encoding='utf-8')
    print(funcG.checkperiod(*[0]*(funcG.getSize()+1)))



if __name__ == "__main__":
    sys.exit(main(sys.argv))
