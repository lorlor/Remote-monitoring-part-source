关于这里的v1_0_cap.c(简称v1),v1_5_cap.c(简称v1.5),v2_cap.c(简称v2)，分别是三个版本的视频采集程序，在v1中，只能采集yuyv422格式的原始视频数据。而v1.5可以采集到yuv420的视频，但是会花屏，v2可以成功采集yuv420视频，并且不存在花屏的问题，这两个程序的区别在于用于存放采集视频的文件指针在不同的地方定义：在v1.5中定义在process_image子函数中，在v2中定义为了一个全局的变量，为了不同的定义方式会产生完全不同的采集质量，这需要慢慢探索。

How to compile encode.c:
gcc -o encode encode.c -lx264 -lpthread -lm

How to compile files using jrtplib and jthread library:
g++ -o outname jrtpfile.cpp -ljrtp -ldl -lpthread -ljthread -I[path to the directory including headers]

How to get a video with mp4 suffix from a .yuv file using ffmpeg console order:
ffmpeg -s 640x480 -i filename.yuv -vcodec mpeg4 filename.mp4
