import serial
import time


port = 'COM10'  # port number

baud_rate = 2400 # baud rate 

# Creating a serial connection
ser = serial.Serial(port, baud_rate, timeout=1)

try:
    file_path = r"C:\Users\lokke\Desktop\nymble_firmware_intern_annamalai\nymble_text.txt"
    while True:
        # Getting user input for the file path
        user_input = input("Enter any character to start the process (type 'exit' to quit): ")
        if user_input.lower() == 'exit':
            break

        try:
            # Reading the content of the text file
            with open(file_path, 'r') as file:
                file_content = file.read()

            # Recording the start time for measuring data transmission speed
            start_time = time.time()

            # Sending the file content to Arduino
            ser.write(file_content.encode())

            # Waiting for a moment to allow Arduino to process and send data back
            time.sleep(3)
            
            # Reading and printing the looped-back data from Arduino
            received_data = ser.readline().decode('utf-8').rstrip()

            elapsed_time = time.time() - start_time
            print("Received data from Arduino:", received_data)

            # Calculating and printing the real-time data transmission speed
           
            total_bits_transmitted = len(file_content) * 8
            data_rate = total_bits_transmitted / elapsed_time
            print(f"Data Transmission Speed: {data_rate} bits/second")

        except FileNotFoundError:
            print("File not found. Please enter a valid file path.")

except KeyboardInterrupt:
    # Closing the serial port when the program is interrupted
    ser.close()
    print("Serial port closed.")
