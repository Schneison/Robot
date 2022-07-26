from tkinter import *
from tkinter import ttk
import serial
import time

def print_hi():
    root = Tk()
    frm = ttk.Frame(root, padding=10)
    frm.grid()
    ttk.Label(frm, text="Hello World!").grid(column=0, row=0)
    ttk.Button(frm, text="Quit", command=root.destroy).grid(column=1, row=0)
    root.mainloop()

    arduino = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=.1)

    def write_read(x):
        arduino.write(bytes(x, 'utf-8'))
        time.sleep(0.05)
        data = arduino.readline()
        return data

    while True:
        num = input("Enter a number: ")  # Taking input from user
        value = write_read(num)
        print(value)  # printing the value


if __name__ == '__main__':
    print_hi()