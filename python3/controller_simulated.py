import logging
from typing import List

import parse
from pydantic import BaseModel, Field

logger = logging.getLogger(__name__)


class LED(BaseModel):
    r: int = 0
    g: int = 0
    b: int = 0


class Controller(BaseModel):
    mode: int = 0
    step: int = 0
    atx: bool = True
    echo: bool = False
    uart5con: bool = False
    timeout: int = 400000
    leds: List[LED] = Field(default=[LED()] * 432, max_items=432, min_items=432)


controller = Controller()


def decode_and_call(input: str, formatstring: str, callfunction):
    decode = parse.parse(formatstring, input)
    if decode is None:
        return None
    logger.debug(decode)
    ret = callfunction(decode.named)
    logger.debug(ret)
    return ret


def decode(input: str) -> str:
    ret = decode_and_call(input, "set mode {mode:d}", decode_mode)
    if ret is not None:
        return ret
    ret = decode_and_call(
        input,
        "set limit {r:d} {g:d} {b:d} {offset:d} {length:d} {mode:d}",
        decode_limit,
    )
    if ret is not None:
        return ret
    ret = decode_and_call(input, "set step {step:d}", decode_step)
    if ret is not None:
        return ret
    ret = decode_and_call(input, "set rgb {r:d} {g:d} {b:d}", decode_rgb)
    if ret is not None:
        return ret
    ret = decode_and_call(input, "set timeout {timeout:d}", decode_timeout)
    if ret is not None:
        return ret


def decode_mode(input):
    global controller
    if input["mode"] > 2 or input["mode"] < 0:
        return "[ERROR]: Stepmode only valid between 0-2\n"
    controller.mode = input["mode"]
    return "[OK]: Set stepmode\n"


def decode_limit(input):
    global controller
    if (
        input["r"] < 0
        or input["r"] > 255
        or input["g"] < 0
        or input["g"] > 255
        or input["b"] < 0
        or input["b"] > 255
        or input["offset"] < 0
        or input["length"] < 0
        or input["offset"] + input["length"] > len(controller.leds)
        or input["mode"] < 0
        or input["mode"] > 2
    ):
        return """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    for i in range(input["length"], input["offset"] + input["length"]):
        controller.leds[i].r = input["r"]
        controller.leds[i].g = input["g"]
        controller.leds[i].b = input["b"]
    return "[OK]: set limit rgb done\n"


def decode_step(input):
    global controller
    if input["step"] < 0 or input["step"] >= 2 ^ 32:
        return "[ERROR] Missing step argument\n"
    controller.step = input["step"]
    return "[OK]: Set step length\n"


def decode_rgb(input):
    global controller
    if "r" not in input:
        return "[ERROR]: Argument to short, Red missing for setrgb\n"
    if "g" not in input:
        return "[ERROR]: Argument to short, Green missing for setrgb\n"
    if "b" not in input:
        return "[ERROR]: Argument to short, Blue missing for setrgb\n"

    if (
        input["r"] < 0
        or input["r"] >= 255
        or input["g"] < 0
        or input["g"] >= 255
        or input["b"] < 0
        or input["b"] >= 255
    ):
        return "[ERROR]: Value Error\n"
    for led in controller.leds:
        led.r = input["r"]
        led.g = input["g"]
        led.b = input["b"]
    return "[OK] set rgb values\n"


def decode_timeout(input):
    global controller
    if input["timeout"] < 0 or input["timeout"] > 2 ^ 32:
        return "[ERROR]: Error in decoding timeout\n"
    controller.timeout = input["timeout"]
    return "[OK]: Set timeout\n"


def simulate_behavior(input: str) -> str:
    mode = parse.compile("set mode {mode:d}")
    ret = mode.parse(input)
    if ret is None:
        return ret
    return ret.named


if __name__ == "__main__":
    ret = simulate_behavior("set mode 0")
    print(ret)
    ret = simulate_behavior("set limit 0")
    print(ret)
