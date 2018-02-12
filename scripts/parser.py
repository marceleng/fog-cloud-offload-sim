import sys
from math import floor

maximum=.15
filename=sys.argv[1]
number_boxes=int(sys.argv[2])

step = maximum / number_boxes
keys = [ (step * (3. * x / 2. + 0.5 )) for x in range(number_boxes) ]

res = [0] * number_boxes

def box_number(i):
    return floor(i/step)

f=open(filename,'r')
for line in f:
    sl = line.split(',')
    response = float(sl[2]) - float(sl[0])
    res[box_number(response)] += 1

f.close()
print(res)
