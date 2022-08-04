## Project Robot        

<blockquote>
    <p>Small program for a line following robot which can accept a variety of instructions via a bluetooth connection.
</p>
</blockquote>

@tableofcontents

@section about About

The project robot is based on an **ATmega328p** board which is equipped with three reflective optical sensors which are 
used to follow a black line on a white test ground. Furthermore, the robot was equipped with a shift register connected to 
three LEDs.

<h1 align="center">
<img src="robot.png" alt=" " width="250" height="200">
</h1>

<blockquote style="color:orange">
    <p>All additional features, i.e. these that were not required by the original task,
are marked with orange font color on the main page.</p>
</blockquote>

---
@section actions Action Modes

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
|  Manual Drive  | W, A, B, D | Drive forward, left, backward or right in manual control.                                |
|   UI Connect   |     Y      | Connects the ui (internally used)                                                        |
| UI Disconnect  |     Q      | Disconnects the ui (internally used)                                                     |

@subsection actDrive Drive
In the main operation mode the robot should start on the @ref startingField "starting field" and
then drive 3 rounds around the @ref track "track". @n At the end it should stop on the starting field and
reset itself after 5 seconds. @n
For more information about the driving see the @ref drive "driving module".
For more information about the reset see: @ref secReset

@subsection actPause Pause
In any position in the driving action if a `P` is entered the robot should stop driving and wait for key to be entered 
again to leave this mode and drive again.

@subsection actHome Home
In any position in the driving action if a `C` is entered the robot should return to the starting field and reset itself 
nearly instant.

@subsection actFreeze Freeze
If a `X` is entered the robot should stop what ever he is doing, don't react to any input and wait for external reset.

@subsection actRest Rest
If a `R` is entered the robot should reset itself after 5 seconds and  don't react to any input after this mode was
activated. If it was currently driving it should instantly stop.@n
For more information about the reset see the @ref utility "utility module".
<span style="color:orange">
@subsection actManual Manual Control
If a `M` is entered the robot enters the manual driving mode and can be controlled by entering `W, A, B, D` how 
explained above. If the key is entered again the previous mode will be activated again.
</span>

---
@section ui User Interface
<span style="color:orange">
Implemented via python with the help of [tkinter](https://docs.python.org/3/library/tkinter.html). Can be used to send 
any text to the robot via a serial connection. Some "shortcut" buttons are also provided for all existing modes.<br>
Furthermore the current state of the field sensors can be seen and the direction that the robot is currently driving.
This is possible thanks to an ongoing message exchange between the robot and the user interface. Created with the help 
of the python library [pySerial](https://pyserial.readthedocs.io/en/latest/pyserial.html). Any message send from the
robot to the serial port will also be displayed in the console window on the left upper side. Additionally, the ui 
contains several buttons that can be used to activate the manual driving and manual drive the robot.

<h1 align="center">
<img src="user_interface.png" alt=" " width="666" height="356">
</h1>

@subsection secUIUpdate State Updates

At the start of the connection the user interface send a `Y` message to the robot, which indicates it that it can send 
state update messages. A state update message contains information of the current state of the robot (Field sensors,
driving direction, etc. ...). After the user interface is closed an `Q` will be sent to the robot, which indicates that
no more ui updates are needed.
</span>

---
@section modules Modules
The structure of this project is divided into multiple modules each of which takes on different
tasks like driving, state control or reading from the sensors.
<blockquote>
- @subpage states
- @subpage drive
- @subpage sensor
- @subpage timers
- @subpage utility
- @subpage usart
- @subpage led
</blockquote>
@subsection secModSet Setup
A module contains functions, macro defs, structs and enums. All methods are prefixed with the
name of respective module.@n
Some but not all modules contain "clean" and "init" (setup) functions, former sets some used registers
to there default values, latter sets these registers to the value needed for the robot to
function correctly. @n
The method @ref setup call all these before the main programm relly starts with the work.

---
## Program Execution
This project makes heavy use of the "Makefile" scheme for compilation, link and flashing to the board. 
Additionally, we use [Doxygen](https://doxygen.nl/) to generate the corresponding documentation for the **C** files.

To execute one of the following targets cust call 'make ***target name***'. If you call just 'make'
if will automatically performe the 'all' target.

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

- `force`
  - Same as `all` target but calls `clean` target beforehand

- `documentation`
  - Creates doxygen docs at `./doc/`

- `clean`
  - Clears `./out/ ` directory, removes ".o", ".d" and ".hex" files
<span style="color:orange">
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

</span>

---
## License
Copyright (c) 2022 Larson Schneider<br>
Licensed under the MIT license.