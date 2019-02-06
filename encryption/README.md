1.模型打包函数，infile1为head.param，infile2为head.bin，生成.model文件
int merge(char *infile1,char *infile2)
2.模型拆分函数，输入.model文件，输出head.param和head.bin
FILE * separate(char *infile)
3.编码函数
int encryption(char *infile)
4.解码函数
int decryption(char *infile)
