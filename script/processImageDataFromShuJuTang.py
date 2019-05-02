import os
import xml.etree.ElementTree as ET
from glob import glob
from shutil import copyfile


def splitBigXML2Many(xml_file, target_dir):
    '''Split by finding "folder".
    '''
    with open(xml_file, 'r') as f:
        lines = f.readlines()

    xml_count = 0
    lines_for_current_image = []
    for i in range(len(lines)):
        current_line = lines[i]

        if "folder" in current_line:
            if len(lines_for_current_image) == 0:
                lines_for_current_image = [current_line]
            else:
                current_xml_file = os.path.join(target_dir, str(xml_count) + ".xml")
                lines_for_current_image.insert(0, "<annotation>\n")
                lines_for_current_image.append("</annotation>")
                with open(current_xml_file, 'w') as f2:
                    f2.writelines(lines_for_current_image)
                xml_count += 1
                lines_for_current_image = [current_line]

        else:
            if len(lines_for_current_image) > 0:
                lines_for_current_image.append(current_line)

    current_xml_file = os.path.join(target_dir, str(xml_count) + ".xml")
    lines_for_current_image.insert(0, "<annotation>\n")
    with open(current_xml_file, 'w') as f2:
        f2.writelines(lines_for_current_image)


def getImagesAndAnnotations(input_xml_dir, parent_dir, target_dir):
    '''Generate Pascal VOC style annotation xml.
    '''
    image_dir = os.path.join(target_dir, "images")
    annotation_dir = os.path.join(target_dir, "annotations")
    if not os.path.exists(image_dir):
        os.makedirs(image_dir)
    if not os.path.exists(annotation_dir):
        os.makedirs(annotation_dir)
    for xml_file in glob(os.path.join(input_xml_dir, "*.xml")):
        print(xml_file)
        root = ET.parse(xml_file)
        folder = root.find('folder')
        folder_name = folder.text
        file_name = root.find('filename')
        image_name = file_name.text
        image_name_no_ext = image_name[:-4]
        new_xml_file = os.path.join(annotation_dir, image_name_no_ext + ".xml")
        new_image_file = os.path.join(image_dir, image_name)
        source_image_file = os.path.join(parent_dir, folder_name, image_name)
        folder.text = "images"
        root.write(new_xml_file)
        copyfile(source_image_file, new_image_file)


if __name__ == "__main__":
    # 1. first deal with typos - replace <floder> by <folder> and </floder> by </folder> for the xml in a text editor

    # 2. split one big xml into many, each for one image.
    #big_xml_file = '/home/shawn/Downloads/DataResult_20181114/ID001_jpg/ID001_jpg.xml'
    big_xml_file = '/Users/winnie/Desktop/DataResult/ID001_jpg/ID001_jpg.xml'
    #split_xml_dir = '/home/shawn/Downloads/DataResult_20181114/ID001_jpg/splitXML'
    split_xml_dir = '/Users/winnie/Desktop/DataResult/ID001_jpg/splitXML'
    splitBigXML2Many(big_xml_file, split_xml_dir)

    # 3. Correct the details for each xml and organize the images and the annotations
    parent_dir = '/Users/winnie/Desktop/DataResult'
    input_xml_dir = split_xml_dir
    target_dir = '/Users/winnie/Desktop/shuJuTang1'
    getImagesAndAnnotations(input_xml_dir, parent_dir, target_dir)
