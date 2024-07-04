#!/user/bin

# ref: https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm


def extendad_gba(a,b):
	old_r = a
	r = b
	old_s = 0
	s = 1
	old_t = 1
	t = 0

	while (r != 0):
		quotent = old_r // r

		(old_r, r) = (r, old_r - quotent * r)
		(old_s, s) = (s, old_s - quotent * s)
		(old_t, t) = (t, old_t - quotent * t)

	print("Bez coef:"+str(old_s)+" "+str(old_t))
	print("gcd: "+str(old_r))
	print("quotents by gcd: "+str(t)+" "+str(s))
	print()

def brute_force(a,b,m):
	for i in range(0,b):
		if (a*i) % b == m:
			return i

print("1. ")
extendad_gba(24140, 16762)
extendad_gba(5655, 12075)
print("2. ")
extendad_gba(19,26)
print(str((11*19)%26))

extendad_gba(17,101)
print(str((6*17)%101))

extendad_gba(1234,4321)
print(str(brute_force(1234,4321,1)))
print(str((-1082*1234)%4321))
print("3. ")
for i in range(1,16):
	print(str(i))
	extendad_gba(i,16)
	print(str(brute_force(i, 16, 1))+"\n")

print("5.")


import re
primes = list()
val = 2923
for i in range(1,val):
	if re.match(r'^1?$|^(11+?)\1+$', '1' * i) == None:
		primes+=[i]
def print_prime_factors(n):
	for a in primes:
		if a >= n:
			break
		for b in primes:
			if b >= n:
				break
			if a*b == n:
				print("prime factors of "+str(n)+" are: "+str(a)+" "+str(b))
				break

def print_factors(n):
	factors = list()
	for i in range(1,n):
		if n % i == 0:
			factors += [i]
	print("factors of "+str(n)+" are "+str(factors))
#2329
print_prime_factors(val)
print_factors(val)
