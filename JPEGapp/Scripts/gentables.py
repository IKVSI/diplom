#!/usr/bin/python3
import psycopg2
import os

def loadtable(name, freq, cursor):
    cursor.execute("""
                CREATE TABLE IF NOT EXISTS jpeg.{} (
                    category smallint PRIMARY KEY,
                    frequency decimal
                );
            """.format(name))
    cursor.execute("SELECT category, frequency FROM jpeg.{}".format(name))
    temp = cursor.fetchall()
    ok = True
    if temp:
        temp = {i[0]: i[1] for i in temp}
        for i in freq:
            temp[i] += freq[i]
        ok = False
    else:
        temp = {}
        for i in range(16 if "dc" in name else 256):
            if i in freq:
                temp[i] = freq[i]
            else:
                temp[i] = 0
    for i in temp:
        if ok:
            cursor.execute("INSERT INTO jpeg.{} VALUES (%s, %s)".format(name), (i, temp[i]))
        else:
            cursor.execute("UPDATE jpeg.{} SET frequency = %s WHERE category = %s".format(name), (temp[i], i))


def main(conn, cursor):
    cursor.execute("""
            CREATE TABLE IF NOT EXISTS jpeg.filesinstats (
                id bigint PRIMARY KEY,
                used boolean
            );
        """)
    conn.commit()
    cursor.execute("SELECT id FROM jpeg.files")
    fileids = cursor.fetchall()
    cursor.execute("SELECT id, used FROM jpeg.filesinstats")
    used = {i[0]:i[1] for i in cursor.fetchall()}
    for i in range(len(fileids)):
        print("{} / {}".format(i, len(fileids)))
        if i in used:
            if used[i]:
                print("File allready in stats!")
                continue
        else:
            cursor.execute("INSERT INTO jpeg.filesinstats VALUES (%s, %s)", (i, False))
        cursor.execute("SELECT filedata FROM jpeg.files WHERE (id = %s)", fileids[i])
        data = bytes(cursor.fetchone()[0])
        with open("temp.jpg", "wb") as fout:
            fout.write(data)
        E = os.system("./jpegstats temp.jpg >> temp.log")
        if E:
            print("Something went wrong!")
        else:
            with open("temp.jpg.stats", "r") as fin:
                row = fin.readline()
                while row:
                    name = '_'.join((i.lower() for i in row[:-1].split(' ')))
                    freq = eval(fin.readline())
                    loadtable(name, freq, cursor)
                    row = fin.readline()
            cursor.execute("UPDATE jpeg.filesinstats SET used = %s WHERE id = %s", (True, i))
            conn.commit()
        pass
    os.remove("temp.jpg")
    os.remove("temp.jpg.stats")
    os.remove("temp.log")

if __name__ == "__main__":
    with psycopg2.connect(
            dbname='cloud',
            user='kvs',
            password='Fkmnfbh9',
            host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            main(conn, cursor)