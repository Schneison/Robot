from __future__ import annotations

import logging
import tkinter as tk
from tkinter import ttk, StringVar, FLAT, LEFT
from tkinter.scrolledtext import ScrolledText
import queue
import atexit
import signal
import sys
from dataclasses import dataclass
from typing import List, Callable, NoReturn, Union

import ser
from ser import UpdateFunction, try_send, open_port, close_port, StateTuple, is_connected
from PIL import Image
from PIL.ImageTk import PhotoImage
import warnings

warnings.filterwarnings("ignore", category=DeprecationWarning)
# Pillow 10

logger = logging.getLogger(__name__)

SENSOR_NONE = 0
SENSOR_LEFT = 1
SENSOR_CENTER = 2
SENSOR_RIGHT = 4
SENSOR_ALL = 7

DRIVE_NONE = 0
DRIVE_LEFT = 1
DRIVE_STRAIGHT = 2
DRIVE_RIGHT = 4


@dataclass
class RobotState:
    """State of the robot, this gets send from the robot if we request it"""
    led: int
    drive_state: int
    action: int
    home: bool
    manuel: bool
    battery: int
    connected: bool

    def with_connection(self, connected) -> RobotState:
        return RobotState(self.led, self.drive_state, self.action, self.home, self.manuel, self.battery, connected)


STATE_EMPTY = RobotState(SENSOR_NONE, DRIVE_NONE, 0, False, False, 0, False)


class QueueHandler(logging.Handler):
    """Internal queue for the logging, which handles the logged messages to the console ui"""

    def __init__(self, log_queue):
        super().__init__()
        self.log_queue = log_queue

    def emit(self, record):
        self.log_queue.put(record)


class ConsoleDisplay:
    """Scrolling text field which displays messages given to the logger"""

    def __init__(self, frame):
        self.frame = frame
        # Create a ScrolledText wdiget
        self.scrolled_text = ScrolledText(frame, state='disabled', height=12)
        self.scrolled_text.grid(row=0, column=0, sticky=tk.NSEW)
        self.scrolled_text.configure(font='TkFixedFont')
        self.scrolled_text.tag_config('INFO', foreground='black')
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
        self.frame.after(100, self.poll_log)

    def display_message(self, record):
        """Displays the given message on the console"""
        msg = self.queue_handler.format(record)
        self.scrolled_text.configure(state='normal')
        self.scrolled_text.insert(tk.END, msg + '\n', record.levelname)
        self.scrolled_text.configure(state='disabled')
        # Autoscroll to the bottom
        self.scrolled_text.yview(tk.END)

    def poll_log(self):
        """Polls messages from the que"""
        # Check every 100ms if there is a new message in the queue to display
        while True:
            try:
                record = self.log_queue.get(block=False)
            except queue.Empty:
                break
            else:
                self.display_message(record)
        self.frame.after(100, self.poll_log)


class ConnectionControl:
    """Controls which can be used to open ports and send data via serial"""

    def __init__(self, frm: ttk.Frame, update_state: UpdateFunction, con_call: Callable[[bool], NoReturn]):
        self.port_var = StringVar()
        self.connect_var = StringVar()
        self.frm = frm
        self.connection_var = StringVar()
        self.init_vars()
        self.init_ui()
        self.update_state = update_state
        self.con_call = con_call

    def init_vars(self):
        """Initialises the variables used by the ui entries and buttons"""
        self.port_var.set("")
        self.connect_var.set("Open")

    def init_ui(self):
        """Creates the ui elements of this control"""
        frm = self.frm
        ttk.Label(frm, text="COM Port", relief=FLAT, justify=LEFT).grid(column=0, row=0, sticky=tk.W)
        ttk.Entry(frm, textvariable=self.port_var).grid(column=0, row=1, columnspan=3, sticky=tk.EW)
        ttk.Button(frm, textvariable=self.connect_var, command=lambda: self.open_or_close_serial()).grid(column=3,
                                                                                                         row=1)
        ttk.Button(frm, text="COM1", command=lambda: self.port_var.set("COM1")).grid(column=0, row=2)
        ttk.Button(frm, text="/dev/ttyACM0", command=lambda: self.port_var.set("/dev/ttyACM0")).grid(column=1, row=2)
        ttk.Button(frm, text="rfcomm0", command=lambda: self.port_var.set("rfcomm0")).grid(column=2, row=2)
        ttk.Label(frm, text="Data", relief=FLAT, justify=LEFT).grid(column=0, row=3, sticky=tk.W)
        data_e = ttk.Entry(frm)
        data_e.grid(column=0, row=4, columnspan=3, sticky=tk.EW)
        ttk.Button(frm, text="Send", command=lambda: try_send(data_e.get(), logger)).grid(column=3, row=4)

    def open_or_close_serial(self) -> None:
        """Action for the open/close button, tries to open/closes the port contained in the port entry."""
        port = self.port_var.get()
        connected = open_port(port, logger, self.update_state)
        self.con_call(connected)
        self.connect_var.set("Close" if connected else "Open")


