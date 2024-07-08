import sys
import time
import socket
import threading

def handle_client(client_socket, clients):
    while True:
        try:
            data = client_socket.recv(1024)
            if not data:
                break
            message = data.decode('ascii')
            print(message, end='')
            client_socket.send(message.encode('ascii'))
        except Exception as e:
            print(f'Error reading data: {e}')
            clients.remove(client_socket)
            break
    client_socket.close()

def send_to_clients(message, clients):
    for client_socket in clients:
        try:
            client_socket.send(message.encode('ascii'))
            print(f'Sent to client: {message}')
        except Exception as e:
            print(f'Error sending data to client: {e}')

def main():
    host = ''
    port = int(sys.argv[1])

    try:
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((host, port))
        server_socket.listen(5)
        print(f'TCP Server started and listening on port {port}...')

        clients = []
        
        def accept_clients():
            while True:
                try:
                    client_socket, client_address = server_socket.accept()
                    print(f'Client connected: {client_address}')
                    clients.append(client_socket)
                    client_thread = threading.Thread(target=handle_client, args=(client_socket, clients))
                    client_thread.start()
                except KeyboardInterrupt:
                    return
                except OSError as e:
                    if 'An operation was attempted on something that is not a socket' in str(e):
                        return
                    raise e

        accept_thread = threading.Thread(target=accept_clients)
        accept_thread.start()

        while True:
            try:
                message = input("Enter message to send to clients or 'exit' to quit: \n")
                if message == 'exit':
                    break
                send_to_clients(message, clients)
            except KeyboardInterrupt:
                break

    except Exception as e:
        print(f'An error occurred: {e}')
    finally:
        print('Stopping TCP server...')
        server_socket.close()
        for client_socket in clients:
            client_socket.close()
        print('TCP Server stopped.')

if __name__ == '__main__':
    main()
