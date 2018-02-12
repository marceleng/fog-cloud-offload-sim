import sys
import numpy as np

sampling = np.logspace(0, 7, dtype=np.int, num=20)-1

cloud = 0
fog_hit = 1
fog_miss = 2

res = {}

for i in sampling:
    res[i] = {cloud:0, fog_hit:0, fog_miss:0}

for filename in sys.argv[1:]:
    print("Searching "+filename)
    f = open(filename,'r')
    for line in f:
        sl = line.split(',')
        contentid = int(sl[1])
        if contentid in sampling:
            path = sl[3]
            if "cloud" in path:
                res[contentid][cloud] += 1
            elif "fog_proc" in path:
                res[contentid][fog_miss] += 1
            else:
                res[contentid][fog_hit] +=1

    f.close()
print(res)
