#!/usr/bin/python3

import psycopg2
import sys

def main():
    pass
    '''
    with psycopg2.connect(
        dbname='cloud',
        user='kvs',
        password='Fkmnfbh9',
        host='192.168.31.132'
    ) as conn:
        with conn.cursor() as cursor:
            with open(filename, "rb") as fin:
                data = fin.read()
            cursor.execute("INSERT INTO jpeg.files VALUES (DEFAULT, %s, %s, %s)", (len(data), data, filename))
            conn.commit()
    '''

if __name__ == "__main__":
    main()