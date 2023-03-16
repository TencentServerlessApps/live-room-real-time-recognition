#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <streambuf>
#include <thread>
#include <vector>
#include "TRTCCloud.h"
#include "usages/TencentSDKWapperRecord.h"  // 单流录制

#define OUTPUT_LOG  // 控制主函数的输出。

constexpr int DEMO_SENCE_RECORD = 2;
constexpr int STR_ROOM_FLAG = 1;

/* 程序运行参数 管理结构体 */
struct InputArgs {
    bool is_running;  // 处理 TERM 信号 收到 TERM 信号会置为 false 程序退出
    int strRoom;  // 0：roomId用整数，1：RoomId用字符串
    std::string appid;   // 【必须】
    std::string secret_id;  // 【必须】
    std::string secret_key;   // 【必须】
    std::string callback_url;   // 【必须】
    uint32_t sdk_app_id;   // 【必须】
    std::string user_id;   // 【必须】
    std::string user_sig;  // 【必须】
    std::string room_id;   // 【必须】
    std::string private_key;   // 【非必须】
    uint32_t scenes;  // 位控制：推流 1 单流录制 2 混流录制 4，如 3表示，推流 +
    // 单流录制 两个场景会先后执行。
    uint32_t record_files;  // 位控制：纯音频 1 纯视频 2 音视频 4，如 3表示，
    // 生成2个文件 一个是纯音频一个是纯视频。
    uint32_t timeout;              // 房间无人超时退出时间单位ms

    InputArgs()
            : is_running(false),
              strRoom(0),
              sdk_app_id(0),
              scenes(0),
              record_files(OUTFILE_TYPE_AUDIO),
//        record_directory("./record"),
              timeout(0) {}

    void printArgs();

    bool checkArgs() {
      if (appid.empty()) {
        return false;
      }

      if (secret_id.empty()) {
        return false;
      }

      if (secret_key.empty()) {
        return false;
      }

      if (sdk_app_id == 0) {
        return false;
      }

      if (user_id.empty()) {
        return false;
      }

      if (user_sig.empty()) {
        return false;
      }

      if (room_id.empty()) {
        return false;
      }

      if (scenes < 1 || scenes > 16) {
        return false;
      }
      return true;
    }
};

InputArgs g_input_arg_info;

// clang-format off
void InputArgs::printArgs() {
#ifdef OUTPUT_LOG
  printf("ARGS:   --sdkAppId = %d\n", sdk_app_id);
  printf("        --userId = %s\n", user_id.c_str());
  printf("        --userSig = %s\n", user_sig.c_str());
  printf("        --privateKey = %s\n", private_key.c_str());
  printf("        --demoSences = %d\n", scenes);
  printf("        --roomId = %s\n", room_id.c_str());
  printf("        --recFiles = %d\n", record_files);
  printf("        --strRoom = %d\n", strRoom);
#endif
}

/* 参数解析 */
char *l_opt_arg = nullptr;
// a: 带一个":"表示带参数值  S:: 带两个":"表示是可选参数。 不带":"表示有参无值。
const char *short_options = "a:u:s:r:t:F::M::A::K::R::T::S::";
struct option long_options[] = {
        {"sdkAppId",    required_argument, NULL, 'a'},
        {"userId",      required_argument, NULL, 'u'},
        {"userSig",     required_argument, NULL, 's'},
        {"privateKey",  optional_argument, NULL, 'k'},
        {"roomId",      required_argument, NULL, 'r'},
        {"demoSences",  required_argument, NULL, 't'},
        {"recFiles",    optional_argument, NULL, 'F'},
        {"appId",       required_argument, NULL, 'M'},
        {"secretId",    required_argument, NULL, 'A'},
        {"secretKey",   required_argument, NULL, 'K'},
        {"callbackUrl", required_argument, NULL, 'R'},
        {"timeout",     optional_argument, NULL, 'T'},
        {"strRoom",     optional_argument, NULL, 'S'},
        {0,             0,                 0,    0}, // 【必须】全0 item 结尾。
};

