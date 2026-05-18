# C++ refactoring

## C++

Вставьте в my_cpp_app содержимое из дирректории

```
cd ~/my_cpp_app
g++ -std=c++17 -o my_server main.cpp -I/usr/local/include -lmysqlcppconn -lpthread -lssl -lcrypto -lboost_system
```
```
sudo systemctl restart cpp-backend.service
```