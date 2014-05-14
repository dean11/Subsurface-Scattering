import os

filename = "sbun.obj.old"

f = open(filename, 'r')
fOut = open("sbun.obj", 'w')


while True:
    text = f.read(1);
    if len(text) == 0:
        break
    
    if text != 'f':
        fOut.write(text + f.readline())
        
    else:
        face = f.readline()
        
        face = face.split()
        
        v0 = face[2]
        v1 = face[1]
        v2 = face[0]
        face[0] = v0
        face[1] = v1
        face[2] = v2
        fOut.write(text +' '+ face[0]+' '+face[1]+' '+face[2]+"\n")
        
        
f.close()
fOut.close()
