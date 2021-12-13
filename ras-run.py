import RaS
import sys

filename = sys.argv[1]
with open(filename) as file:
    for line in file:
        if(line.isspace() == False):
            RaS.run(line.rstrip(), [], filename)
