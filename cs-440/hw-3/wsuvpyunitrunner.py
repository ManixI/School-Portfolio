import unittest

class WSUVTestCase(unittest.TestCase):
    pass


def category(s):
    def wrapper(f):
        return f
    return wrapper
