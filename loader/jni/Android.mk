LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS +=-fPIE -DDEBUG -D__ARM__ -Wunused
LOCAL_LDFLAGS += -ldl -llog -fPIE -pie
LOCAL_MODULE:=loader
LOCAL_SRC_FILES := main.cpp
include $(BUILD_EXECUTABLE)
