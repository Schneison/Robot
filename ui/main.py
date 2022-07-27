import tkinter as tk
from tkinter import ttk, StringVar, VERTICAL, HORIZONTAL, N, S, E, W, FLAT, LEFT
from tkinter.scrolledtext import ScrolledText
from typing import Final, Union
import queue
from struct import *
import logging
from serial import Serial, SerialException, PortNotOpenError, SerialTimeoutException
import serial as serial
import time
import threading

baudrate: Final[int] = 115200
serial_connected: bool = False
logger = logging.getLogger(__name__)


class SerialHandler:

    def __init__(self, ser: Serial):
        self.ser = ser
        self.data = []
        self.stop = False
        self.thread1 = threading.Thread(target=self.receive_data)
        self.thread1.daemon = True
        #     thread2 = threading.Thread(target=self.update_gui)
        #     thread2.daemon = True

    def stop_threads(self):
        self.stop = True
        self.thread1.join()
        self.stop = False
        #self.thread2.joint()

    def start_threads(self):
        self.thread1.start()

    #     thread2 = threading.Thread(target=self.update_gui)
    #     thread2.daemon = True
    #     thread2.start()
    #
    # def update_gui(self):
    #     while True:
    #         try:
    #             if len(self.temp) < len(self.data) and self.ser is not None:  # if data has been added
    #                 self.comm_text.configure(state='normal')
    #                 self.comm_text.insert('end', self.data[len(self.data) - 1] + '\n')
    #                 self.comm_text.see('end')  # scroll text
    #                 self.comm_text.configure(state='disabled')
    #                 self.temp.append(self.data[len(self.data) - 1])
    #             time.sleep(0.2)
    #         except:
    #             pass

    def receive_data(self):
        while not self.stop:
            if self.ser is not None:
                if self.ser.inWaiting() > 0:
                    logger.log(logging.INFO, self.ser.inWaiting())
                    try:
                        txt = self.ser.readline().decode().replace('\n', '')
                        # print("I received :", txt)
                        logger.log(logging.INFO, txt)
                        #self.received = self.ser.readline().decode().replace('\n', '')
                        #self.data.append(str(self.com_port) + ': ' + self.received)
                        #print("I received :", self.received)
                    except:
                        pass
            time.sleep(0.05)


ser_port: Union[Serial, None] = None
ser_handler: Union[SerialHandler, None] = None


class TextHandler(logging.Handler):
    """This class allows you to log to a Tkinter Text or ScrolledText widget"""

    def __init__(self, text):
        # run the regular Handler __init__
        logging.Handler.__init__(self)
        # Store a reference to the Text it will log to
        self.text = text

    def emit(self, record):
        msg = self.format(record)

        def append():
            self.text.configure(state='normal')
            self.text.insert(tk.END, msg + '\n')
            self.text.configure(state='disabled')
            # Autoscroll to the bottom
            self.text.yview(tk.END)

        # This is necessary because we can't modify the Text from other threads
        self.text.after(0, append)


class QueueHandler(logging.Handler):
    """Class to send logging records to a queue

    It can be used from different threads
    """

    def __init__(self, log_queue):
        super().__init__()
        self.log_queue = log_queue

    def emit(self, record):
        self.log_queue.put(record)


class ConsoleUi:
    """Poll messages from a logging queue and display them in a scrolled text widget"""

    def __init__(self, frame):
        self.frame = frame
        # Create a ScrolledText wdiget
        self.scrolled_text = ScrolledText(frame, state='disabled', height=12)
        self.scrolled_text.grid(row=0, column=0, sticky=(N, S, W, E))
        self.scrolled_text.configure(font='TkFixedFont')
        self.scrolled_text.tag_config('INFO', foreground='black')
        self.scrolled_text.tag_config('DEBUG', foreground='gray')
        self.scrolled_text.tag_config('WARNING', foreground='orange')
        self.scrolled_text.tag_config('ERROR', foreground='red')
        self.scrolled_text.tag_config('CRITICAL', foreground='red', underline=True)
        # Create a logging handler using a queue
        self.log_queue = queue.Queue()
        self.queue_handler = QueueHandler(self.log_queue)
        formatter = logging.Formatter('%(message)s')
        self.queue_handler.setFormatter(formatter)
        logger.addHandler(self.queue_handler)
        # Start polling messages from the queue
        self.frame.after(100, self.poll_log_queue)

    def display(self, record):
        msg = self.queue_handler.format(record)
        self.scrolled_text.configure(state='normal')
        self.scrolled_text.insert(tk.END, msg + '\n', record.levelname)
        self.scrolled_text.configure(state='disabled')
        # Autoscroll to the bottom
        self.scrolled_text.yview(tk.END)

    def poll_log_queue(self):
        # Check every 100ms if there is a new message in the queue to display
        while True:
            try:
                record = self.log_queue.get(block=False)
            except queue.Empty:
                break
            else:
                self.display(record)
        self.frame.after(100, self.poll_log_queue)


def try_send(data: str):
    if len(data) > 1 or not data.isalpha() or not data.isupper():
        print("Not Send: Invalid character")
        return
    send_byte(data)


