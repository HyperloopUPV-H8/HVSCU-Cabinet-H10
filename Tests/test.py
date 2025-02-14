import sys, os
sys.path.append(os.path.join(os.path.dirname(__file__), "VirtualMCU", "src"))

from runner import runner
from vmcu.shared_memory import SharedMemory
from vmcu.pin import Pinout, DigitalOut
from vmcu.services.digital_out import DigitalOutService
from vmcu.assertions import *


@runner.test()
def led_toggle():
    TOGGLE_PERIOD = milliseconds(100 * 2)
    ALLOWED_SLACK = milliseconds(5)

    shm = SharedMemory("gpio__blinking_led", "state_machine__blinking_led")
    led = DigitalOutService(shm, Pinout.PA1)

    def led_turns_on():
        nonlocal led
        return led.get_pin_state() is DigitalOut.State.High

    def led_turns_off():
        nonlocal led
        return led.get_pin_state() is DigitalOut.State.Low

    #sync with board
    completes(
        wait_until_true(led_turns_on),
        before=(TOGGLE_PERIOD / 2) + ALLOWED_SLACK,
        msg="Sync fails"
    )

    for i in range(150):
        completes(
            wait_until_true(led_turns_off),
            before=(TOGGLE_PERIOD / 2) + ALLOWED_SLACK,
            after=(TOGGLE_PERIOD / 2) - ALLOWED_SLACK,
            msg="turns off"
        )
        completes(
            wait_until_true(led_turns_on),
            before=(TOGGLE_PERIOD / 2) + ALLOWED_SLACK,
            after=(TOGGLE_PERIOD / 2) - ALLOWED_SLACK,
            msg="turns on"
        )
        print("toggle", i)
    

runner.run() # Runs the tests, do not delete!
