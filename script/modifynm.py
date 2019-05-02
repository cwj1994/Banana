from xml.etree.ElementTree import parse, Element
import sys
path = str(sys.argv[1])
doc = parse(path)
root = doc.getroot()
# Remove a few elements
for ob in root.iter('object'):
    name = ob.find('name')
    if name.text == 'person':
        name.text = 'head-shoulder'
    # Insert a new element after <nm>...</nm>
#    n = ob.getchildren().index(ob.find('pose'))
#    print(n)
#    e = Element('name')
#    #f = "\n"
#    e.text = 'head'
#    #ob.insert(0, f)
#    ob.insert(n-1, e)
#
#    # Write back to a file
doc.write(path, xml_declaration=True)
