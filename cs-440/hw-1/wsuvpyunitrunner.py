# reference: https://github.com/python/cpython/blob/master/Lib/unittest/runner.py

import io
import os
import sys
import time
import json
from contextlib import contextmanager

import unittest
from unittest import result
from unittest import TextTestResult
from inspect import isclass

_defaultCategory = 'Correctness'
# Don't forget @functools.wraps(f) to maintain doc strings and stuff

def score(points):
    """Annotation to configure the score"""
    def decorator(f):
        def wrappedTest(testCase):
            print("Adding annotation score(%s)" % points)
            testCase.setPotentialPoints(points)
            f(testCase)
        return wrappedTest
    return decorator

def secret(f):
    """Annotation to denote that a test's output and score should be hidden"""
    def wrappedTest(testCase):
        print("Adding annotation @secret")
        testCase.markSecret()
        # Change the file descriptor out from under the standard definitions to stop output temporarily.
        with file_redirected(file=sys.stdout, to=os.devnull), file_redirected(file=sys.stderr, to=os.devnull):
            f(testCase)

    return wrappedTest

def stage(obj, name):
    """Annotation to denote the semantic feedback stage of a test method or test case class"""
    if isclass(obj):
        # TODO: decorate class
        return obj
    else:
        # TODO: decorate test method
        return obj

def category(name):
    """Annotation to denote the category of a test method or test case class"""
    def decorator(obj):
        if isclass(obj):
            return _categoryClass(obj, name)
        else:
            return _categoryMethod(obj, name)
    return decorator

def _categoryClass(c, name):
    """Annotation to denote the category of a test class"""
    print("in category class decorator")
    print("Adding annotation category(%s) to class %s" % (name, c))
    c.category = name
    return c

def _categoryMethod(f, name):
    """Annotation to denote the category of a test method"""
    def wrappedTest(testCase):
        print("Adding annotation category(%s)" % name)
        testCase.category = name
        f(testCase)
    return wrappedTest

def hint(message):
    """Annotation to configure a custom hint for a test"""
    def decorator(f):
        def wrappedTest(testCase):
            print("Adding hint(%s) to %s" % (message, testCase._testMethodName))
            testCase.hint = message
            f(testCase)
        return wrappedTest
    return decorator

def stage(stageName):
    """Annotation to configure a custom stage assignment for a test"""
    def decorator(f):
        def wrappedTest(testCase):
            print("Adding stage(%s) to %s" % (stageName, testCase._testMethodName))
            testCase.stage = stageName
            f(testCase)
        return wrappedTest
    return decorator

# Utility functions for deriving automatic stage and label names from a test object

def stageFromTest(test):
    if hasattr(test, 'stage'):
        return test.stage
    
    namestr = type(test).__name__
    if namestr.endswith("Tests"):
        namestr = namestr[:-len("Tests")]
    return namestr

def labelFromTest(test):
    namestr = test._testMethodName
    if namestr.startswith("test"):
        namestr = namestr[len("test"):]
    return namestr

def hintForTest(test, err):
    if hasattr(test, 'hint'):
        return test.hint
    return simpleHintFromErr(err)

def simpleHintFromErr(err):
    _, value, _ = err
    return str(value)

def categoryForTest(test):
    if hasattr(test, 'category'):
        return test.category

    return _defaultCategory


# Extension of TestCase with some useful helpers for registering feedback outside of the normal
# success/failure/error codepaths.

class WSUVTestCase(unittest.TestCase):
    
    def __init__(self, methodName='runTest'):
        super().__init__(methodName)
        self.isSecret = False

    def markSecret(self):
        self.isSecret = True

    def setPotentialPoints(self, potentialPoints):
        self.potentialPoints = potentialPoints

    def registerSemanticMetadata(self, value):
        self.registerSemanticMetadata(
            stageFromTest(self),
            labelFromTest(self),
            value
        )

    def registerSemanticMetadata(self, stage, label, value):
        self._outcome.result.registerSemanticMetadata(stage, label, value)

    def registerSemanticSuccessMetadata(self, stage, label):
        self._outcome.result.registerSemanticSuccessMetadata(stage, label)
    
    def registerSemanticFailureMetadata(self, stage, label, optionalHint):
        self._outcome.result.registerSemanticSuccessMetadata(stage, label, optionalHint)


