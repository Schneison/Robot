from typing import Final, Union, Literal, Callable, Tuple, NoReturn
import logging
from serial import Serial, SerialException, PortNotOpenError, SerialTimeoutException
import serial as serial
import time
import threading
import queue
from ast import literal_eval as make_tuple

baudrate: Final[int] = 9600
serial_connected: bool = False
StateTuple = Tuple[int, int, int, int]
UpdateFunction = Callable[[StateTuple], NoReturn]


class SerialHandler:

    def __init__(self, port: str, logger: logging.Logger, update_state: UpdateFunction):
        self.ser = Serial(port=port, baudrate=baudrate, parity=serial.PARITY_NONE,
                          stopbits=serial.STOPBITS_TWO,
                          bytesize=serial.EIGHTBITS, timeout=0.2)
        self.data = queue.Queue()
        self.port = port
        self.stop = False
        self.update_state = update_state
        self.thread1 = threading.Thread(target=self.receive_data)
        self.thread1.daemon = True
        self.logger = logger
        self.thread2 = threading.Thread(target=self.update_gui)
        self.thread2.daemon = True

    def stop_threads(self):
        self.stop = True
        self.thread1.join()
        self.thread2.join()
        self.stop = False

    def start_threads(self):
        self.thread1.start()
        self.thread2.start()

    def update_gui(self):
        while True:
            if not self.data.empty():  # if data has been added
                self.update_state(self.data.get())
            time.sleep(0.01)

    def receive_data(self):
        while not self.stop:
            if self.ser is not None and self.ser.is_open and self.ser.inWaiting() > 0:
                try:
                    txt = self.ser.readline().decode().replace('\n', '')
                    # State info
                    if txt.startswith('[') and txt.endswith(']'):
                        self.read_state(txt)
                    else:
                        self.logger.log(logging.INFO, txt)
                except:
                    pass
            time.sleep(0.01)

    def read_state(self, txt: str):
        try:
            data = txt[1:len(txt) - 1]
            state_tuple = make_tuple(data)
            self.data.put(state_tuple)
        except SyntaxError as msg:
            logging.log(logging.ERROR, "Failed to read state %s" % msg)

    def request_state(self):
        if not serial_connected:
            return
        self.ser.write(bytes('N\r\n', 'ascii', 'ignore'))

    def send_byte(self, data: str):
        if not serial_connected:
            print("Not Send: Not connected")
            return
        print("Send: %s" % bytes(data, 'ascii', 'ignore'))
        self.ser.write(bytes(data, 'ascii', 'ignore'))
        self.ser.write(bytes('\r\n', 'ascii', 'ignore'))

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.stop_threads()


ser_handler: Union[SerialHandler, None] = None


def try_send(data: str, logger: logging.Logger):
    if not ser_handler:
        logger.log(logging.ERROR, "Not Send: Not Connected")
        return
    if len(data) > 1 or not data.isalpha() or not data.isupper():
        print("Not Send: Invalid character")
        return
    ser_handler.send_byte(data)


def ser_close():
    if ser_handler:
        ser_handler.close()


def open_port(port: str, logger: logging.Logger, update_state: UpdateFunction) -> bool:
    global serial_connected
    global ser_handler
    if serial_connected:
        logger.log(logging.INFO, "Disconnected from: " + ser_handler.port)
        ser_close()
        serial_connected = False
        ser_handler = None
        return False
    connect = False
    try:
        logger.log(logging.INFO, "Connected to " + port)
        connect = True
        ser_handler = SerialHandler(port, logger, update_state)
        ser_handler.start_threads()
    except PortNotOpenError:
        logger.log(logging.ERROR, "Connection failed, port not open")
    except SerialTimeoutException:
        logger.log(logging.ERROR, "Connection failed, timeout")
    except SerialException as msg:
        logger.log(logging.ERROR, "Connection failed")
        logger.log(logging.ERROR, msg)
    serial_connected = connect
    return connect

