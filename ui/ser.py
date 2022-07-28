from typing import Final, Union, Literal, Callable, Tuple, NoReturn
from logging import Logger, INFO, ERROR
from serial import Serial, SerialException, PortNotOpenError, SerialTimeoutException
import serial as serial
import time
import threading
import queue
from ast import literal_eval as make_tuple

baud_rate: Final[int] = 9600
serial_connected: bool = False
StateTuple = Tuple[int, int, int, int]
UpdateFunction = Callable[[StateTuple], NoReturn]


class SerialHandler:
    """Handles all operations regarding the serial port"""

    def __init__(self, port: str, logger: Logger, update_state: UpdateFunction):
        """Initialises a serial handler object, and open the given port."""
        self.ser = Serial(port=port, baudrate=baud_rate, parity=serial.PARITY_NONE,
                          stopbits=serial.STOPBITS_TWO,
                          bytesize=serial.EIGHTBITS, timeout=0.2)
        self.data = queue.Queue()
        self.port = port
        self.stop = False
        self.update_state = update_state
        self.thread_receive = threading.Thread(target=self.receive_data)
        self.thread_receive.daemon = True
        self.logger = logger
        self.thread_update = threading.Thread(target=self.update_gui)
        self.thread_update.daemon = True

    def stop_threads(self):
        """Stops all current threads that run on the port"""
        self.stop = True
        self.thread_receive.join()
        self.thread_update.join()
        self.stop = False

    def start_threads(self):
        """Starts all current threads that run on the port"""
        self.thread_receive.start()
        self.thread_update.start()

    def update_gui(self):
        """Run on the update thread to update the current ui state"""
        while not self.stop:
            if not self.data.empty():  # if data has been added
                self.update_state(self.data.get())
            time.sleep(0.01)

    def receive_data(self):
        """Run on the receive thread to read data from the port"""
        while not self.stop:
            if self.ser is not None and self.ser.is_open and self.ser.inWaiting() > 0:
                try:
                    txt = self.ser.readline().decode().replace('\n', '')
                    # State info
                    if txt.startswith('[') and txt.endswith(']'):
                        self.read_state(txt)
                    else:
                        self.logger.log(INFO, txt)
                except:
                    pass
            time.sleep(0.01)

    def read_state(self, txt: str):
        """Serialises the given text and add it to the state queue"""
        try:
            data = txt[1:len(txt) - 1]
            state_tuple = make_tuple(data)
            self.data.put(state_tuple)
        except SyntaxError as msg:
            self.logger.log(ERROR, "Failed to read state %s" % msg)

    def request_state(self):
        """Writes message to the port, that request a state update from the robot"""
        if not serial_connected:
            return
        self.ser.write(bytes('N\r\n', 'ascii', 'ignore'))

    def send_byte(self, data: str):
        """Writes the given text to the port"""
        if not serial_connected:
            print("Not Send: Not connected")
            return
        print("Send: %s" % bytes(data, 'ascii', 'ignore'))
        self.ser.write(bytes(data, 'ascii', 'ignore'))
        self.ser.write(bytes('\r\n', 'ascii', 'ignore'))

    def close(self):
        """Close the port and stop all threads"""
        self.stop_threads()
        if self.ser and self.ser.is_open:
            self.ser.close()


ser_handler: Union[SerialHandler, None] = None


def is_connected() -> bool:
    """Checks if we have a open connection to a port"""
    return ser_handler and ser_handler.ser.is_open


def try_send(data: str, logger: Logger):
    """Tries to send the given data via serial to the robot"""
    if not ser_handler:
        logger.log(ERROR, "Not Send: Not Connected")
        return
    if len(data) > 1 or not data.isalpha() or not data.isupper():
        print("Not Send: Invalid character")
        return
    ser_handler.send_byte(data)


def close_port():
    """Closes the serial handler and the associated port"""
    if ser_handler:
        ser_handler.close()


def open_port(port: str, logger: Logger, update_state: UpdateFunction) -> bool:
    """Creates a serial handler, opens the port to the serial, or closes it if it is already open
     and print a message to the log."""
    global serial_connected
    global ser_handler
    if serial_connected:
        logger.log(INFO, "Disconnected from " + ser_handler.port)
        close_port()
        serial_connected = False
        ser_handler = None
        return False
    connect = False
    try:
        logger.log(INFO, "Connected to " + port)
        connect = True
        ser_handler = SerialHandler(port, logger, update_state)
        ser_handler.start_threads()
    except PortNotOpenError:
        logger.log(ERROR, "Connection failed, port not open")
    except SerialTimeoutException:
        logger.log(ERROR, "Connection failed, timeout")
    except SerialException as msg:
        logger.log(ERROR, "Connection failed")
        logger.log(ERROR, msg)
    serial_connected = connect
    return connect
