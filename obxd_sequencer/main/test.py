import math
import time
import datetime
from demo_opts import get_device
from luma.core.render import canvas


DISPLAY_CONFIG = ["--display", "ssd1306", "--i2c-port", "0", "--i2c-address", "0x3C", "--width", "128", "--height", "32"]

def do_nothing():
    pass

def main():
    time.sleep(1)
    print("Clearing device")
    device.clear()
    #device.command(167)
    time.sleep(3)
    #device.command(174)
    with canvas(device) as draw:
        print("Drawing rectangle")
        #device.command(175)
        #device.command(166)
        #device.command(0x26)
        #device.command(0x00)
        #device.command(0x01)
        #device.command(0x00)
        #device.command(0x07)
        #device.command(0x2F)
        print("Done!")
        time.sleep(3)
        draw.ellipse((0,0,128,64), fill="black")
        print("Goodbye")


if __name__ == "__main__":
    try:
        print("Configuring...")
        device = get_device(DISPLAY_CONFIG)
        print("Done!")
        #device.cleanup = do_nothing
        main()
    except KeyboardInterrupt:
        pass
