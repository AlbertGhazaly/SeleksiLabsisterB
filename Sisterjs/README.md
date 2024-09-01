# SisterJS


## Table of Contents
* [Ketentuan](#ketentuan)
* [Features](#Features)
* [Total Poin](#total-poin)
* [Screenshots](#screenshots)
* [Setup](#setup)
* [Usage](#usage)
* [Pengujian](#pengujian)
* [Profile](#profile)
## Ketentuan

![Ketentuan1](./img/Ketentuan1.png)</br>
![Ketentuan2](./img/Ketentuan2.png)</br>

## Features
- Spek Wajib (6 Poin)
- Bonus Parser Json manual (1 Poin)
- Bonus Buat program dengan bahas C (2 poin)
- Bonus Paralelisasi thread-based (4 poin)
- Bonus Paralelisasi event-based (4 poin)
- Bonus HTTP2 (4 poin)

## Total Poin
    point = 6 + 1 + 2 + 4 + 4 + 4 = 21 Poin



## Usage
    Untuk server: 

    cd src
        gcc -o server server.c -lpthread

        - untuk thread-based:
            ./server thread

        - untuk event-based:
            ./server event
    
    Untuk test:
        cd src
        gcc -o test test.c -lcurl</br>
        ./test

## Pengujian
1. GET
- no query:</br>
![getNoQuery](./img/getNoQuery.png)
</br>
Note: Ambil data adalah fungsi untuk get hash table data dari server, nilai awal kosong
![ambilData](./img/ambil_data.png)
- with query:</br>
![getWithQuery](./img/getWithQuery.png)
</br>
2. POST
- plain text</br>
Request</br>
![PostPlainCode](./img/postPlainCode.png)</br>
Response</br>
![PostPlainCls](./img/postPlainCls.png)</br>
- Json </br>
Request</br>
![PostJsonCode](./img/postJsonCode.png)</br>
Response</br>
![PostJsonCls](./img/postJsonCls.png)</br>
- Urlencoded </br>
Request</br>
![PostUrlCode](./img/postUrlCode.png)</br>
Response</br>
![PostUrlCls](./img/postUrlCls.png)</br>
</br>
3. PUT
- plain text</br>
Request</br>
![PutPlainCode](./img/putPlainCode.png)</br>
Response</br>
![PutPlainCls](./img/putPlainCls.png)</br>
- Json </br>
Request</br>
![PutJsonCode](./img/putJsonCode.png)</br>
Response</br>
![PutJsonCls](./img/putJsonCls.png)</br>
- Urlencoded </br>
Request</br>
![PutUrlCode](./img/putUrlCode.png)</br>
Response</br>
![PutUrlCls](./img/putUrlCls.png)</br>
4. Delete
Request</br>
![DeleteCode](./img/Delete.png)</br>
Response</br>
![DeleteCls](./img/DeleteCls.png)</br>
5. HTTP2
Request</br>
![Http2Code](./img/Http2Code.png)</br>
Response</br>
![Http2Cls](./img/Http2Cls.png)</br>

## Profile
* Nama: Albert Ghazaly
* NIM: 13522150
* Kelas: K03
