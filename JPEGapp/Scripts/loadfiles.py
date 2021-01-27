#!/usr/bin/python3

import os
import sys
import psycopg2
from shutil import copyfile
import hashlib

def main():
    if (len(sys.argv) > 1):
        dirpath = os.path.abspath(sys.argv[1])
    else:
        sys.exit(1)
    print("DIR: \"{}\"".format(dirpath))
    if not os.path.isdir(dirpath):
        sys.exit(2)
    for i in os.listdir(dirpath):
        i = dirpath + "\{}".format(i)
        if os.path.isfile(i):
            print("File: \"{}\"".format(i))
            copyfile(i, "temp.jpg")
            os.system("jpegclear.exe temp.jpg > temp".format(i))
        if not (os.path.isfile("temp.jpg.clear.jpg")):
            continue
        with open("temp.jpg.clear.jpg", "rb") as fin:
            data = fin.read()
        h = str(hashlib.sha1(data).hexdigest())
        size = len(data)
        with psycopg2.connect(
                dbname='cloud',
                user='kvs',
                password='Fkmnfbh9',
                host='192.168.31.132'
        ) as conn:
            with conn.cursor() as cursor:
                cursor.execute("SELECT * FROM jpeg.files WHERE (hash = %s) AND (size = %s)", (h, size))
                if not cursor.fetchall():
                    print("File send!")
                    cursor.execute("INSERT INTO jpeg.files VALUES (DEFAULT, %s, %s, %s)", (size, data, h))
                    conn.commit()
                else:
                    print("File exists!")
        os.remove("temp.jpg")
        os.remove("temp.jpg.clear.jpg")
        os.remove("temp")

if __name__ == "__main__":
    main()