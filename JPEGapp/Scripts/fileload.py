#!/usr/bin/python3

import os
import sys
import psycopg2
from shutil import copyfile
import hashlib

def loadfile(filename, conn, cursor):
    jpegfile = "{}/temp.jpg".format(os.getcwd())
    copyfile(filename, jpegfile)
    print("File {} copied!".format(jpegfile))
    with open(jpegfile, "br") as fin:
        data = fin.read(2)
    if data == b"\xFF\xD8":
        E = os.system("./jpegclear \"{}\" >> temp.log".format(jpegfile))
        if E:
            print("Bad file!")
        else:
            jfc = "{}{}".format(jpegfile, ".clear.jpg")
            print("File {} cleared!".format(jfc))
            with open(jfc, "br") as fin:
                data = fin.read()

            filesize = len(data)
            hash = hashlib.sha1(data).hexdigest()
            cursor.execute("SELECT COUNT(*) FROM jpeg.files WHERE (hash = %s) AND (filesize = %s)", (hash, filesize))
            if cursor.fetchone()[0]:
                print("File exists in database!")
            else:
                cursor.execute("INSERT INTO jpeg.files VALUES (DEFAULT, %s, %s, %s)", (hash, data, filesize))
                conn.commit()
                print("File sent!")
            os.remove(jfc)
            os.remove(jpegfile)
            print("Files removed!")


def main(conn, cursor):
    cursor.execute("""
        CREATE TABLE IF NOT EXISTS jpeg.files (
            id bigserial,
            hash text,
            filedata bytea,
            filesize bigint,
            PRIMARY KEY (hash, filesize)
        );
    """)
    conn.commit()
    folder = os.path.abspath(sys.argv[1])
    print(folder)
    if os.path.exists(folder):
        if os.path.isfile(folder):
            loadfile(folder)
        elif os.path.isdir(folder):
            d = os.listdir(folder)
            for i in range(len(d)):
                print("{} / {}".format(i, len(d)))
                filepath = "{}/{}".format(folder, d[i])
                if os.path.isfile(filepath):
                    loadfile(filepath, conn, cursor)
        os.remove("{}/{}".format(os.getcwd(), "temp.log"))




if __name__ == "__main__":
    with psycopg2.connect(
            dbname='cloud',
            user='kvs',
            password='Fkmnfbh9',
            host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            main(conn, cursor)