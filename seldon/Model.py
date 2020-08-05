import subprocess

class Model:
    def __init__(self):
        pass

    def predict(self, X, names=[], meta=[]):
        return subprocess.check_output(["bin/main"])
