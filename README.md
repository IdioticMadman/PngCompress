### PNG压缩
    
本仓库使用了https://github.com/ImageOptim/libimagequant进行 png 的量化。以及使用了 libpng 作为最后 png 文件的输出

由于 libimagequant 是用 rust 写的。所以这里使用了 https://github.com/bbqsrc/cargo-ndk 进行构建 android 平台的的静态库文件
用法参考 cargo-ndk 中 example 使用构建


https://github.com/pnggroup/libpng
libpng 则是使用 cmake 进行构建的

```shell
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$NDK_ROOT/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 -DPNG_TESTS=OFF -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_DEBUG=OFF -DPNG_ARM_NEON=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install_android/arm64_v8a
cmake --build build -j 32
cmake --install build

cmake -B build -DCMAKE_TOOLCHAIN_FILE=$NDKROOT/build/cmake/android.toolchain.cmake -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 -DPNG_TESTS=OFF -DPNG_SHARED=OFF -DPNG_STATIC=ON -DPNG_DEBUG=OFF -DPNG_ARM_NEON=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install_android/armeabi_v7a
cmake --build build -j 32
cmake --install build

```