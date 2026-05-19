# C++ refactoring

## C++

```
cd ~/my_cpp_app
```
```
touch functions.h functions.cpp database.h database.cpp
```
Обновите содержимое файлов
```
g++ -std=c++17 -o my_server main.cpp database.cpp functions.cpp -I/usr/local/include -lmysqlcppconn -lpthread -lssl -lcrypto -lboost_system
```
```
sudo systemctl restart cpp-backend.service
```