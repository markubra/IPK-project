# HTTP server (IPK project)

Projekt byl vypracován v jazyce C. Slouží pro výpis jména hostu, nazvu procesoru nebo aktuální zátěže procesoru.

## Autor
Marko Kubrachenko [xkubra00]

## Použití
```
1. make
2. ./hinfosvc [port] &
3. curl http://localhost:[port]/[hostname | cpu_name | load]
```
## Příklad
```
$ ./hinfosvc 8080 &
$ curl http://localhost:8080/load
Output: 5%
```
## Použité zdroje
```
1. https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
2. https://tutorialspoint.dev/language/cpp/socket-programming-cc
```
