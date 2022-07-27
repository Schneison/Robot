from typing import Final, Union
import logging
from serial import Serial, SerialException, PortNotOpenError, SerialTimeoutException
import serial as serial
import time
import threading

baudrate: Final[int] = 9600
serial_connected: bool = False


class SerialHandler:

    def __init__(self, ser: Serial, logger: logging.Logger):
        self.ser = ser
        self.data = []
        self.stop = False
        self.thread1 = threading.Thread(target=self.receive_data)
        self.thread1.daemon = True
        self.logger = logger
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
            if self.ser is not None and self.ser.inWaiting() > 0:
                try:
                    txt = self.ser.readline().decode().replace('\n', '')
                    # print("I received :", txt)
                    self.logger.log(logging.INFO, txt)
                    #self.received = self.ser.readline().decode().replace('\n', '')
                    #self.data.append(str(self.com_port) + ': ' + self.received)
                    #print("I received :", self.received)
                except:
                    pass
            time.sleep(0.05)


ser_port: Union[Serial, None] = None
ser_handler: Union[SerialHandler, None] = None


def try_send(data: str):
    if len(data) > 1 or not data.isalpha() or not data.isupper():
        print("Not Send: Invalid character")
        return
    send_byte(data)


def open_port(port: str, logger: logging.Logger) -> bool:
    global ser_port
    global serial_connected
    global ser_handler
    if serial_connected:
        logger.log(logging.INFO, "Disconnected from: " + ser_port.portstr)
        ser_close()
        serial_connected = False
        return False
    connect = False
    try:
        ser_port = Serial(port=port, baudrate=baudrate, parity=serial.PARITY_NONE,
                          stopbits=serial.STOPBITS_TWO,
                          bytesize=serial.EIGHTBITS, timeout=0.2)
        logger.log(logging.INFO, "Connected to " + port)
        connect = True
        serial_connected = True
        ser_handler = SerialHandler(ser_port, logger)
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


def send_byte(data: str):
    if not serial_connected:
        print("Not Send: Not connected")
        return
    print("Send: %s" % bytes(data, 'ascii', 'ignore'))
    ser_port.write(bytes(data, 'ascii', 'ignore'))
    ser_port.write(bytes('\r\n', 'ascii', 'ignore'))


def read_struc():
    ser_port.read_until(4)


def ser_close():
    if ser_port and ser_port.is_open:
        ser_port.close()
    if ser_handler:
        ser_handler.stop_threads()

