from controller_simulated import decode


def test_decode_mode():
    assert decode("set mode 0") == "[OK]: Set stepmode\n"
    assert decode("set mode -1") == "[ERROR]: Stepmode only valid between 0-2\n"
    assert decode("set mode 3") == "[ERROR]: Stepmode only valid between 0-2\n"


def test_decode_limit():
    assert decode("set limit 0 0 0 10 5 1") == "[OK]: set limit rgb done\n"
    assert (
        decode("set limit -1 0 0 10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 256 0 0 10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 -1 0 10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 256 0 10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 -1 10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 256 10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 0 -10 5 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 0 10 -3 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 0 100 400 1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 0 10 5 -1")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )
    assert (
        decode("set limit 0 0 0 10 5 3")
        == """[ERROR]: Something went wrong!
[ERROR]: set limit r g b offset length mode\n"""
    )


def test_decode_step():
    assert decode("set step 1") == "[OK]: Set step length\n"
    assert decode("set step -1") == "[ERROR] Missing step argument\n"
    assert decode("set step 4394967296") == "[ERROR] Missing step argument\n"


def test_decode_rgb():
    assert decode("set rgb 1 1 1") == "[OK] set rgb values\n"
    assert decode("set rgb -1 1 1") == "[ERROR]: Value Error\n"
    assert decode("set rgb 256 1 1") == "[ERROR]: Value Error\n"
    assert decode("set rgb 1 -1 1") == "[ERROR]: Value Error\n"
    assert decode("set rgb 1 256 1") == "[ERROR]: Value Error\n"
    assert decode("set rgb 1 1 -1") == "[ERROR]: Value Error\n"
    assert decode("set rgb 1 1 256") == "[ERROR]: Value Error\n"


def test_timeout():
    assert decode("set timeout 5") == "[OK]: Set timeout\n"
    assert decode("set timeout -5") == "[ERROR]: Error in decoding timeout\n"
    assert decode("set timeout 4394967296") == "[ERROR]: Error in decoding timeout\n"


def test_lights():
    assert decode("set lights off") == "[OK]: lights off\n"
    assert decode("set lights on") == "[OK]: lights on\n"
    assert decode("set lights asdf") == "[ERROR]: lights only takes on|off\n"


def test_set_atx():
    assert decode("set atx on") == "[ATX] Send endable signal\n[OK]: ATX on\n"
    assert decode("set atx off") == "[OK]: remove enable signal\n[ATX]: Atx off\n"
    assert decode("set atx asdf") == ""


def test_set_uart5con():
    assert decode("set uart5con on") == "[OK]: Enable uart5 consol\n"
    assert decode("set uart5con off") == "[OK]: Disable uart5 consol\n"
    assert decode("set uart5con asdf") == "[ERROR]: uart5con on or off\n"
