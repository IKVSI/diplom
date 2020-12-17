#!/usr/bin/python3
import os

TEST = [
    "golombzigzag.py",
    "golomb2plus.py",
    "golombplus.py"
]

def run(prog):
    sum = 0
    N = 0
    with open("matrices.txt") as fin:
        line = ' '
        while(line):
            with open("t.{}.main".format(prog), 'w') as fout:
                size = 8
                for i in fin:
                    fout.write(i)
                    size -= 1
                    if not size:
                        break
            line = fin.readline()
            os.system("type t.{}.main | python {} e > t.{}.enc".format(prog, prog, prog))
            with open("t.{}.enc".format(prog)) as encfin:
                s = len(encfin.readline()[:-1])
            os.system("type t.{}.enc | python {} d > t.{}.dec".format(prog, prog, prog))
            r = os.system("FC t.{}.main t.{}.dec > nul".format(prog, prog))
            if r != 0:
                print("WRONG!!!")
            sum += s
            N += 1
            print("algo: {} number: {} length: {} allength: {}".format(prog, N, s, sum))
    with open("result", 'a') as fout:
        fout.write("algo: {} number: {} length: {} allength: {}\n".format(prog, N, s, sum))



def main():
    run(TEST[0])

if __name__ == "__main__":
    main()