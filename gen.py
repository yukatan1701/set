import random

n = 10000
dist = n * 2
f = open("input.txt", 'w')
f.write(str(n) + ' ')
for _ in range(n):
	x = random.randint(-dist, dist)
	f.write(str(x) + ' ')
m = int(n / 2)
f.write(str(m) + ' ')
for _ in range(m):
	x = random.randint(-dist, dist)
	f.write(str(x) + ' ')
