# Project Robot        

> Small program for a line following robot which can accept a variety of instructions via a bluetooth connection.

---
## About

The project robot is based on an **ATmega328p** board which is equipped with three reflective optical sensors which are 
Used to follow a black line on a test ground. Furthermore, the robot was equipped with a shift register connected to 
three LEDs.

<h1 align="center">
<img src="images/robot.png" alt=" " width="250" height="200">
</h1>

---
## Modes

The current mode is selected by sending the related key via a serial connection to the board.

|      Mode      |    Key     | Description                                                                              |
|:--------------:|:----------:|------------------------------------------------------------------------------------------|
|     Drive      |     S      | The robot should drive 3 rounds around the track, then stop and reset on the start field |
|     Pause      |     P      | The robot should do nothing and wait for the key to be entered again to leave this mode. |
|      Home      |     C      | Finish this round, then drive back to the start field and reset.                         |
|     Freeze     |     X      | Save state! The robot reacts to nothing and waits for external reset.                    |
|      Help      |     ?      | Prints help text to the serial, if located on the start field.                           |
|      Rest      |     R      | Resets the robot after 5 seconds                                                         |
| Manual Control |     M      | Enables manual control for the robot                                                     |
|   UI Connect   |     Y      | Connects the ui (internally used)                                                        |
| UI Disconnect  |     Q      | Disconnects the ui (internally used)                                                     |
|  Manual Drive  | W, A, B, D | Drive forward, left, backward or right in manual control.                                |

### Drive
If the robot is placed on the stating field, it should start to blink in a frequency of 5 HZ. If an `S` is entered, the
robot should start to drive 3 rounds around the track stop on the starting field again and reset itself in the end after
5 seconds.

### Pause
In any position in the driving action if a `P` is entered the robot should stop driving and wait for key to be entered
again to leave this mode and drive again.

### Home
In any position in the driving action if a `C` is entered the robot should return to the starting field and reset itself
nearly instant.

### Freeze
If a `X` is entered the robot should stop what ever he is doing, don't react to any input and wait for external reset.

### Rest
If a `P` is entered the robot should reset itself after 5 seconds and  don't react to any input after this mode was
activated.

### Manual Control
If a `M` is entered the robot enters the manual driving mode and can be controlled by entering `W, A, B, D` how
explained above. If the key is entered again the previous mode will be activated again.

---
## User Interface

Implemented via python with the help of [tkinter](https://docs.python.org/3/library/tkinter.html). Can be used to send
any text to the robot via a serial connection. Some "shortcut" buttons are also provided for all existing modes.<br>
Furthermore the current state of the field sensors can be seen and the direction that the robot is currently driving.
This is possible thanks to an ongoing message exchange between the robot and the user interface. Created with the help
of the python library [pySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html). Any message send from the
robot to the serial port will also be displayed in the console window on the left upper side. Additionally, the ui
contains several buttons that can be used to activate the manual driving and manual drive the robot.

<h1 align="center">
<img src="images/user_interface.png" alt=" " width="666" height="356">
</h1>

---
### Updates

At the start of the connection the user interface send a `Y` message to the robot, which indicates it that it can send
state update messages. A state update message contains information of the current state of the robot (Field sensors,
driving direction, etc. ...). After the user interface is closed an `Q` will be sent to the robot, which indicates that
no more ui updates are needed.

---
## Program Execution
This project makes heavy use of the "Makefile" shame for compilation, link and flashing to the board. 
Additionally, we use [Doxygen](https://doxygen.nl/) to generate the corresponding documentation for the **C** files.

The following targets are defined in the makefile:
- `all`
  - Default target
  - Performs `compile` `link` `flash` and `documentation`
- `compile`
  - Compiles all sources

- `link`
  - Creates ".o" files for every C file in `./out/` and creates ".hex" file also in `./out/`

- `flash`
  - Flashes the program on to the robot

- `documentation`
  - Creates doxygen docs at `./doc/`

- `clean`
  - Clears `./out/ ` directory, removes ".o", ".d" and ".hex" files

- `help`
  - Displays all possible actions with explanations

- `info`
  - Displays information about the project and the used files

- `indent`
  - Formats all source files (.c and .h) and puts them into a `./indent/` folder

- `cppcheck`
  - Performance a static analysis on all source and header files

- `try_connect`
  - Tries to establish a connection with a robot via bluetooth, if any robot is found it has to be selected manually

- `try_disconnect`
  - Tries to disconnect the connection to the robot if any is existent

---
## License
Copyright (c) 2022 Larson Schneider<br>
Licensed under the MIT license.