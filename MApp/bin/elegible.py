#!/usr/bin/python
import re
import sys

def main():
    
    typesizesfile = sys.argv[1]
    cachesize = int(sys.argv[2])
    infile = sys.argv[3]
    typesize = {}
    
    # Build type size hash from architecture information file
    for line in open(typesizesfile):
        linetype = line.split('\t')
        typesize[linetype[0]] = int(linetype[1])
    
    #print typesize
    
    searchtype = re.compile(r"'.*' .* '(.*)' .* '.*' '.*' .*")
    searchelements = re.compile(r"'.*' .* '.*' (.*) '.*' '.*' .*")
    for line in open(infile):
	line = line.strip()
        typematch = searchtype.match(line)
        sizematch = searchelements.match(line)
        
        try:
            currenttype = typematch.group(1)
            currentsize = sizematch.group(1).split()
        except AttributeError: 
            continue
        currenttotalsize = typesize[currenttype]
        for i in currentsize[1:]:
            currenttotalsize *= int(i)
	#print currenttotalsize
        if currenttotalsize >= cachesize:
            print line

if __name__ == "__main__":
    main()
