#obtain all files from dir, then output into a txt file
import os
names = os.listdir('/Users/winnie/Desktop/BACKUP/oldimg-label')
with open('/Users/winnie/Desktop/txtList.txt','wt') as f:
    print('\n'.join(str(x) for x in names),file=f)
