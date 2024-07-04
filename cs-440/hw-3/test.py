#!/user/bin

from csp_tests import *
from csp_ec_tests import *

t = CSPTests()

t.testCSP_4Colors()
print("Test 1 Passed")
t.testCSP_3Colors()
print("Test 2 Passed")
t.testCSP_2ColorsIsUnsolvable()
print("Test 3 Passed")
t.testCSP_2Colorsv2()
print("Test 4 Passed")
t.testCSP_2Colorsv3()
print("Test 5 Passed")
t.testCSP_2ColorsCount()
print("Test 6 Passed")
t.testCSP_2ColorsWantAll()
print("Test 7 Passed")
t.testCSP_2ColorsWantTooMany()
print("Test 8 Passed")

print("Normal Tests Passed!")

te = CSP_EC_Tests()

te.testCSPStats_2ColorsIsUnsolvable()
print("Test 1 Passed")
te.testCSPStats_2Colorsv2()
print("Test 2 Passed")
te.testCSPStats_2Colorsv3()
print("Test 3 Passed")
te.testCSPStats_2ColorsCount()
print("Test 4 Passed")

print("EC Tests Passed!")