import socket

LISTEN_IP = "0.0.0.0"
LISTEN_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((LISTEN_IP, LISTEN_PORT))

print(f"Listening UDP on {LISTEN_IP}:{LISTEN_PORT} ...")

while True:
    data, addr = sock.recvfrom(2048)
    print(f"[{addr[0]}:{addr[1]}] {data.decode(errors='replace').rstrip()}")