# Extension of TextTestResult that builds up semantic JSON in a dictionary

class WSUVTextTestResult(TextTestResult):
    def __init__(self, *args, **kwargs):
        super(WSUVTextTestResult, self).__init__(*args, **kwargs)

        # This will be just like the failures and errors members
        self.successes = []

        self._allSemanticResults = dict()

        self._allSemanticResults["_presentation"] = "semantic"
        self._allSemanticResults["stages"] = []

    def _addItemToStage(self, stage, label, data):
        self._addStageIfNotExists(stage)
        self._allSemanticResults[stage][label] = data

    def _addStageIfNotExists(self, stageName):
        if stageName not in self._allSemanticResults["stages"]:
            self._allSemanticResults["stages"].append(stageName)
            self._allSemanticResults[stageName] = dict()

    def registerSemanticFailureMetadata(self, stage, label, optionalHint=""):
        meta = { "passed": False, "hint": optionalHint }
        self._addItemToStage(stage, label, meta)

    def registerSemanticSuccessMetadata(self, stage, label):
        meta = { "passed": True }
        self._addItemToStage(stage, label, meta)

    def registerSemanticMetadata(self, stage, label, value):
        self._addItemToStage(stage, label, value)

    def addSuccess(self, test):
        super(WSUVTextTestResult, self).addSuccess(test)

        self.successes.append(test)
        if not (hasattr(test, 'isSecret') and test.isSecret):
            self.registerSemanticSuccessMetadata(stageFromTest(test), labelFromTest(test))
        else:
            print('hiding secret success')

    def addError(self, test: unittest.case.TestCase, err) -> None:
        super(WSUVTextTestResult, self).addError(test, err)

        if not (hasattr(test, 'isSecret') and test.isSecret):
            self.registerSemanticFailureMetadata(stageFromTest(test), labelFromTest(test), hintForTest(test, err))
        else:
            print('hiding secret error')


    def addFailure(self, test: unittest.case.TestCase, err) -> None:
        super(WSUVTextTestResult, self).addFailure(test, err)
        
        if not (hasattr(test, 'isSecret') and test.isSecret):
            self.registerSemanticFailureMetadata(stageFromTest(test), labelFromTest(test), hintForTest(test, err))
        else:
            print('hiding secret failure')

# A TextTestRunner that opens a config file before testing and uses the
# WSUVTextTestResult class to access semantic feedback

def allOrNone(iterable):
    return not any(iterable) or all(iterable)

