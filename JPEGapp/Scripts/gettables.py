#!/usr/bin/python3

import psycopg2

def main():
    with psycopg2.connect(
        dbname='cloud',
        user='kvs',
        password='Fkmnfbh9',
        host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            cursor.execute("SELECT table_name FROM information_schema.tables WHERE (table_schema = 'jpeg') AND (table_name != 'files')")
            tablenames = list(i[0] for i in cursor.fetchall())
            for i in tablenames:
                cursor.execute("SELECT * FROM jpeg.\"{}\"".format(i))
                dct = [(int(i[1]), i[0]) for i in cursor.fetchall()]
                dct.sort()
                L = []
                N = len(dct)
                for i in dct:
                    for j in range(16):
                        L.append((i[0], 1<<j, i[1]))
                fullprice = (N - 1) * (2 << 16)
                print(fullprice * len(L))
                break




if __name__ == "__main__":
    main()