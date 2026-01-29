import socket
import json
import time
import paho.mqtt.client as mqtt

# ==== UDP ====
UDP_IP = "0.0.0.0"
UDP_PORT = 5005

# ==== THINGSBOARD ====
TB_HOST = "iot.makowski.edu.pl"
TB_PORT = 1883
TB_TOKEN = "1gjCvmT34JxQxHwZR4bo"
TB_TOPIC = "v1/devices/me/telemetry"

MIN_SEND_PERIOD_S = 10.0
PRINT_TELEMETRY = True

client = mqtt.Client()
client.username_pw_set(TB_TOKEN)
client.connect(TB_HOST, TB_PORT, keepalive=60)
client.loop_start()

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening UDP on {UDP_IP}:{UDP_PORT} ...")

last_send = 0.0

while True:
    raw, addr = sock.recvfrom(4096)

    try:
        data = json.loads(raw.decode("utf-8", errors="strict"))
    except Exception as e:
        # jakby przyszły śmieci / ucięta ramka
        print("JSON parse error:", repr(e))
        continue

    now = time.time()
    if now - last_send < MIN_SEND_PERIOD_S:
        continue
    last_send = now

    if PRINT_TELEMETRY:
        print("TELEMETRY:", data)

    # Wysyłamy identyczny JSON do ThingsBoard
    client.publish(TB_TOPIC, json.dumps(data, separators=(",", ":")), qos=1)