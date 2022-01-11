#!/usr/bin/python3
from threading import Thread
import time
import os
import serial
import queue

import logging
logger = logging.getLogger(__name__)

class ControllerUnresponsive(Exception):
    def __init__(self, expression, message):
        self.expression = expression
        self.message = message

class SerialManager:
    def __init__(self, path_to_serial=None, timeout=1):
        if path_to_serial is None:
            path_to_serial = self.find_serial_lightcontroller()
        self.com = serial.Serial(path_to_serial, 115200, timeout=timeout)
        self.com.write(bytes("set echo off\r \r", 'utf-8'))
        self.com.flush()
        time.sleep(0.1)
        self.com.reset_input_buffer()

    def __del__(self):
        logger.debug("SerialManager destruction")
        self.com.write(bytes("set echo on\r",'utf-8'))
        self.com.close()

    def test_serial_port(self, path):
        logger.debug("Try Serial: {}".format(path))
        com = serial.Serial(path, 115200, timeout=1)
        com.write(bytes('identity \r', 'utf-8'))
        for i in range(1, 3):
            s = com.readline()
            if s.find(bytes('[OK]: Lightcontroller Software', 'utf-8')) != -1:
                return True
        return False

    def find_serial_lightcontroller(self):
        files = os.listdir('/dev')
        logger.info("Try to find Serial")
        for port in files:
            if 'ttyUSB' in port:
                if self.test_serial_port('/dev/{}'.format(port)):
                    logger.info("Found Serial")
                    return '/dev/' + port
        raise ValueError("Lightcontroller serial connection not found")

    def write(self, message):
        self.com.reset_input_buffer()
        logger.debug("Message: {}".format(message))
        counter = 0
        while True:
            self.com.write(bytes(message, 'utf-8'))
            self.com.flush()
            s = self.com.readline()
            if s.find(bytes("[OK]", 'utf-8')) != -1:
                logger.debug("Recieved: {}".format(s))
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

class StopLightController(Exception): pass

def Check_abortqueue(abortqueue, commandqueue, returnqueue, delay=0):
    if not abortqueue.empty() or delay != 0:
        try:
            message = abortqueue.get(timeout=delay)
            logger.debug("Got abort queue message")
            while not commandqueue.empty():
                commandqueue.get()
            if "close" in message:
                raise StopLightController
            returnqueue.put("ready")
            return True
        except queue.Empty:
            pass
    return False


def lightcontrollerbackend(commandqueue, abortqueue, returnqueue, path_to_serial=None, timeout=1):
    com = SerialManager(path_to_serial=path_to_serial, timeout=timeout)
    logger.info("LightController Backend initialised")
    returnqueue.put("ready")
    try:
        while True:
            Check_abortqueue(abortqueue, commandqueue, returnqueue)
            command = commandqueue.get()
            if not Check_abortqueue(abortqueue, commandqueue, returnqueue, delay=command['delay']):
                ret = com.write(command['message'])
                if command['return'] is True:
                    returnqueue.put(ret)
    except StopLightController:
        pass
    del com
    logger.info("Stop Lightcontrollerbackend")

class LightController:
    def __init__(self, commandqueue=None, abortqueue=None, returnqueue=None, path_to_serial=None, timeout=1):
        self.commandqueue = commandqueue
        if self.commandqueue is None:
            self.commandqueue = queue.Queue()
        self.abortqueue = abortqueue
        if self.abortqueue is None:
            self.abortqueue = queue.Queue()
        self.returnqueue = returnqueue
        if self.returnqueue is None:
            self.returnqueue = queue.Queue()
        self.bthread = Thread(target=lightcontrollerbackend, args=(self.commandqueue, self.abortqueue, self.returnqueue, path_to_serial, timeout, ))
        self.bthread.start()
        self.mode = 1
        self.step = 4
        self.timeout = 4 * 60 * 60 * 1000 # 4 hours in us
        message = self.returnqueue.get(timeout=20)
        if not "ready" in message:
            raise ValueError("Backend took more than 20 seconds to initialise")
        logger.info("LightController Frontend initialised")

    def __del__(self):
        self.stop_bthread()

    def stop_bthread(self):
        logger.debug("destroy LightController")
        self.abortqueue.put("close ")
        logger.debug("send close")
        self.write("identity \r")
        logger.debug("send identity, wait for closing")
        self.bthread.join()
        logger.info("LightController Backend stopped")

    def write(self, message, delay=0, retexpected=False):
        writedic = {}
        writedic['message'] = message
        writedic['delay'] = delay
        writedic['return'] = retexpected
        self.commandqueue.put(writedic)
        if retexpected:
            try:
                return self.returnqueue.get(timeout=300)
            except queue.empty:
                self.stop_bthread()
                raise RuntimeError("Backend of Lightcontroller not responsive. stopped")

    def abortprevious(self):
        self.abortqueue.put("abort")
        message = self.write("bla",delay=0, retexpected=True)
        if not "ready" in message:
            raise ValueError("Backend not properly reset")
    
    def backgroundthreadalive(self):
        return self.bthread.is_alive()

    @property
    def atx(self):
        s = self.write("status\r", retexpected=True)
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
        self.write(s)

    @property
    def mode(self):
        return self.__mode

    @mode.setter
    def mode(self, mode:int):
        self.__mode = mode
        s = "set mode {}\r".format(mode)
        self.write(s)

    @property
    def step(self):
        return self.__step

    @step.setter
    def step(self, step:int):
        self.__step = step
        s = "set step {}\r".format(step)
        self.write(s)

    @property
    def timeout(self):
        return self.__timeout

    @timeout.setter
    def timeout(self, timeout:int):
        self.__timeout = timeout
        s = "set timeout {}\r".format(timeout)
        self.write(s)

    def set_rgb(self, r:int, g:int, b:int, delay=0):
        if r >255 or g>255 or b>255:
            raise ValueError("RGB values need to be between 0-255!")
        s = "set rgb {r} {g} {b}\r".format(r=r, g=g, b=b)
        self.write(s, delay=delay)

    def set_rgb_limit(self, r:int, g:int, b:int, offset:int, length:int, mode=-1, delay=0):
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
        self.write(s, delay=delay)

    def set_lights(self, on:bool, delay=0):
        if self.atx is not on:
            s = "set lights "
            if on:
                s = s + "on\r"
            else:
                s = s + "off\r"
            self.write(s, delay=delay)
 
if __name__ == '__main__':  
    import time  
    #backend = SerialBackend('/dev/ttyUSB0')  
    #backend.clearup()  
    #time.sleep(1) 
    logging.basicConfig(format='%(asctime)s - %(name)s - %(levelname)s : %(message)s', level=logging.DEBUG)
    lights = LightController()  
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
    lights.set_lights(False, delay=5)  
    while lights.atx:  
        time.sleep(0.5)  
    print("atx on")  
    lights.set_lights(True)
    time.sleep(5)
    lights.set_rgb(255, 255, 255)
    time.sleep(1)
    del lights
#print("asdf")
