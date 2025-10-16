import socket

SERVER_IP = "127.0.0.1"
SERVER_PORT = 5555
MESSAGE = b"Radar test packet"

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Send message once
sock.sendto(MESSAGE, (SERVER_IP, SERVER_PORT))
print(f"Sent: {MESSAGE.decode()} -> {SERVER_IP}:{SERVER_PORT}")

# Close socket immediately
sock.close()
