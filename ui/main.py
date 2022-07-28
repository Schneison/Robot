import logging
import tkinter as tk
from tkinter import ttk, StringVar, FLAT, LEFT
from tkinter.scrolledtext import ScrolledText
import queue
import atexit
import signal
import sys
from dataclasses import dataclass
from ser import UpdateFunction, try_send, open_port, close_port, StateTuple
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
    led: int
    drive_state: int
    action: int
    pos: int


STATE_EMPTY = RobotState(SENSOR_NONE, DRIVE_NONE, 0, 0)


class QueueHandler(logging.Handler):
    def __init__(self, log_queue):
        super().__init__()
        self.log_queue = log_queue

    def emit(self, record):
        self.log_queue.put(record)


class ConsoleUi:
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

    def display(self, record):
        msg = self.queue_handler.format(record)
        self.scrolled_text.configure(state='normal')
        self.scrolled_text.insert(tk.END, msg + '\n', record.levelname)
        self.scrolled_text.configure(state='disabled')
        # Autoscroll to the bottom
        self.scrolled_text.yview(tk.END)

    def poll_log(self):
        # Check every 100ms if there is a new message in the queue to display
        while True:
            try:
                record = self.log_queue.get(block=False)
            except queue.Empty:
                break
            else:
                self.display(record)
        self.frame.after(100, self.poll_log)


class SerialConnection:
    def __init__(self, frm: ttk.Frame, update_state: UpdateFunction):
        self.port_var = StringVar()
        self.connect_var = StringVar()
        self.frm = frm
        self.connection_var = StringVar()
        self.init_vars()
        self.init_ui()
        self.update_state = update_state

    def init_vars(self):
        self.port_var.set("")
        self.connect_var.set("Open")

    def init_ui(self):
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
        port = self.port_var.get()
        connected = open_port(port, logger, self.update_state)
        self.connect_var.set("Close" if connected else "Open")


class DriveControl:
    def __init__(self, frm: ttk.Labelframe):
        self.frm = frm
        self.init_ui()

    def init_ui(self):
        # -S-
        # FPR
        # -H-
        ttk.Button(self.frm, text="Start", command=lambda: try_send('S', logger)).grid(column=1, row=0)
        ttk.Button(self.frm, text="Pause", command=lambda: try_send('P', logger)).grid(column=1, row=1)
        ttk.Button(self.frm, text="Rest", command=lambda: try_send('R', logger)).grid(column=2, row=1)
        ttk.Button(self.frm, text="Home", command=lambda: try_send('C', logger)).grid(column=1, row=2)
        ttk.Button(self.frm, text="Freeze", command=lambda: try_send('X', logger)).grid(column=0, row=1)
        # Needed for space between the buttons
        ttk.Label(self.frm, text="").grid(column=1, row=3)
        # -W-
        # AMD
        # -B-
        ttk.Button(self.frm, text="Forward", command=lambda: try_send('W', logger)).grid(column=1, row=4)
        ttk.Button(self.frm, text="Right", command=lambda: try_send('D', logger)).grid(column=2, row=5)
        ttk.Button(self.frm, text="Manuel", command=lambda: try_send('M', logger)).grid(column=1, row=5)
        ttk.Button(self.frm, text="Backward", command=lambda: try_send('B', logger)).grid(column=1, row=6)
        ttk.Button(self.frm, text="Left", command=lambda: try_send('A', logger)).grid(column=0, row=5)


def create_image(path: str, flip=False) -> PhotoImage:
    img = Image.open(path).convert("RGBA").resize((80, 80))
    if flip:
        img = img.transpose(Image.Transpose.FLIP_LEFT_RIGHT)
    return PhotoImage(img)


class StateControl(tk.Frame):

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
        self.init_ui()

    def update_state(self, state_tuple: StateTuple):
        self.set_state(RobotState(state_tuple[0], state_tuple[1], state_tuple[2], state_tuple[3]))

    def set_state(self, state: RobotState):
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
        self.canvas = tk.Canvas(self)
        self.led_left = self.canvas.create_rectangle(30, 10, 120, 80)
        self.led_center = self.canvas.create_rectangle(150, 10, 240, 80)
        self.led_right = self.canvas.create_rectangle(270, 10, 370, 80)

        self.drive_left = self.canvas.create_image(130, 150)
        self.drive_straight = self.canvas.create_image(200, 150)
        self.drive_right = self.canvas.create_image(270, 150)

        self.set_state(STATE_EMPTY)

        self.canvas.pack(fill=tk.BOTH, expand=1)


def exit_handler():
    close_port()


def sigint_handler(signal, frame):
    exit_handler()
    print('Interrupted: Close')
    sys.exit(0)


def create_ui(root: tk.Tk):
    frm = ttk.Frame(root, padding=10)
    frm.grid()
    ser_frame = ttk.Frame(frm)
    ser_frame.grid(sticky=tk.N)
    console_frame = ttk.Labelframe(frm, text="Console")
    console_frame.grid(row=0, column=1)
    ConsoleUi(console_frame)
    drive_frame = ttk.Labelframe(frm, text="Drive")
    drive_frame.grid(row=1, column=0, sticky=tk.NSEW)
    DriveControl(drive_frame)
    ex = StateControl(frm)
    ex.grid(row=1, column=1)
    SerialConnection(ser_frame, ex.update_state)


def main() -> None:
    atexit.register(exit_handler)
    signal.signal(signal.SIGINT, sigint_handler)
    logging.basicConfig(level=logging.INFO)
    root = tk.Tk()
    root.title("Robot Control")
    create_ui(root)
    root.mainloop()
    exit_handler()


if __name__ == '__main__':
    main()
