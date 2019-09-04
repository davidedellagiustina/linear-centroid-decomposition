import sys

# The tree is binary. Only even nodes have two children (but not all even nodes )
# A node i has two child: (i-1)/3 (if integer and odd) and i*2


def BP(node, adj, f):
    f.write("(")
    if node in adj: # internal node
        for n in sorted(adj[node]):
            BP(n, adj, f)
    f.write(")")

n = int(sys.argv[1])

edges = set()


for i in range(2, n):
    while i != 1:
        if i % 2 == 0:
            j = i // 2
        else:
            j = 3*i + 1
        edge = (i, j)
        if edge in edges:
            #print("exit:", edge)
            break
        #print("add:", edge)
        edges.add(edge)
        i = j

max_v = 0
for p in edges:
    max_v = max(max_v, p[0], p[1])

print("Largest number:", max_v)

adj = {}
distinctNodes = set()

for p in edges:
    parent = p[1]
    child = p[0]
    if parent not in adj:
        adj[parent] = []
    adj[parent].append(child)
    distinctNodes.add(parent)
    distinctNodes.add(child)

#print(adj)

print("Number of internal nodes:", len(adj))
print("Number of nodes:         ", len(distinctNodes))
print("Number of edges:         ", len(edges))
#print(adj)

# Visit the tree and produce BP representation
f = open("Collatz_"+str(n), "w")
BP(1, adj, f)
print()
