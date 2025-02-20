l = [1, 2, 3, 4, 5]
assert(l[0] == 1)
assert(l[1] == 2)
assert(l[2] == 3)
assert(l[3] == 4)
assert(l[4] == 5)

print(l)

it = l.iter()
assert(it.next() == 1)
assert(it.next() == 2)
assert(it.next() == 3)
assert(it.next() == 4)
assert(it.next() == 5)

l.insert(0, 10)
assert(l[0] == 10)
assert(l.size() == 6)
l.remove(0)
assert(l.size() == 5)
assert(l[0] == 1)
l.setitem(0, 42)
assert(l[0] == 42)
assert(l.item(2) == 3)
l.resize(20)
assert(l.size() == 20)
print(l.tostring())