void print_usage() {
#ifdef OUTPUT_LOG
  printf("DESCRIPTION\n");
  printf("         show tencent trtc sdk functions, such as push mp4, record stream and stream mix-record.\n\n");
  printf("  --sdkAppId, -a\n");
  printf("         a number like 1400XXXXXX.\n\n");
  printf("  --userId, -u\n");
  printf("         a str like teacher_01 stand for a teacher.\n\n");
  printf("  --userSig, -s\n");
  printf("         a str like _KDU#DDDSKJFSK_*DSKSDFKDSF... stand for a token, witch relates with sdkAppId,userId. \n\n");
  printf("  --privateKey, -k\n");
  printf("         a str specifies more stringent permissions for a UserID, including whether the UserID can be used to enter a specific room and perform audio/video upstreaming in the room. \n\n");
  printf("  --roomId, -r\n");
  printf("         a number like 66666 stand for a room.\n\n");
  printf("  --demoSences, -t\n");
  printf("         a number in [1,7] stand for witch demos you want to run, 1 for push, 2 for record stream, 4 for stream mix-record.\n\n");
  printf("  --recFiles, -F\n");
  printf("         if demoSences [2,7] must be setted. a number in [1,7] stand for files want to record, 1 for audio only, 2 for video only, 4 for audio and video\n\n");
  printf("  --appId, -M\n");
  printf("         a str like 123345 stand for a user.\n\n");
  printf("  --secretId, -A\n");
  printf("         a str like sadasdaas-sdad-sdaads stand for secret id .\n\n");
  printf("  --secretKey, -K\n");
  printf("         a str like adadaasdas-adadad-asdasd stand for secret key.\n\n");
  printf("  --callbackUrl, -R\n");
  printf("         a str like https://xxxx/callback stand fo callback url.\n\n");
  printf("  --timeout, -T\n");
  printf("         unit is millisecond \n\n");
#endif
}

// clang-format on
void print_arg(int argc, char *argv[]) {
#ifdef OUTPUT_LOG
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }
#endif
}

int parse_arg(int argc, char *argv[]) {
  print_arg(argc, argv);
  int c;
  std::string opt;
  while ((c = getopt_long(argc, argv, short_options, long_options, NULL)) !=
         -1) {
    switch (c) {
      case 'a':
        opt = optarg;
        g_input_arg_info.sdk_app_id = std::stoi(opt);
        break;
      case 'u':
        opt = optarg;
        g_input_arg_info.user_id = opt;
        break;
      case 's':
        opt = optarg;
        g_input_arg_info.user_sig = opt;
        break;
      case 'k':
        opt = optarg;
        g_input_arg_info.private_key = opt;
        break;
      case 'r':
        opt = optarg;
        g_input_arg_info.room_id = opt;
        break;
      case 't':
        opt = optarg;
        g_input_arg_info.scenes = std::stoi(opt);
        break;
      case 'F':
        opt = optarg;
        g_input_arg_info.record_files = std::stoi(opt);
        break;
      case 'M':
        opt = optarg;
        g_input_arg_info.appid = opt;
        break;
      case 'A':
        opt = optarg;
        g_input_arg_info.secret_id = opt;
        break;
      case 'K':
        opt = optarg;
        g_input_arg_info.secret_key = opt;
        break;
      case 'R':
        opt = optarg;
        g_input_arg_info.callback_url = opt;
        break;
      case 'T':
        opt = optarg;
        g_input_arg_info.timeout = std::stoi(opt);
        break;
      case 'S':
        opt = optarg;
        g_input_arg_info.strRoom = std::stoi(opt);
        break;
      default:
        return -1;
    }
  }

  if (!g_input_arg_info.checkArgs()) {
    return -2;
  }

  return 0;
}

// 处理USR1 自定义信号。
void onSIGUSR1_Stop(int sig) {
  printf("receive sig SIGUSR1(%d) stop record!\n", sig);
  g_input_arg_info.is_running = false;
}