class WSUVTextTestRunner(unittest.TextTestRunner):
    resultclass = WSUVTextTestResult

    def __init__(self, **kwargs):
        """Construct a TextTestRunner.

        Subclasses should accept **kwargs to ensure compatibility as the
        interface changes.
        """
        super(WSUVTextTestRunner, self).__init__(**kwargs)


    def run(self, test):
        "Run the given test case or test suite."

        # Load the json file first, so a student can't overwrite it...
        with open('wsuvtest.json') as fin:
            config = json.load(fin)

        result = super(WSUVTextTestRunner, self).run(test)


        totalPoints = 0
        maxTotalPoints = 0
        scores = {}
        
        allTests = [failure[0] for failure in result.failures] + [error[0] for error in result.errors] + result.successes

        categories = list(set([categoryForTest(test) for test in allTests]))
        
        if not all([category in categories for category in config['scores'].keys()]):
            print('json keys', config['scores'].keys())
            print('test categories', categories)
            raise ValueError('All wsuvtest.json test categories should match the set of categories found on test cases.')

        useEqualWeighting = not any([hasattr(test, 'potentialPoints') for test in allTests])
        if not allOrNone([hasattr(test, 'potentialPoints') for test in allTests]):
            raise ValueError('All tests should have an @score annotation or none should.')

        for category in categories:
            categoryTests = [test for test in allTests if categoryForTest(test) is category]
            maxCategoryPoints = config['scores'][category]
            categoryTestCount = len(categoryTests)
        
            totalPotentialPoints = sum([test.potentialPoints if hasattr(test, 'potentialPoints') else 0 for test in categoryTests])
            if not useEqualWeighting and maxCategoryPoints != totalPotentialPoints:
                raise ValueError('All `%s` tests @score annotations should sum to the max score.' % category)

            if useEqualWeighting:
                failed, errored = 0,0
                failedNames = []
                erroredNames = []
                if not result.wasSuccessful():
                    failedNames = [failure._testMethodName for failure, _ in result.failures if categoryForTest(failure) is category]
                    failed = len(failedNames)
                    erroredNames = [error._testMethodName for error, _ in result.errors if categoryForTest(error) is category]
                    errored = len(erroredNames)
                # print(category, 'equal weighting', 'failed', failedNames, 'errored', erroredNames, 'all', categoryTests)
                categoryScore = (categoryTestCount-failed-errored)*1.0*maxCategoryPoints/categoryTestCount
            else:
                categoryScore = sum([getattr(test, 'potentialPoints', 0) for test in result.successes if categoryForTest(test) is category])

            maxTotalPoints += maxCategoryPoints
            totalPoints += categoryScore
            scores[category] = round(categoryScore, 1)

        return "\n".join([
            "Total points %.1f out of %.1f"%(totalPoints, maxTotalPoints),  # Normal text piece
            json.dumps(result._allSemanticResults),                         # Semantic Feedback (with stages) piece
            json.dumps({'scores': scores})                                  # Scoring piece
        ])

@contextmanager
def file_redirected(file=None, to=os.devnull):
    """Redirect `file` to `to` for the duration of the context.
    
    The object won't be changed directly. Instead the underlying file descriptor will be swapped using dup2.
    Copied from https://stackoverflow.com/questions/4675728/redirect-stdout-to-a-file-in-python/22434262#22434262
    """

    def fileno(file_or_fd):
        fd = getattr(file_or_fd, 'fileno', lambda: file_or_fd)()
        if not isinstance(fd, int):
            raise ValueError("Expected a file (`.fileno()`) or a file descriptor")
        return fd

    if file is None:
       file = sys.stdout

    file_fd = fileno(file)
    # copy stdout_fd before it is overwritten
    #NOTE: `copied` is inheritable on Windows when duplicating a standard stream
    with os.fdopen(os.dup(file_fd), 'wb') as copied: 
        file.flush()  # flush library buffers that dup2 knows nothing about
        try:
            os.dup2(fileno(to), file_fd)  # $ exec >&to
        except ValueError:  # filename
            with open(to, 'wb') as to_file:
                os.dup2(to_file.fileno(), file_fd)  # $ exec > to
        try:
            yield file # allow code to be run with the redirected stdout
        finally:
            # restore stdout to its previous value
            #NOTE: dup2 makes stdout_fd inheritable unconditionally
            file.flush()
            os.dup2(copied.fileno(), file_fd)  # $ exec >&copied

if __name__ == "__main__":
    import sys
    
    test_modules = unittest.defaultTestLoader.discover(start_dir='.',
                                                       pattern='*tests.py',
                                                       top_level_dir=None)
    try:
        sys.stdout = open('wsuv.autograder.out', 'wt')

        runner = WSUVTextTestRunner(verbosity=5)
        result = runner.run(test_modules)
        print(result)
    finally:
        sys.stdout.close()
