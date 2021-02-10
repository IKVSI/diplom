#!/usr/bin/python3

import os
import sys
import shutil

def main():
    try:
        folder = os.path.abspath(sys.argv[1])
    except ValueError:
        pass
    if (not os.path.isdir(folder)):
        print("Usage: python3 decompress.py [folder]")
        sys.exit(1)
    codingfile = "{}/{}".format(folder, "coding.table")
    flist = os.listdir(folder)
    flist = [i for i in flist if "coding" not in i]
    decompressfolder = "{}/{}".format(folder, "DECOMPRESS")
    if os.path.exists(decompressfolder):
        shutil.rmtree(decompressfolder)
    os.mkdir(decompressfolder)
    p = 0
    for i in flist:
        p += 1
        file = "{}/{}".format(folder, i)
        if os.path.isfile(file):
            print("{} / {} Decompress \"{}\"".format(p, len(flist), file))
            E = os.system("jpegdecompress.exe {} {} >NUL".format(codingfile, file))
            if E:
                print("Something went wrong!")
            else:
                decompressedfile = os.path.basename(file)
                decompressedfile = decompressedfile.replace("compressed", "")
                if ".jpg" == decompressedfile[-3:]:
                    decompressedfile += ".jpg"
                decompressedfile = "{}/{}".format(decompressfolder, decompressedfile)
                shutil.move(file+".jpg", decompressedfile)

if __name__ == "__main__":
    main()