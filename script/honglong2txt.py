import os

#filenames = [f for f in os.listdir() if f.endswith(".jpg")]
#for f in filenames:
with open("/Users/winnie/Desktop/Gesture/robo.txt", "r") as infile:
    for line in infile:
        new = line.split(',')
        name = new[0].split('/')
        name_noext = name[-1][0:-4]
        DX = int(new[1])
        DH = int(new[2])
        #print(DX)
        #print(DH)
        if DX != 0 and DH != 0:
            x1 = int(new[4])
            y1 = int(new[5])
            x2 = int(new[6])
            y2 = int(new[7])
            x = (x1+x2)/(2*DX)
            y = (y1+y2)/(2*DH)
            w = (x2-x1)/DX
            h = (y2-y1)/DH
            #print(x)
            #print(y)
            #print(w)
            #print(h)
            with open("/Users/winnie/Desktop/Gesture/annotations/"+name_noext+".txt", "a+") as outfile:
                if new[3] == 'head':
                    outfile.write("0"+" "+str(x)+" "+str(y)+" "+str(w)+" "+str(h)+"\n")
                if new[3] == 'hand':
                    outfile.write("1"+" "+str(x)+" "+str(y)+" "+str(w)+" "+str(h)+"\n")
        else:
            print("error"+name_noext)                    
