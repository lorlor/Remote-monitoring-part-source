���������v1_0_cap.c(���v1),v1_5_cap.c(���v1.5),v2_cap.c(���v2)���ֱ��������汾����Ƶ�ɼ�������v1�У�ֻ�ܲɼ�yuyv422��ʽ��ԭʼ��Ƶ���ݡ���v1.5���Բɼ���yuv420����Ƶ�����ǻỨ����v2���Գɹ��ɼ�yuv420��Ƶ�����Ҳ����ڻ��������⣬����������������������ڴ�Ųɼ���Ƶ���ļ�ָ���ڲ�ͬ�ĵط����壺��v1.5�ж�����process_image�Ӻ����У���v2�ж���Ϊ��һ��ȫ�ֵı�����Ϊ�˲�ͬ�Ķ��巽ʽ�������ȫ��ͬ�Ĳɼ�����������Ҫ����̽����

How to compile encode.c:
gcc -o encode encode.c -lx264 -lpthread -lm

How to compile files using jrtplib and jthread library:
g++ -o outname jrtpfile.cpp -ljrtp -ldl -lpthread -ljthread -I[path to the directory including headers]

How to get a video with mp4 suffix from a .yuv file using ffmpeg console order:
ffmpeg -s 640x480 -i filename.yuv -vcodec mpeg4 filename.mp4
