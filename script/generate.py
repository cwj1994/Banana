import glob, os
file_list = glob.glob('./images/*.jpg')
print(len(file_list))
with open("imagelist.txt", 'a') as f:
    for path in file_list:
        #print(path)
        filename = os.path.basename(path)
        f.write(filename+"\n")

