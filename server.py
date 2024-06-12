"""import socket
import threading

# Configurações do servidor
HOST = '0.0.0.0'  # Escuta em todas as interfaces de rede disponíveis
PORT = 5000       # Porta para escutar conexões

# Cria o socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen()

print(f"Servidor escutando em {HOST}:{PORT}")

conn, addr = server_socket.accept()
print(f"Conectado por {addr}")

lastMotorState = False
lastWeightState = False

# Função para receber dados do Arduino
def receive_data():
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                break
            print(f"Dados recebidos: {data.decode()}")
    finally:
        conn.close()
        server_socket.close()

# Função para enviar comandos ao Arduino
def send_motor_command(state):
   if state == 'on':
      command = 'activate_motor'
   elif state == 'off':
      command = 'deactivate_motor'
   else:
      command = lastMotorState
  
   try:
     conn.sendall(command.encode())
     print(f"Comando '{command}' enviado ao Arduino.")
   except Exception as e:
     print(f"Erro ao enviar comando: {e}")

def get_weight(state):
   if state == 'true':
      command = 'get_weight'
   elif state == 'false':
      command = 'stop_weighing'
   else: 
      command = lastWeightState

   try: 
      conn.sendall(command.encode())
      print(f"Comando '{command}' enviado ao Arduino.")
   except Exception as e:
        print(f"Erro ao enviar comando: {e}")

# Função para enviar comandos ao Arduino com entrada do usuário
def user_input():
    try:
        while True:
            state = input("Digite o seu comando ['on','off,'true','false']: ").strip().lower()
            if state in ['on', 'off']:
                send_motor_command(state)
            elif state in ['true','false']:
                get_weight(state)    
            else:
                print("Entrada inválida. Digite 'on' ou 'off'.")
    except Exception as e:
        print(f"Erro ao processar entrada do usuário: {e}")
    finally:
        conn.close()

# Cria threads para receber dados e enviar comandos
thread_receive = threading.Thread(target=receive_data)
thread_input = threading.Thread(target=user_input)

# Inicia as threads
thread_receive.start()
thread_input.start()

# Aguarda o término das threads
thread_receive.join()
thread_input.join()"""

import socket
import threading

# Configurações do servidor
HOST = '0.0.0.0'  # Escuta em todas as interfaces de rede disponíveis
PORT = 5000       # Porta para escutar conexões

# Cria o socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen()

print(f"Servidor escutando em {HOST}:{PORT}")

def handle_client(conn, addr):
    print(f"Conectado por {addr}")

    # Função para receber dados do Arduino
    def receive_data():
        try:
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                print(f"Dados recebidos: {data.decode()}")
        except Exception as e:
            print(f"Erro ao receber dados: {e}")
        finally:
            conn.close()

    # Função para enviar comandos ao Arduino
    def send_motor_command(state):
        command = 'activate_motor' if state == 'on' else 'deactivate_motor'
        try:
            conn.sendall(command.encode())
            print(f"Comando '{command}' enviado ao Arduino.")
        except Exception as e:
            print(f"Erro ao enviar comando: {e}")

    def reset_scale(state):
        command = 'reset_scale'
        try:
            conn.sendall(command.encode())
            print(f"Comando '{command}' enviado ao Arduino.")
        except Exception as e:
            print(f"Erro ao enviar comando: {e}")

    # Função para enviar comandos ao Arduino com entrada do usuário
    def user_input():
        try:
            while True:
                state = input("Digite o seu comando ['on','off,'true']: ").strip().lower()
                if state in ['on', 'off']:
                    send_motor_command(state)
                elif state in ['true']:
                    reset_scale(state)
                else:
                    print("Entrada inválida. Digite 'on', 'off' ou 'true'.")
        except Exception as e:
            print(f"Erro ao processar entrada do usuário: {e}")
        finally:
            conn.close()

    # Cria threads para receber dados e enviar comandos
    thread_receive = threading.Thread(target=receive_data)
    thread_input = threading.Thread(target=user_input)

    # Inicia as threads
    thread_receive.start()
    thread_input.start()

    # Aguarda o término das threads
    thread_receive.join()
    thread_input.join()

while True:
    try:
        conn, addr = server_socket.accept()
        client_thread = threading.Thread(target=handle_client, args=(conn, addr))
        client_thread.start()
    except KeyboardInterrupt:
        print("Servidor interrompido pelo usuário")
        server_socket.close()
        break
    except Exception as e:
        print(f"Erro no servidor: {e}")
