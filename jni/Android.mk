LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPPFLAGS += -pie -fPIE -ffunction-sections -fdata-sections -fvisibility=hidden
LOCAL_LDFLAGS += -pie -fPIE -Wl,--gc-sections
LOCAL_CFLAGS := -Wno-error=format-security -fpermissive
LOCAL_CFLAGS += -ffunction-sections -fdata-sections -fvisibility=hidden
LOCAL_CFLAGS += -fno-rtti -fno-exceptions
LOCAL_CFLAGS += -DNDEBUG

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_MODULE := sdkdumper32
else
    LOCAL_MODULE := sdkdumper64
endif

LOCAL_SRC_FILES := source/main.cpp \
                   source/core/Dumper.cpp \
                   source/utils/Tools.cpp \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/include/core \
                    $(LOCAL_PATH)/include/utils \
					$(LOCAL_PATH)/include/game

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -lz -llog

include $(BUILD_EXECUTABLE)




