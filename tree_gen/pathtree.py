import sys

n = int(sys.argv[1])

if len(sys.argv) < 3: k = 2
else: k = int(sys.argv[2])

print(n, k)

f = open("pathtree_" + str(n) + "_" + str(k), "w")

to_close = 1
f.write("(")
n-=1
while n > 0:
	for _ in range(k-1):
		f.write("()")
	to_close += 1
	f.write("(")
	n -= k

for _ in range(to_close):
	f.write(")")
