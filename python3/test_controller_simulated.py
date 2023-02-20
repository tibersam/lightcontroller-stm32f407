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
