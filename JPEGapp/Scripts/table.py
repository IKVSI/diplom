#!/usr/bin/python3

import psycopg2
import sys
import os

def createtable(cursor, component, tp):
    cursor.execute(
        """
        CREATE TABLE jpeg."{}_{}" (
            category smallint primary key,
            frequency numeric
        )
        """.format(component, tp)
    )
    mx = 256 if tp == "AC" else 16
    cursor.executemany("INSERT INTO jpeg.\"{}_{}\" VALUES (%s, %s)".format(component, tp), list([(i, 0)for i in range(mx)]))

def checktable(cursor, component, tp):
    cursor.execute(
        "SELECT EXISTS(SELECT * FROM information_schema.tables WHERE table_schema = 'jpeg' AND table_name = '{}_{}')".format(
            component, tp), ())
    return cursor.fetchone()[0]

def addtable(cursor, component, tp, dct):
    for i in dct:
        cursor.execute("SELECT frequency FROM jpeg.\"{}_{}\" WHERE category = %s".format(component, tp), (i,))
        c = cursor.fetchone()[0]
        c += dct[i]
        cursor.execute("UPDATE jpeg.\"{}_{}\" SET frequency = %s WHERE category = %s".format(component, tp), (c, i))

def saveFile(cursor):
    pass


def main():
    with psycopg2.connect(
        dbname='cloud',
        user='kvs',
        password='Fkmnfbh9',
        host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            cursor.execute("SELECT MIN(fileid) FROM jpeg.files")
            mn = 631
            cursor.execute("SELECT MAX(fileid) FROM jpeg.files")
            mx = cursor.fetchone()[0]
            print(mn, mx)
            t = 0
            for i in range(mn ,mx):
                cursor.execute("SELECT file, hash, fileid FROM jpeg.files WHERE fileid = %s", (i,))
                data = cursor.fetchone()
                if data:
                    h = data[1]
                    id = data[2]
                    data = bytes(data[0])
                    print("FileID:", id, "Hash:", h)
                    with open("temp.jpg", 'wb') as fout:
                        fout.write(data)
                    print("\tSaved")
                    os.system("jpegstats.exe temp.jpg")
                    print("\tGetStats")
                    os.remove("temp.jpg")
                    with open("temp.jpg.stats") as fin:
                        data = fin.readline()
                        while data:
                            component, tp = data[:-1].split(' ')
                            data = eval(fin.readline())
                            if not checktable(cursor, component, tp):
                                createtable(cursor, component, tp)
                            addtable(cursor, component, tp, data)
                            print("\tUPDATE {}_{} TABLE!".format(component, tp))
                            data = fin.readline()

if __name__ == "__main__":
    main()