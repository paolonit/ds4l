# DualShock 4 Linux
##### By Andrew Myers, Tom Paoloni, and Quinlan Deval
This project allows you to map a DualShock 4 (Playstation 4) controller to keyboard input over USB. This can be used to play games, or even perform normal computer tasks with a controller rather than the standard keyboard. By configuring the layout of keys, you can specify how to make the controller fit your needs best.


## Install
In order to use this project, you must install [libxdo-dev](https://www.semicomplete.com/projects/xdotool/). Xdo is a tool used for simulating keyboard input and is used for generating output from controller button presses. It can be installed using the following command:
```
apt-get install libxdo-dev
```
To make the project run `make` and then to run the module and driver run `make init`


## Configuration
To specify how the layout should be setup, go to the *key_map.config* file. Here, just type in the desired keyboard input for the appropriate DSL4 buttons. Save the file, and restart the program if necessary.


## Usage
Once the configuration file is setup, just plug in the controller using USB and you are ready to go! The computer should now be reading the controller input as keyhboard input.
