import subprocess
from argparse import ArgumentParser
import time


class DuplicatedTestError(Exception):
    def __init__(self, name):
        self._name = name

    def __str__(self):
        return f"Test {self._name} is duplicated"


class UnitUnderTest:
    def __init__(self, executable):
        self._executable = executable
    
    def __enter__(self):
        self._process = subprocess.Popen(
            self._executable,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

    def __exit__(self, *args):
        try:
            out, err = self._process.communicate(timeout=1)
        except Exception:
            self._process.kill()
            try:
                out, err = self._process.communicate(timeout=1)
            except Exception:
                out = "Error recovering stdout"
                err = "Error recovering stderr"
        
        if out:
            print(f"  * UUT stdout:\n{out}")
        if err:
            print(f"  * UUT stderr:\n{err}")


class Test:
    def __init__(self, func):
        self._func = func
        self._prepare = None
        self._cleanup = None

    def __call__(self, *args, **kwargs):
        return self._func(*args, **kwargs)
    
    def prepare(self):
        def decorator(prepare_func):
            nonlocal self
            self._prepare = prepare_func
            return prepare_func

        return decorator
    
    def cleanup(self):
        def decorator(cleanup_func):
            nonlocal self
            self._cleanup = cleanup_func
            return cleanup_func

        return decorator

    def run_prepare(self):
        if self._prepare is not None:
            self._prepare()
    
    def run_cleanup(self):
        if self._cleanup is not None:
            self._cleanup()

class TestRunner:

    def __init__(self, uut_executable):
        self._prepare = {}
        self._tests = {}
        self._cleanup = {}
        self._uut = UnitUnderTest(uut_executable)

    # Registers a new test in the runner, name is infered from the function name
    def test(self):
        def decorator(test_func):
            nonlocal self

            if test_func.__name__ in self._tests:
                raise DuplicatedTestError(test_func.__name__)
            
            self._tests[test_func.__name__] = Test(test_func)

            return self._tests[test_func.__name__]
        
        return decorator
    
    
    # Runs all the registered tests, cleaning up after each test
    def run(self):
        for name, test in self._tests.items():
            try:
                test.run_prepare()

                with self._uut:
                    time.sleep(0.1)
                    try:
                        print(f"[{name}] Running...")
                        result = test()
                        print(f"[{name}] Succesfull!")
                        if result is not None:
                            print(f"  * Result: {result}")
                    except Exception as reason:
                        print(f"[{name}] Failed!")
                        print(f"  * Reason: {reason}")

                test.run_cleanup()
            except KeyboardInterrupt:
                print(f"[{name}] Keyboard Interrupt. Aborted.")



parser = ArgumentParser(
    prog="test",
    description="run multiple simulator tests on a target executable"
)

parser.add_argument(
    "-uut",
    "--executable",
    required=True,
    help="full path to the target executable"
)

args = parser.parse_args()

runner = TestRunner(args.executable)
