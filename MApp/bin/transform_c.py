#!/usr/bin/python
import re
import sys
import os

#
def extract(text, sub1, sub2):
	return text.split(sub1)[-1].split(sub2)[0]

def main():

    infile = sys.argv[1]	
    filedir = sys.argv[2]
    mainfile = sys.argv[3]
    finalfile = sys.argv[4]
    numafactor = int(sys.argv[5])
    numberlinks = int(sys.argv[6])   

#including MAi-array library
    mainpath = filedir  + mainfile
    tmpmainfile = filedir+'tmp'+mainfile
    tmpmainfiled = open(tmpmainfile,'w')
    mainfiled = open(mainpath)
    tmpmainfiled.write('#include "mai.h"\n')
    while True:
	instruction = mainfiled.readline()
	if not instruction: 
		break
	tmpmainfiled.write(instruction)	
    if instruction.find('main(') != -1:
	while instruction.find('{') != -1:
		tmpmainfiled.write(instruction+'\n')
		instruction = mainfiled.readline()
	tmpmainfiled.write(instruction+'\n')
	tmpmainfiled.write('mai_init(NULL);\n')	
    tmpmainfiled.flush()
    tmpmainfiled.close()	
    mainfiled.close()
    os.rename(tmpmainfile, mainpath)

    searcharray = re.compile(r"'.*' (ARRAY_DIM) '.*' .* '.*' '.*' .*")
    searchfile = re.compile(r"'.*' .* '.*' .* '.*' '(.*)' .*")
    searchvar = re.compile(r"'(.*)' .* '.*' .* '.*' '.*' .*")
    searchtype = re.compile(r"'.*' .* '(.*)' .* '.*' '.*' .*")
    searchdim = re.compile(r"'.*' .* '.*' (.*) '.*' '.*' .*")

#find and change variables declaration
    for line in open(infile): #get variable, type and its file
	line = line.strip()
        arraymatch = searcharray.match(line)
	if arraymatch != None: #do only if it is array
		typematch = searchtype.match(line)
        	varmatch = searchvar.match(line)
		filematch = searchfile.match(line)
		dimmatch = searchdim.match(line)
		try:
            		currenttype = typematch.group(1)
            		currentvar = varmatch.group(1)
			currentfile = filematch.group(1)
			dim = dimmatch.group(1).split()
			currentdim = int(dim[0])
        	except AttributeError: 
            		continue
		tmpfile = filedir+'tmp'+currentfile
		currentfiled = open(filedir+currentfile)
		tmpfiled = open(tmpfile,'w')
		for instruction in currentfiled: #get the declaration line
			if instruction.find(currentvar) != -1:
				if instruction.find(currenttype) != -1:
					str1 = extract(instruction,currentvar,',')
					if str1[-1] == ']':
						srcvar = currentvar+str1
					str1 = extract(instruction,currentvar,';')
					if str1[-1] == ']':
						srcvar = currentvar+str1
					str1 = extract(instruction,currentvar,' ')
					if str1[-1] == ']':
						srcvar = currentvar+str1
					destvar = currentdim*'*'+currentvar
					instruction = instruction.replace(srcvar,destvar)
			tmpfiled.write(instruction)
		tmpfiled.flush()
		currentfiled.close()
		tmpfiled.close()
		os.rename(tmpfile,filedir+currentfile)

#insert MAi functions on main file source

    if int(numafactor) >=2 and int(numberlinks) == 1:
		policy = 'bind'
    else:
		policy = 'cyclic'

    for line in open(infile): #get variable, type and its file
		line = line.strip()
		typematch = searchtype.match(line)
        	varmatch = searchvar.match(line)
		filematch = searchfile.match(line)
		dimmatch = searchdim.match(line)
		try:
            		currenttype = typematch.group(1)
            		currentvar = varmatch.group(1)
			currentfile = filematch.group(1)
			dim = dimmatch.group(1).split()
			currentdim = dim[0]
        	except AttributeError: 
            		continue
		tmpfile = filedir+'tmp'+finalfile
		currentfiled = open(filedir+mainfile) # main file
		tmpfiled = open(tmpfile,'w')
		while True:
#for instruction in currentfiled: 
			instruction = currentfiled.readline()
			
			if not instruction: 
				break

			instruction = instruction.strip()
			#print 'read: '+ instruction
			if mainfile == currentfile: #get the declaration line and place MAi functions after that
				if instruction.find(currentvar) != -1 and instruction.find(currenttype) != -1:
					while instruction.find(';') != -1:
						#print 'var: '+instruction
						tmpfiled.write(instruction+'\n')
						instruction = currentfiled.readline()
					tmpfiled.write(instruction+'\n')
					newline = currentvar+'= mai_alloc_'+currentdim+'D('
					i = 1
  					while i < (int(currentdim)+1):
						newline +=dim[i] 
						newline +=','	
						i = i + 1
					newline = newline+'sizeof('+currenttype+'),'+currenttype.upper()+');'
					#print newline
					tmpfiled.write(newline+'\n')
					if policy == 'bind':
						if int(currentdim) >= 2:
							newline = 'mai_'+policy+'_rows('+currentvar+');'
						else: 
							newline = 'mai_'+policy+'_columns('+currentvar+');'
					else:
						newline = 'mai_'+policy+'('+currentvar+');'
					tmpfiled.write(newline+'\n')
				else:
					tmpfiled.write(instruction+'\n')
			else: #place MAi functions after that
				if instruction.find('main(') != -1:
					while instruction.find('{') != -1:
						tmpfiled.write(instruction+'\n')
						instruction = currentfiled.readline()
					tmpfiled.write(instruction+'\n')
					newline = currentvar+'= mai_alloc_'+currentdim+'D('
					i = 1
  					while i < (int(currentdim)+1):
						newline +=dim[i] 
						newline +=','	
						i = i + 1
					newline = newline+'sizeof('+currenttype+'),'+currenttype.upper()+');'
					#print newline
					tmpfiled.write(newline+'\n')
					if policy == 'bind':
						if int(currentdim) >= 2:
							newline = 'mai_'+policy+'_rows('+currentvar+');'
						else: 
							newline = 'mai_'+policy+'_columns('+currentvar+');'
					else:
						newline = 'mai_'+policy+'('+currentvar+');'
					tmpfiled.write(newline+'\n')
				else:
					tmpfiled.write(instruction+'\n')


		#tmpfiled.flush()
		currentfiled.close()
		tmpfiled.close()
		os.rename(tmpfile, filedir+mainfile)
   #rename file    
    os.rename(filedir+mainfile,filedir+finalfile)


if __name__ == "__main__":
    main()
