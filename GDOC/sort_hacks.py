import os
import json


files = [x for x in os.listdir() if x.lower().endswith('.json')]


for i in files:
    r = json.loads(open(i).read())
    hacks = r['hacks']
    names = []
    for hack in hacks:
        names.append(hack['name'])
    names.sort()
    result = []
    for hack0 in names:
        for hack1 in hacks:
            if not hack1['name'] == hack0:
                continue
            result.append(hack1)
            break
    r['hacks'] = result
    open(i, 'w').write(json.dumps(r))
