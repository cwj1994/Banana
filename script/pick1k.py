import os, random, shutil
def copyFile(fileDir):
        pathDir = os.listdir(fileDir)    #取图片的原始路径
        filenumber=len(pathDir)
        rate=0.2    #自定义抽取图片的比例，比方说100张抽10张，那就是0.1
        picknumber=int(filenumber*rate) #按照rate比例从文件夹中取一定数量图片
        sample = random.sample(pathDir, picknumber)  #随机选取picknumber数量的样本图片
        print (sample)
        for name in sample:
                shutil.copy(fileDir+name, tarDir+name)
        return

if __name__ == '__main__':
	fileDir = "/home/etcvision/Desktop/Git/BodyDetectionVideos/ETCTest10K/images/"#"/Users/winnie/Desktop/ShuJuTang2-7F/ShuJuTang7F/images/"#"/home/cwj1994/Desktop/ShuJuTang6F-hs/images/"#"/home/cwj1994/Desktop/Head-Data/Midea+Robo-hs/head-shoulder/"#"/home/cwj1994/Desktop/Head-Data/ShuJuTang6F/images/"    #源图片文件夹路径
	tarDir = "/home/etcvision/Desktop/Git/BodyDetectionVideos/ETCTest10K/images-picked/"#"/home/cwj1994/Desktop/Head-Data-APR10/images/"#"/home/cwj1994/Desktop/warmup_head/images/"    #移动到新的文件夹路径
	copyFile(fileDir)
