import socket

# Define server parameters
HOST = '127.0.0.1'  # Localhost
PORT = 12341# Port to bind the server to

# Create a socket object
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
    # Bind the socket to the host and port
    server_socket.bind((HOST, PORT))
    # Start listening for incoming connections (max 5 connections in the queue)
    server_socket.listen(5)

    print(f"Server started, listening on {HOST}:{PORT}")

    while True:
        # Accept a connection (this blocks until a client connects)
        client_socket, client_address = server_socket.accept()
        with client_socket:
            print(f"Connection established with {client_address}")
            while True:
                # Receive data from the client
                hello = "Server received your message"
                client_socket.sendall(hello.encode('utf-8'))

                data = client_socket.recv(1024)
                if not data:
                    # If no data is received, the client has closed the connection
                    print("Connection closed by client")
                    break
                # Print received data and send a response
                print(f"Received: {data.decode('utf-8')}")
                response = "Server received your message"
                client_socket.sendall(response.encode('utf-8'))

