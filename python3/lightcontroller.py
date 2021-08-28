
import serial
import logging
import time
import os

logger = logging.getLogger(__name__)

class ControllerUnresponsive(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = message

def test_serial_port(path):
    com = serial.Serial(path, 115200, timeout=1)
    com.write(bytes('identity \r', 'utf-8'))
    for i in range(1, 3):
        s = com.readline()
        if s.find(bytes('[OK]: Lightcontroller Software', 'utf-8')) != -1:
            return True
    return False

def find_serial_lightcontroller():
    files = os.listdir('/dev')
    for port in files:
        if 'ttyUSB' in port:
            if test_serial_port('/dev/' + port):
                return '/dev/' + port
    raise ValueError("Lightcontroller serial connection not found")

class SerialBackend:

    def __init__(self, path_to_serial=None, timeout=1):
        if path_to_serial is None:
            path_to_serial = find_serial_lightcontroller()
        self.com = serial.Serial(path_to_serial, 115200,timeout=timeout)
        #self.com.write(bytes("set echo off\r", 'utf-8'))
        self.write("set echo off\r \r")
        time.sleep(0.1)
        self.com.flush()

    def write(self, message):
        self.com.reset_input_buffer()
        logger.info("Message {}".format(message))
        counter = 0
        while True:
            self.com.write(bytes(message, 'utf-8'))
            self.com.flush()
            s = self.com.readline()
            if s.find(bytes("[OK]", 'utf-8')) != -1:
                return s
            print(s)
            logger.info("Tried to execute cmd {}, got {}".format(message, s))
            time.sleep(0.1)
            self.com.reset_input_buffer()
            if counter == 10:
                logger.critical("Controller not responding")
                raise ControllerUnresponsive("Controller is not responding in the way expected")
            counter = counter + 1
    def clearup(self):
        self.com.write(bytes("\r\n", 'utf-8'))
        self.com.flush()

class LightController:

    def __init__(self, serialbackend=None):
        if serialbackend is None:
            serialbackend = SerialBackend()
        self.backend = serialbackend
        self.backend.clearup()
        self.mode = 1
        self.step = 4
        self.timeout = 4 * 60 * 60 * 1000 # 4 hours in us

    def __del__(self):
        self.backend.write("set echo on\r")


    @property
    def atx(self):
        s = self.backend.write("status\r")
        if s.find(bytes("ATX on", 'utf-8')) != -1:
            return True
        if s.find(bytes("ATX off", 'utf-8')) != -1:
            return False
        logger.critical("Message not parsebel for atx: {}".format(s))
        raise ValueError("ATX message parsing error")

    @atx.setter
    def atx(self, state:bool):
        s = "set atx "
        if state:
            s = s + "on\r"
        else:
            s = s + "off\r"
        print(s)
        self.backend.write(s)

    @property
    def mode(self):
        return self.__mode

    @mode.setter
    def mode(self, mode:int):
        self.__mode = mode
        s = "set mode {}\r".format(mode)
        self.backend.write(s)

    @property
    def step(self):
        return self.__step

    @step.setter
    def step(self, step:int):
        self.__step = step
        s = "set step {}\r".format(step)
        self.backend.write(s)
    
    @property
    def timeout(self):
        return self.__timeout

    @timeout.setter
    def timeout(self, timeout:int):
        self.__timeout = timeout
        s = "set timeout {}\r".format(timeout)
        self.backend.write(s)

    def set_rgb(self, r:int, g:int, b:int):
        if r >255 or g>255 or b>255:
            raise ValueError("RGB values need to be between 0-255!")
        s = "set rgb {r} {g} {b}\r".format(r=r, g=g, b=b)
        self.backend.write(s)

    def set_rgb_limit(self, r:int, g:int, b:int, offset:int, length:int, mode=-1):
        if mode == -1:
            mode = self.mode
        if r >255 or g>255 or b>255:
            raise ValueError("RGB values need to be between 0-255!")
        s = "set limit {r} {g} {b} {offset} {length} {mode}\r".format(r=r,
                                                                      g=g,
                                                                      b=b,
                                                                      offset=offset,
                                                                      length=length,
                                                                      mode=mode)
        self.backend.write(s)

    def set_lights(self, on:bool):
        if self.atx is not on:
            s = "set lights "
            if on:
                s = s + "on\r"
            else:
                s = s + "off\r"
            self.backend.write(s)





if __name__ == '__main__':
    import time
    backend = SerialBackend('/dev/ttyUSB0')
    backend.clearup()
    time.sleep(1)
    lights = LightController(backend)
    if lights.atx is not True:
        lights.atx = True
    print("red")
    lights.set_rgb(255, 0, 0)
    time.sleep(10)
    lights.mode = 0
    lights.set_rgb(0, 255, 0)
    print("green")
    time.sleep(10)
    lights.mode = 2
    lights.step = 1
    lights.set_rgb(0, 0, 255)
    print("blue")
    time.sleep(10)
    lights.mode = 1
    lights.set_rgb(255, 255, 255)
    lights.set_rgb_limit(255, 10, 255, 60, 60)
    print("Lights on")
    time.sleep(10)
    print("atx off")
    lights.set_lights(False)
    while lights.atx:
        time.sleep(0.5)
    print("atx on")
    lights.set_lights(True)
