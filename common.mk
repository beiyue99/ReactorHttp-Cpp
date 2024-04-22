ifeq ($(DEBUG),true)
CC = g++ -g
VERSION = debug
else
CC = g++
VERSION = release
endif


# $(wildcard *.c)表示扫描当前目录下所有.c文件
SRCS = $(wildcard *.cpp)

#把字符串中的.c替换为.o
OBJS = $(SRCS:.cpp=.o)

#把字符串中的.c替换为.d
DEPS = $(SRCS:.cpp=.d)

#可以指定BIN文件的位置
BIN := $(addprefix $(BUILD_ROOT)/,$(BIN))

#定义存放ojb文件的目录
LINK_OBJ_DIR = $(BUILD_ROOT)/app/link_obj
DEP_DIR = $(BUILD_ROOT)/app/dep

#-p是递归创建目录，没有就创建，有就不需要创建了
$(shell mkdir -p $(LINK_OBJ_DIR))
$(shell mkdir -p $(DEP_DIR))

#我们要把目标文件生成到上述目标文件目录去
OBJS := $(addprefix $(LINK_OBJ_DIR)/,$(OBJS))
DEPS := $(addprefix $(DEP_DIR)/,$(DEPS))

#找到目录中的所有.o文件（编译出来的）
LINK_OBJ = $(wildcard $(LINK_OBJ_DIR)/*.o)
#因为构建依赖关系时app目录下这个.o文件还没构建出来，所以LINK_OBJ是缺少这个.o的，我们 要把这个.o文件加进来
LINK_OBJ += $(OBJS)

#如下这行会是开始执行的入口
all:$(DEPS) $(OBJS) $(BIN)

#有必要先判断这些文件是否存在，不然make可能会报一些.d文件找不到

ifneq ("$(wildcard $(DEPS))","") 
include $(DEPS)  
endif




$(BIN):$(LINK_OBJ)
#	@echo "------------------------build $(VERSION) mode--------------------------------!!!"

	$(CC) -o $@ $^



#将每个 .cpp 文件编译成一个同名的 .o 文件，并将其放置在 $(LINK_OBJ_DIR) 目录下 
#-c: 这个选项告诉编译器只生成目标文件，而不进行链接。
$(LINK_OBJ_DIR)/%.o:%.cpp
	$(CC) -I$(INCLUDE_PATH) -o $@ -c $(filter %.cpp,$^)



#我们现在希望当修改一个.h时，也能够让make自动重新编译我们的项目，所以，我们需要指明让.o依赖于.h文件
#我们可以用“gcc -MM c程序文件名” 来获得这些依赖信息并重定向保存到.d文件中
#.d文件中的内容可能形如：nginx.o: nginx.c ngx_func.h
$(DEP_DIR)/%.d:%.cpp
#echo 中 -n表示后续追加不换行
	echo -n $(LINK_OBJ_DIR)/ > $@
#  >>表示追加
	gcc -I$(INCLUDE_PATH) -MM $^ >> $@