class DriveControl:
    """Controls which can be used to drive the robot or give commands"""
    connection_buttons: List[ttk.Button]
    manuel_buttons: List[ttk.Button]

    def __init__(self, frm: ttk.Labelframe):
        self.frm = frm
        self.connection_buttons = []
        self.manuel_buttons = []
        self.init_ui()

    def update_state(self, robot_state: RobotState):
        """Update the state of the ui elements"""
        state = tk.NORMAL if robot_state.connected else tk.DISABLED
        state_manuel = tk.NORMAL if robot_state.connected and robot_state.manuel else tk.DISABLED
        for widget in self.connection_buttons:
            widget.configure(state=state)
        for widget in self.manuel_buttons:
            widget.configure(state=state_manuel)

    def init_ui(self):
        """Creates the ui elements of this control"""

        def add_connection(button: ttk.Button) -> ttk.Button:
            self.connection_buttons.append(button)
            return button

        def add_manuel(button: ttk.Button) -> ttk.Button:
            self.manuel_buttons.append(button)
            return button

        # -S-
        # FPR
        # -H-
        add_connection(ttk.Button(self.frm, text="Start", command=lambda: try_send('S', logger))).grid(column=1, row=0)
        add_connection(ttk.Button(self.frm, text="Pause", command=lambda: try_send('P', logger))).grid(column=1, row=1)
        add_connection(ttk.Button(self.frm, text="Rest", command=lambda: try_send('R', logger))).grid(column=2, row=1)
        add_connection(ttk.Button(self.frm, text="Home", command=lambda: try_send('C', logger))).grid(column=1, row=2)
        add_connection(ttk.Button(self.frm, text="Freeze", command=lambda: try_send('X', logger))).grid(column=0, row=1)
        # Needed for space between the buttons
        ttk.Label(self.frm, text="").grid(column=1, row=3)
        # -W-
        # AMD
        # -B-
        add_manuel(ttk.Button(self.frm, text="Forward", command=lambda: try_send('W', logger))).grid(column=1, row=4)
        add_manuel(ttk.Button(self.frm, text="Right", command=lambda: try_send('D', logger))).grid(column=2, row=5)
        add_connection(ttk.Button(self.frm, text="Manuel", command=lambda: try_send('M', logger))).grid(column=1, row=5)
        add_manuel(ttk.Button(self.frm, text="Backward", command=lambda: try_send('B', logger))).grid(column=1, row=6)
        add_manuel(ttk.Button(self.frm, text="Left", command=lambda: try_send('A', logger))).grid(column=0, row=5)


def create_image(path: str, flip=False) -> PhotoImage:
    """Creates a 80x80 image object for the given path and flips it if needed."""
    img = Image.open(path).convert("RGBA").resize((80, 80))
    if flip:
        img = img.transpose(Image.FLIP_LEFT_RIGHT)
    return PhotoImage(img)


def convert_tuple_state(state_tuple: StateTuple) -> RobotState:
    """Converts the tuple state to a state object"""
    return RobotState(state_tuple[0], state_tuple[1], state_tuple[2], state_tuple[3] > 0, state_tuple[4] > 0,
                      state_tuple[5], is_connected())


