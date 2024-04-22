#定义项目编译的根目录
export BUILD_ROOT = $(shell pwd)

#定义头文件的路径变量
export INCLUDE_PATH = $(BUILD_ROOT)/_include

#定义我们要编译的目录
BUILD_DIR = $(BUILD_ROOT)/dispatcher/ \
			$(BUILD_ROOT)/network/   \
			$(BUILD_ROOT)/threadpool/   \
			$(BUILD_ROOT)/app/ 

#编译时是否生成调试信息。GNU调试器可以利用该信息
#很多调试工具，包括Valgrind工具集都会因为这个为true能够输出更多的调试信息；
export DEBUG = true


all:
#-C是指定目录
#make -C signal   

#可执行文件应该放最后
#make -C app      

#用shell命令for搞，shell里边的变量用两个$
	@for dir in $(BUILD_DIR); \
		do \
			make -C $$dir; \
		done


clean:
#-rf：删除文件夹，强制删除
	rm -rf app/link_obj app/dep server
