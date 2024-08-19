import sys
import serial
import socket
import threading

ev = threading.Event()

def tcp_to_serial(sock, ser):
    print(f'TCP to serial forwarding started on {sock.getpeername()} to {ser.name}...')
    while True:
        try:
            if ev.is_set():
                break
            data = sock.recv(1024)
            if not data:
                print(f'No data received from {sock.getpeername()}')
                continue
            if ev.is_set():
                break
            ser.write(data)
            print(f'Sent {len(data)} bytes to serial port...')
        except socket.timeout:
            pass
        except KeyboardInterrupt:
            print('KB1')
            break
        except Exception as e:
            print(f'Error reading data from {sock.getpeername()}: {e}')
            break
    ev.set()

def serial_to_tcp(sock, ser):
    print(f'Serial to TCP forwarding started on {serial_port} to {host}:{port}...')
    while True:
        try:
            if ev.is_set():
                break
            data = ser.read(8)
            if not data:
                print(f'No data received from {ser.name}')
                continue
            _, client_addr, server_addr, bytes_to_read = map(lambda x: int.from_bytes(x, 'big'), list(data[x:x+2] for x in range(0, 8, 2)))
            print(f'Reading {bytes_to_read} bytes from {client_addr} to {server_addr}...')
            data = data + ser.read(bytes_to_read)
            if ev.is_set():
                break
            print(f'Sending {len(data)} bytes to {host}:{port}...')
            for x in data:
                print(f'{x:02X}', end=' ')
            print()
            sock.sendall(data)
        except serial.SerialTimeoutException:
            pass
        except KeyboardInterrupt:
            print('KB2')
            break
        except Exception as e:
            print(f'Error reading data from {ser.name}: {e}')
            break
    ev.set()

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print('Usage: python serial_to_tcp.py <serial_port> <baud_rate> <host> <port>')
        sys.exit(1)
    serial_port = sys.argv[1]
    baud_rate = int(sys.argv[2])
    host = sys.argv[3]
    port = int(sys.argv[4])
    
    ser = serial.Serial(serial_port, baud_rate, timeout=10)
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(10)
    sock.connect((host, port))
    
    try:
        threads = [
            threading.Thread(target=serial_to_tcp, args=(sock, ser)),
            threading.Thread(target=tcp_to_serial, args=(sock, ser)),
        ]
    
        for thread in threads:
            thread.start()
    
        for thread in threads:
            thread.join()
    
    except KeyboardInterrupt:
        print('KB0')
        pass
    
    ser.close()
    sock.close()
    print('Serial-to-TCP stopped.')
    sys.exit(0)