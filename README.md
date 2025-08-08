# SDKDumper
ue4 sdk dump for android.

# How to Use
- configure the ndk environment.
- Execute the command of `ndk-build` in the root directory.
- Then you will get the executable file `sdkdumper64` in the libs directory.
- You can modify the files in the `game` directory to adapt to different games. Note that you need to reference the header file in `dumper.h`.