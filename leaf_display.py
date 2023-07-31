
# linux: sudo ip link set can0 up type can bitrate 500000

import can
from tkinter import *
import time

counter = 0
timeout_sec = 1.0

def update():
    global counter
    counter += 1

    msg = bus.recv(timeout_sec)
    try:

        if msg.arbitration_id == 0x1DB:
            print("soc",msg)

    except AttributeError:
        pass

    l1.config(text = counter)
    l2.config(text = counter)
    win.after(10, update)

bus = can.interface.Bus(bustype='socketcan', channel='can0', bitrate=500000)

win=Tk()

win_width = int(win.winfo_screenwidth() / 3)
win_height = int(win.winfo_screenheight() / 3)

win_columns = 2
win_rows = 10

spacing_x = int(win_width / win_columns)
spacing_y = int(win_height / win_rows)

size = str(int(win_width)) + 'x' + str(int(win_height))

win.geometry(size)

win.eval('tk::PlaceWindow . center')

font_size = int(win_height / win_rows)

l1 = Label(win, text = counter, justify=RIGHT)
l1.place(x = spacing_x * 0, y= spacing_y * 0)
l1.config(font =("Courier", font_size))

l1_label = Label(win, text = 'voltage', justify=LEFT)
l1_label.place(x = spacing_x * 1, y= spacing_y * 0)
l1_label.config(font =("Courier", font_size))

l2 = Label(win, text = counter, justify=RIGHT)
l2.place(x = spacing_x * 0, y= spacing_y * 1)
l2.config(font =("Courier", font_size))

l2_label = Label(win, text = 'current', justify=LEFT)
l2_label.place(x = spacing_x * 1, y= spacing_y * 1)
l2_label.config(font =("Courier", font_size))

win.after(1000, update)
win.mainloop()
bus.shutdown()