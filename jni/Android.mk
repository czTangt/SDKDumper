LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPPFLAGS += -pie -fPIE -ffunction-sections -fdata-sections -fvisibility=hidden
LOCAL_LDFLAGS += -pie -fPIE -Wl,--gc-sections
LOCAL_CFLAGS := -Wno-error=format-security -fpermissive
LOCAL_CFLAGS += -ffunction-sections -fdata-sections -fvisibility=hidden
LOCAL_CFLAGS += -fno-rtti -fno-exceptions
LOCAL_CFLAGS += -DNDEBUG

LOCAL_MODULE := sdkdumper64

LOCAL_SRC_FILES := source/main.cpp \
                   source/core/FNames.cpp \
				   source/core/Offsets.cpp \
                   source/core/GUObjects.cpp \
                   source/core/SDK.cpp \
                   source/elf/Fix.cpp \
                   source/utils/Tools.cpp \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/include/core \
                    $(LOCAL_PATH)/include/elf \
                    $(LOCAL_PATH)/include/utils

LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -lz -llog

include $(BUILD_EXECUTABLE)




