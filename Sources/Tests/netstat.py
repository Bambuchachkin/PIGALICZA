import socket

UDP_IP = "0.0.0.0"      # слушаем на всех доступных интерфейсах
UDP_PORT = 8889

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"UDP слушатель запущен на порту {UDP_PORT}")
print("Ожидание данных...")

while True:
    data, addr = sock.recvfrom(1024)   # буфер 1024 байта
    print(f"Получено от {addr}: {data.decode('utf-8')}")