// 处理终止程序的信号，可以响应 kill -TERM PID 命令，顺利退出程序。
void onSIGTERM_Stop(int sig) {
  printf("receive sig SIGTERM(%d) stop record!\n", sig);
  g_input_arg_info.is_running = false;
}

// 实时音频识别
void Recognize() {
  printf(" stream record begin \n");
  std::string roomName = g_input_arg_info.room_id;

  // step 1 进房参数
  TRTCParams params;
  params.sdkAppId = g_input_arg_info.sdk_app_id;  // 进房参数设置 sdkappid
  std::string userid = g_input_arg_info.user_id;  // 进房参数设置 userid
  std::string usersig = g_input_arg_info.user_sig;  // 进房参数设置 usersig
  params.userId = userid;
  params.userSig = usersig;

  std::string privateKey = g_input_arg_info.private_key;  // 进房参数设置 privateKey
  if (!privateKey.empty()) {
    params.privateMapKey = privateKey;
  }

  std::string businessInfo;
  // 区分房间ID是字符串房间ID还是整数房间ID
  if (STR_ROOM_FLAG == g_input_arg_info.strRoom) {
    printf("room type: strRoomId \n");
    params.roomId = -1;
    businessInfo = "{\"strGroupId\": \"" + roomName + "\"}";
    params.businessInfo = businessInfo;
  } else {
    params.roomId = std::stol(roomName);
    params.businessInfo = roomName;
  }
  params.clientRole =
          TRTCClientRole::TRTCClientRole_Audience;  // 【必须】 用观众角色。

  // step 2 开始进房
  ASRParam asrParam;
  asrParam.appid = g_input_arg_info.appid;
  asrParam.secret_id = g_input_arg_info.secret_id;
  asrParam.secret_key = g_input_arg_info.secret_key;
  asrParam.sdk_app_id = g_input_arg_info.sdk_app_id;

  if (STR_ROOM_FLAG == g_input_arg_info.strRoom) {
    asrParam.room_id = -1;
    asrParam.str_room_id = roomName;
  } else {
    asrParam.room_id = std::stol(roomName);
    asrParam.str_room_id = "";
  }
  asrParam.callback_url = g_input_arg_info.callback_url;

  std::shared_ptr <TencentSDKWarperRecognizer> inst =
          std::make_shared<TencentSDKWarperRecognizer>(params.sdkAppId, &asrParam);
  inst->StartRecord(params, roomName.c_str(), g_input_arg_info.record_files,
                    g_input_arg_info.timeout);

  g_input_arg_info.is_running = true;
  int reason = 0;
  while (g_input_arg_info.is_running && inst->IsRunning(reason)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  // step 3 退房
  inst->StopRecord();

  printf(" stream speech recognition end with reason %d\n", reason);
}

void MainProcess() {
  if (g_input_arg_info.scenes & DEMO_SENCE_RECORD) {
    Recognize();
  }
}

int main(int argc, char *argv[]) {
  signal(SIGUSR1, onSIGUSR1_Stop);
  signal(SIGTERM, onSIGTERM_Stop);

  if (0 != parse_arg(argc, argv)) {
    print_usage();
    printf("parse input args failed!\n");
    return 1;
  }

  g_input_arg_info.printArgs();

  /**
   * 日志默认存放在 /tmp 目录下对应的roomid和userid 的目录下。
   */
  char logpath[1024] = {0};
  snprintf(logpath, 1024, "/tmp/%s_%s", g_input_arg_info.room_id.c_str(),
           g_input_arg_info.user_id.c_str());
  setLogDirPath(logpath);
  setLogLevel(TRTCLogLevelError);

//  setConfigPath("trtc_config.json");

  // 设置运行环境，线上环境
  setEnvironment(0);

  /**
   * 如果不想控制台输出SDK日志，请使用 setConsoleEnabled(false)
   */
  setConsoleEnabled(true);

  /**
   * 逻辑总入口
   */
  printf("start main process...\n");
  MainProcess();
  printf("end main process...\n");

  /**
   * 延迟5s退出让事件上报飞一会儿。
   */
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));
  return 0;
}