class SerialConnection:
    def __init__(self, frm: ttk.Frame):
        self.port_var = StringVar()
        self.frm = frm
        self.connection_var = StringVar()
        self.init_vars()
        self.init_ui()

    def init_vars(self):
        self.port_var.set("rfcomm0")

    def init_ui(self):
        frm = self.frm
        ttk.Label(frm, text="COM Port", relief=FLAT, justify=LEFT).grid(column=0, row=0, sticky="w")
        ttk.Entry(frm, textvariable=self.port_var).grid(column=0, row=1, columnspan=2)
        ttk.Button(frm, text="Apply", command=lambda: self.link_serial()).grid(column=3, row=1)
        ttk.Label(frm, text="Data", relief=FLAT, justify=LEFT).grid(column=0, row=2, sticky="w")
        data_e = ttk.Entry(frm)
        data_e.grid(column=0, row=3, columnspan=2)
        ttk.Button(frm, text="Send", command=lambda: try_send(data_e.get())).grid(column=3, row=3)

    def link_serial(self) -> None:
        port = self.port_var.get()
        open_port(port)


class DriveControl:
    def __init__(self, frm: ttk.Labelframe):
        self.frm = frm
        self.init_ui()

    def init_ui(self):
        ttk.Button(self.frm, text="Start", command=lambda: try_send('S')).grid(column=1, row=0)
        ttk.Button(self.frm, text="Pause", command=lambda: try_send('P')).grid(column=1, row=1)
        ttk.Button(self.frm, text="Rest", command=lambda: try_send('R')).grid(column=2, row=1)
        ttk.Button(self.frm, text="Home", command=lambda: try_send('C')).grid(column=1, row=2)
        ttk.Button(self.frm, text="Freeze", command=lambda: try_send('X')).grid(column=0, row=1)


def open_port(port: str):
    global ser_port
    global serial_connected
    global ser_handler
    if ser_port is not None and ser_port.is_open:
        ser_port.close()
        if ser_handler is not None:
            ser_handler.stop_threads()
    connect = False
    try:
        ser_port = Serial(port=port, baudrate=baudrate, parity=serial.PARITY_NONE,
                          stopbits=serial.STOPBITS_TWO,
                          bytesize=serial.EIGHTBITS, )
        logger.log(logging.INFO, "Connected to " + port)
        connect = True
        ser_handler = SerialHandler(ser_port)
        ser_handler.start_threads()
    except PortNotOpenError:
        logger.log(logging.ERROR, "Connection failed, port not open")
    except SerialTimeoutException:
        logger.log(logging.ERROR, "Connection failed, timeout")
    except SerialException as msg:
        logger.log(logging.ERROR, "Connection failed")
        logger.log(logging.ERROR, msg)
        # con.set_state("Connection failed, ")
    serial_connected = connect


def send_byte(data: str):
    if not serial_connected:
        print("Not Send: Not connected")
        return
    print("Send: %s" % bytes(data, 'ascii', 'ignore'))
    ser_port.write(bytes(data, 'ascii', 'ignore'))
    ser_port.write(bytes('\n', 'ascii', 'ignore'))


def read_struc():
    ser_port.read_until(4)


class Example(tk.Frame):

    def __init__(self, p: ttk.Frame):
        super().__init__(p)

        self.init_ui()

    def init_ui(self):
        canvas = tk.Canvas(self)
        canvas.create_rectangle(30, 10, 120, 80,
                                 fill="#fb0")
        canvas.create_rectangle(150, 10, 240, 80,
                                fill="#f50")
        canvas.create_rectangle(270, 10, 370, 80,
                                fill="#05f")

        canvas.pack(fill=tk.BOTH, expand=1)


def main() -> None:
    logging.basicConfig(level=logging.DEBUG)
    root = tk.Tk()
    port_var = StringVar()
    port_var.set("rfcomm0")
    connection_var = StringVar()
    connection_var.set("State: Not Connected")
    root.title("Robot Control")
    # root.geometry("200x200")
    root.minsize(width=250, height=250)
    frm = ttk.Frame(root, padding=10)
    frm.grid()
    ser_frame = ttk.Frame(frm)
    ser_frame.grid(sticky=N)
    console_frame = ttk.Labelframe(frm, text="Console")
    console_frame.grid(row=0, column=1)
    ConsoleUi(console_frame)
    SerialConnection(ser_frame)
    drive_frame = ttk.Labelframe(frm, text="Drive")
    drive_frame.grid(row=1, column=0, sticky=(E, W))
    DriveControl(drive_frame)
    ex = Example(frm)
    ex.grid(row=1, column=1)
    try:
        root.mainloop()
    except KeyboardInterrupt:
        root.destroy()

    # arduino = serial.Serial(port='rfcomm0', baudrate=115200, timeout=.1)

    # def write_read(x):
    #     arduino.write(bytes(x, 'utf-8'))
    #     time.sleep(0.05)
    #     data = arduino.readline()
    #     return data

    # while True:
    #     num = input("Enter a number: ")  # Taking input from user
    #     value = write_read(num)
    #     print(value)  # printing the value


if __name__ == '__main__':
    main()