class StateDisplay(tk.Frame):
    """Displays the current state of the robot"""

    canvas: Union[tk.Canvas, None]
    battery: Union[ttk.Progressbar, None]

    def __init__(self, p: ttk.Frame):
        super().__init__(p)

        self.drive_right = None
        self.drive_straight = None
        self.drive_left = None
        self.arrow_right = create_image("assets/arrow_right.png")
        self.arrow_right_light = create_image("assets/arrow_right_light.png")
        self.arrow_left = create_image("assets/arrow_right.png", flip=True)
        self.arrow_left_light = create_image("assets/arrow_right_light.png", flip=True)
        self.arrow_straight = create_image("assets/arrow_straight.png")
        self.arrow_straight_light = create_image("assets/arrow_straight_light.png")
        self.led_right = None
        self.led_center = None
        self.led_left = None
        self.canvas = None
        self.battery = None
        self.init_ui()

    def update_state(self, state: RobotState):
        """Update the state of the ui elements"""
        # Battery
        self.battery.configure(value=state.battery)
        # Blue LED
        self.canvas.itemconfig(self.led_left, fill="#05f" if state.led & SENSOR_LEFT else "#667e92")
        # Green LED
        self.canvas.itemconfig(self.led_center, fill="#3d7d30" if state.led & SENSOR_CENTER else "#89ac76")
        # Yellow LED
        self.canvas.itemconfig(self.led_right, fill="#fb0" if state.led & SENSOR_RIGHT else "#f2e7bf")
        # Drive Left
        self.canvas.itemconfig(self.drive_left,
                               image=self.arrow_left if state.drive_state & DRIVE_LEFT else self.arrow_left_light)
        # Drive Straight
        self.canvas.itemconfig(self.drive_straight,
                               image=self.arrow_straight if state.drive_state & DRIVE_STRAIGHT else self.arrow_straight_light)
        # Drive Right
        self.canvas.itemconfig(self.drive_right,
                               image=self.arrow_right if state.drive_state & DRIVE_RIGHT else self.arrow_right_light)

    def init_ui(self):
        frm = ttk.Labelframe(self, text="Battery")
        self.battery = ttk.Progressbar(frm, maximum=100)
        self.battery.pack()
        frm.pack(pady=4, fill=tk.X, expand=1)
        self.battery.pack(pady=4, fill=tk.X, expand=1)
        """Creates the ui elements of this control"""
        self.canvas = tk.Canvas(self)
        self.led_left = self.canvas.create_rectangle(30, 10, 120, 80)
        self.led_center = self.canvas.create_rectangle(150, 10, 240, 80)
        self.led_right = self.canvas.create_rectangle(270, 10, 370, 80)

        self.drive_left = self.canvas.create_image(130, 150)
        self.drive_straight = self.canvas.create_image(200, 150)
        self.drive_right = self.canvas.create_image(270, 150)

        self.canvas.pack(fill=tk.BOTH, expand=1)


def exit_handler():
    """Handles the cleanup on exit, aka closes all open connections and stops all running threads"""
    close_port()


def sigint_handler(signal, frame):
    exit_handler()
    print('Interrupted: Close')
    sys.exit(0)


def create_ui(root: tk.Tk):
    """Creates the main ui of this program"""
    frm = ttk.Frame(root, padding=10)
    frm.grid()
    ser_frame = ttk.Frame(frm)
    ser_frame.grid(sticky=tk.N)
    console_frame = ttk.Labelframe(frm, text="Console")
    console_frame.grid(row=0, column=1)
    ConsoleDisplay(console_frame)
    drive_frame = ttk.Labelframe(frm, text="Drive")
    drive_frame.grid(row=1, column=0, sticky=tk.NSEW)
    drive = DriveControl(drive_frame)
    ex = StateDisplay(frm)
    ex.grid(row=1, column=1)

    def update(state: RobotState):
        ex.update_state(state)
        drive.update_state(state)

    ConnectionControl(ser_frame,
                      lambda state_tuple: update(convert_tuple_state(state_tuple)),
                      lambda connected: update(STATE_EMPTY.with_connection(connected)))
    update(STATE_EMPTY)


def main() -> None:
    """Main method"""
    atexit.register(exit_handler)
    signal.signal(signal.SIGINT, sigint_handler)
    logging.basicConfig(level=logging.INFO)
    root = tk.Tk()
    root.title("Robot Control")
    root.wm_iconbitmap("assets/icon.ico")
    root.resizable(False, False)
    create_ui(root)
    root.mainloop()
    exit_handler()


if __name__ == '__main__':
    main()
