# 简介

# 简介

欢迎使用腾讯云语音SDK，腾讯云语音SDK为开发者提供了访问腾讯云语音识别、语音合成等语音服务的配套开发工具，简化腾讯云语音服务的接入流程。

本项目是腾讯云语音SDK的C++语言版本。

# 依赖库安装

## Unix环境

### 本地调试依赖

本地编译调试依赖一下库：
1、openssl  ---1.1.1g+版本
2、cmake    ---3.8+版本
3、make工具
4、gcc      --- 8.0+版本

推荐安装库：
devtoolset-8-gcc
openssl-1.1.1g
cmake-3.9.2

本包已经集成了其余依赖库，可以直接执行 ``make``后进入bin目录执行。

注意：make编译前需要配置LD_LIBRARY_PATH环境变量，LD_LIBRARY_PATH={speech_engine_dir_path}

如果依赖库不能兼容您的机器，需要您自行下载编译。

1. [boost](https://www.boost.org/)下载最新release版本，解压，编译，并替换动态库到lib下。
2. 下载[openssl-1.1.1g](https://www.openssl.org/source/openssl-1.1.1h.tar.gz)，编译，并替换动态库到lib下。

## Windows环境

windows环境区分x64和x86的库安装。

1. 安装pthread的动态库。

   下载后解压，可以看到有个"Pre-built.2“文件夹。

   ![pthreads-w32-2-9-1-release](https://asr-develop-1256237915.cos.ap-nanjing.myqcloud.com/release.png)

   进入该文件夹，可以看到”dll“目录。

   ![Pre-built.2](https://asr-develop-1256237915.cos.ap-nanjing.myqcloud.com/prebuilt.2.png)

   进入”dll“目录，可以看到有“x86”和“x64”两个目录。

   ![dll目录](https://asr-develop-1256237915.cos.ap-nanjing.myqcloud.com/dll.png)

   * 如果使用x86编译器，将“x86”文件夹下的所有dll文件拷贝至“C:\Windows\SysWOW64”下。
   * 如果使用x64编译器，将“x64”文件夹下的所有dll文件拷贝至“C:\Windows\System32”下。
2. 安装openssl的动态库。

   * 如果使用x86编译器[Win32OpenSSL_Light-1_1_1h.exe](http://slproweb.com/download/Win32OpenSSL_Light-1_1_1h.exe)下载后，点击安装一直到完成。最后修改本SDK的lib目录下的“libcrypto32.lib”名称为“libcrypto.lib”。
   * 如果使用x64编译器[Win64OpenSSL_Light-1_1_1h.exe](http://slproweb.com/download/Win64OpenSSL_Light-1_1_1h.exe)下载后，点击安装一直到完成。最后修改本SDK的lib目录下的“libcrypto64.lib”名称为“libcrypto.lib”。

# 示例

编译得到可执行文件，配置run.sh参数，执行#bash run.sh即可本地启动TRTC实时语音识别；进入TRTC房间，识别结果通过回调接口返回

欢迎使用腾讯云语音SDK，腾讯云语音SDK为开发者提供了访问腾讯云语音识别、语音合成等语音服务的配套开发工具，简化腾讯云语音服务的接入流程。

本项目是腾讯云语音SDK的C++语言版本。

# 依赖库安装

## Unix环境

### 本地调试依赖

本地编译调试依赖一下库：
1、openssl  ---1.1.1g+版本
2、cmake    ---3.8+版本
3、make工具
4、gcc      --- 8.0+版本

推荐安装库：
devtoolset-8-gcc
openssl-1.1.1g
cmake-3.9.2

本包已经集成了其余依赖库，可以直接执行 ``make``后进入bin目录执行。

如果依赖库不能兼容您的机器，需要您自行下载编译。

1. [boost](https://www.boost.org/)下载最新release版本，解压，编译，并替换动态库到lib下。
2. 下载[openssl-1.1.1g](https://www.openssl.org/source/openssl-1.1.1h.tar.gz)，编译，并替换动态库到lib下。

## Windows环境

windows环境区分x64和x86的库安装。

1. 安装pthread的动态库。

   下载后解压，可以看到有个"Pre-built.2“文件夹。

   ![pthreads-w32-2-9-1-release](https://asr-develop-1256237915.cos.ap-nanjing.myqcloud.com/release.png)

   进入该文件夹，可以看到”dll“目录。

   ![Pre-built.2](https://asr-develop-1256237915.cos.ap-nanjing.myqcloud.com/prebuilt.2.png)

   进入”dll“目录，可以看到有“x86”和“x64”两个目录。

   ![dll目录](https://asr-develop-1256237915.cos.ap-nanjing.myqcloud.com/dll.png)

   * 如果使用x86编译器，将“x86”文件夹下的所有dll文件拷贝至“C:\Windows\SysWOW64”下。
   * 如果使用x64编译器，将“x64”文件夹下的所有dll文件拷贝至“C:\Windows\System32”下。
2. 安装openssl的动态库。

   * 如果使用x86编译器[Win32OpenSSL_Light-1_1_1h.exe](http://slproweb.com/download/Win32OpenSSL_Light-1_1_1h.exe)下载后，点击安装一直到完成。最后修改本SDK的lib目录下的“libcrypto32.lib”名称为“libcrypto.lib”。
   * 如果使用x64编译器[Win64OpenSSL_Light-1_1_1h.exe](http://slproweb.com/download/Win64OpenSSL_Light-1_1_1h.exe)下载后，点击安装一直到完成。最后修改本SDK的lib目录下的“libcrypto64.lib”名称为“libcrypto.lib”。

# 示例

编译得到可执行文件，配置run.sh参数，执行#bash run.sh即可本地启动TRTC实时语音识别；进入TRTC房间，识别结果通过回调接口返回。
