import xml.etree.ElementTree as ET
import sys
from importlib import reload

#reload(sys)
#sys.setdefaultencoding('utf-8')


#path='/Users/winnie/Desktop/mov_008_130315.xml'
path=str(sys.argv[1])
tree = ET.parse(path)
rect={}
line=""
root = tree.getroot()
for name in root.iter('path'):
    rect['path'] = name.text
for ob in root.iter('object'):
    for bndbox in ob.iter('bndbox'):
        # for l in bndbox:
        #     print(l.text)
        for xmin in bndbox.iter('xmin'):
            rect['xmin'] = xmin.text
        for ymin in bndbox.iter('ymin'):
            rect['ymin'] = ymin.text
        for xmax in bndbox.iter('xmax'):
            rect['xmax'] = xmax.text
        for ymax in bndbox.iter('ymax'):
            rect['ymax'] = ymax.text
        width = float(xmax.text)-float(xmin.text)
        length = float(ymax.text)-float(ymin.text)
        area = width*length
        line = path + "\t"+ rect['xmin']+ "\t"+rect['ymin']+"\t"+rect['xmax']+"\t"+rect['ymax']+"\t"+str(area)+"\n"
        f1 = open('../select.txt', 'a+')
        f1.write(line)

