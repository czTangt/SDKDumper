#!/bin/bash
set -e

adb(){
    /mnt/c/Users/czTang/AppData/Local/Android/Sdk/platform-tools/adb.exe "$@"
}

# 设置架构
ARCH=${1:-64}  # 默认64位，如果有参数则使用参数

if [ "$ARCH" = "32" ]; then
    APP_ABI="armeabi-v7a"
    GENERATE_FILE="libs/armeabi-v7a/sdkdumper32"
    SDKDUMP_FILE="/data/local/tmp/sdkdumper32"
else
    APP_ABI="arm64-v8a"
    GENERATE_FILE="libs/arm64-v8a/sdkdumper64"
    SDKDUMP_FILE="/data/local/tmp/sdkdumper64"
fi

LOCAL_SDK_DIR="./dump/sdk"
REMOTE_SDK_DIR="/sdcard/Download/match2024_final"

echo "============= Building for $ARCH-bit ============="
ndk-build clean
ndk-build APP_ABI=$APP_ABI

# echo "============= Pushing & Executing ============="
# adb push $GENERATE_FILE $SDKDUMP_FILE
# adb shell chmod 755 $SDKDUMP_FILE
# adb shell su -c "$SDKDUMP_FILE"

# echo "============= Pulling SDK Files ============="
# mkdir -p $LOCAL_SDK_DIR
# adb pull $REMOTE_SDK_DIR/Strings.txt $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Strings.txt"
# adb pull $REMOTE_SDK_DIR/Actors.txt $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Actors.txt"
# adb pull $REMOTE_SDK_DIR/Objects.txt $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Objects.txt"
# adb pull $REMOTE_SDK_DIR/Class.hpp $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Class.hpp"
# adb pull $REMOTE_SDK_DIR/Function.hpp $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Function.hpp"
# adb pull $REMOTE_SDK_DIR/Enum.hpp $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Enum.hpp"
# adb pull $REMOTE_SDK_DIR/Struct.hpp $LOCAL_SDK_DIR/ 2>/dev/null || echo "Failed to pull Struct.hpp"

# echo "Files pulled to: $LOCAL_SDK_DIR"

# echo "============= Complete ============="