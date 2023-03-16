#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <sys/syscall.h>
#include <cmath>
#include "TencentSDKWapperRecord.h"
#include "speech_recognizer.h"
#include "tcloud_util.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <stddef.h>
#include "httplib.h"
#include "unistd.h"
#include "time.h"
#include <chrono>

//#define CLOCKS_PER_SEC ((clock_t)1000)

namespace
{
  // the switch of custom decrypt, keep false.
  constexpr bool ENABLE_CUSTOM_DECRYPT = false;

  uint64_t GetTimestamps()
  {
    return (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count() /
           1000000;
  }

} // namespace

#pragma mark - AsrStreamRecognizer

// voice_id与recognizers实例映射
std::map<std::string, AsrStreamRecognizer *> voiceUsers_;

// 环境变量
const char *envRealtimeEnable = "ENV_REALTIME_ENABLE"; // 是否返回实时识别中间结果
const char *envEngineType = "ENV_ENGINETYTE";

/**
 * 用于统计链路耗时
 */
int sentenceNum = 1;
int asrCallbackNum = 1;
int sentenceCallbackNum = 1;
std::queue<int64_t> *sentenceStartTime = new std::queue<int64_t>();

int64_t GetNowTimestampMS()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void PushStartTime(int64_t startTime)
{
  sentenceStartTime->push(startTime);
}

int64_t PollStartTime()
{
  int64_t startTime;
  if (!sentenceStartTime->empty())
  {
    startTime = sentenceStartTime->front();
    sentenceStartTime->pop();
  }
  return startTime;
}

std::string MarshalJson(RecResult recResult)
{
  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer(s);

  writer.StartObject();

  writer.Key("SdkAppId");
  writer.Int(recResult.SdkAppId);

  writer.Key("RoomId");
  writer.Int(recResult.RoomId);

  writer.Key("UserId");
  writer.String(recResult.UserId.c_str());

  writer.Key("StrRoomId");
  writer.String(recResult.StrRoomId.c_str());

  writer.Key("Status");
  writer.String(recResult.Status.c_str());

  writer.Key("Result");
  writer.StartArray();
  for (int i = 0; i < recResult.Result.size(); i++)
  {

    RegText iResult = recResult.Result[i];
    writer.StartObject();

    writer.Key("Voice");
    writer.String(iResult.Voice.c_str());

    writer.Key("Index");
    writer.Int(iResult.Index);

    writer.Key("StartTime");
    writer.Int(iResult.StartTime);

    writer.Key("EndTime");
    writer.Int(iResult.EndTime);

    writer.Key("Message");
    writer.String(iResult.Message.c_str());

    writer.EndObject();
  }
  writer.EndArray();
  writer.EndObject();

  std::string strResult = s.GetString();
  // 打印生成的json字符串
  //  printf("strJson: %s", strResult.c_str());

  return strResult;
}

void ParseUrl(const std::string &mUrl, std::string &schema, std::string &domain, std::string &path)
{
  std::string::size_type n;
  std::string url = mUrl;
  std::string protol = "http://";

  if (url.substr(0, 7) == "http://")
  {
    url.erase(0, 7);
  }

  if (url.substr(0, 8) == "https://")
  {
    protol = "https://";
    url.erase(0, 8);
  }

  n = url.find('/');
  if (n != std::string::npos)
  {
    domain = url.substr(0, n);
    path = url.substr(n);
  }
  else
  {
    domain = url;
    path = "/";
  }
  schema = protol + domain;
}

void Post(httplib::Client *cli, const std::string &path, const std::string &body)
{
  printf("------------------------------\n");
  printf("callback recognition result: %s\n", body.c_str());

  int retryNum = 2;
  int count = 0;
  std::string contentType = "application/json";
  while (count < retryNum)
  {
    auto res = cli->Post(path.c_str(), body, contentType.c_str());
    if (res != nullptr && res->status < 500)
    {
      //      printf("callback success, resp cntent: %s", res->body.c_str());
      break;
    }
    printf("callback failed, retry...");
    count++;
  }
}

// 识别结果推送策略：设置基准index和voice_text并push进queue，通过比较基准index和voice_text判断OnRecognitionResultChange回调结果是否发生变化
// 如果不变化则不做操作
// 当index或voice_text出现变化，则意味识别结果发生变化，更新基准信息，并push新的基准语音进queue回调
void RealTimeCompareAndPush(std::string &status, SpeechRecognitionResponse *rsp)
{
  auto recognizer_it = voiceUsers_.find(rsp->voice_id);
  if (recognizer_it != voiceUsers_.end())
  {
    std::string voice_text_str = rsp->result.voice_text_str;
    int index = rsp->result.index;

    if (index != *recognizer_it->second->base_voice_index || voice_text_str.compare(*recognizer_it->second->base_voice_text_str))
    {
      RegText regText;
      regText.Message = rsp->message;
      regText.Voice = rsp->result.voice_text_str;
      regText.Index = rsp->result.index;
      regText.StartTime = rsp->result.start_time;
      regText.EndTime = rsp->result.end_time;

      // 如果实时识别结果变化频次>5，下一次变化时立即将列表中缓存的和本次识别结果push队列做回调
      // 如果变化频次<=5,说明实时识别结果变化很快，用户一直在说明，这种场景不适合回调每一次识别结果
      // 用一个数组先缓存识别结果，达到5次批量做回调
      // 如果缓存数未到5，同时频次变化>5,将缓存的结果跟本次结果合并push队列
      if (*recognizer_it->second->change_count > 5 || *recognizer_it->second->base_voice_index == -1)
      {
        // 当前实时识别结果推送queue
        std::cout << "message not change for " << *recognizer_it->second->change_count << " times, push to queue..." << std::endl;
        RecResult recResult;
        recResult.SdkAppId = recognizer_it->second->GetSdkAppId();
        recResult.RoomId = recognizer_it->second->GetRoomId();
        recResult.UserId = recognizer_it->second->GetUserId();
        recResult.StrRoomId = recognizer_it->second->GetStrRoomId();
        recResult.Status = status;

        std::vector<RegText> result = *recognizer_it->second->GetRegList();
        result.push_back(regText);
        recResult.Result = result;
        recognizer_it->second->ClearList();

        std::string strRecResult = MarshalJson(recResult);
        recognizer_it->second->Produce(strRecResult);
      }
      else
      {
        recognizer_it->second->PushBack(regText);
        // 如果数组保存识别结果达到5条，push进队列批量回调
        if (recognizer_it->second->RegListSize() >= 5)
        {
          //          std::cout << "start push speed recognition result to queue..." << std::endl;
          RecResult recResult;
          recResult.SdkAppId = recognizer_it->second->GetSdkAppId();
          recResult.RoomId = recognizer_it->second->GetRoomId();
          recResult.UserId = recognizer_it->second->GetUserId();
          recResult.StrRoomId = recognizer_it->second->GetStrRoomId();
          recResult.Status = status;
          recResult.Result = *recognizer_it->second->GetRegList();
          recognizer_it->second->ClearList();
          std::string strRecResult = MarshalJson(recResult);
          recognizer_it->second->Produce(strRecResult);
        }
      }
      // 更新基准voice识别结果
      *recognizer_it->second->base_voice_index = index;
      *recognizer_it->second->base_voice_text_str = voice_text_str;
      *recognizer_it->second->change_count = 0;
    }
    else
    {
      *recognizer_it->second->change_count += 1;
      std::cout << "real-time recognition result is sample, ignore..." << std::endl;
    }
  }
}

// 将实时识别结果加入列表，批量回调
void RealTimePushQueue(std::string &status, SpeechRecognitionResponse *rsp)
{
  auto recognizer_it = voiceUsers_.find(rsp->voice_id);
  int textLimit = 5; // 批量推送数
  if (recognizer_it != voiceUsers_.end())
  {
    RegText regText;
    regText.Message = rsp->message;
    regText.Voice = rsp->result.voice_text_str;
    regText.Index = rsp->result.index;
    regText.StartTime = rsp->result.start_time;
    regText.EndTime = rsp->result.end_time;
    recognizer_it->second->PushBack(regText);
    // 如果数组保存识别结果达到5条，push进队列批量回调
    if (recognizer_it->second->RegListSize() >= textLimit)
    {
      // std::cout << "start push speed recognition result to queue..." << std::endl;
      RecResult recResult;
      recResult.SdkAppId = recognizer_it->second->GetSdkAppId();
      recResult.RoomId = recognizer_it->second->GetRoomId();
      recResult.UserId = recognizer_it->second->GetUserId();
      recResult.StrRoomId = recognizer_it->second->GetStrRoomId();
      recResult.Status = status;
      recResult.Result = *recognizer_it->second->GetRegList();
      recognizer_it->second->ClearList();
      std::string strRecResult = MarshalJson(recResult);
      recognizer_it->second->Produce(strRecResult);
    }
  }
}

// 将识别结果序列化后push队列
void PushQueue(std::string &status, SpeechRecognitionResponse *rsp)
{
  //  printf("start push speed recognition message to queue...");
  RecResult recResult;
  auto recognizer_it = voiceUsers_.find(rsp->voice_id);
  if (recognizer_it != voiceUsers_.end())
  {
    recResult.SdkAppId = recognizer_it->second->GetSdkAppId();
    recResult.RoomId = recognizer_it->second->GetRoomId();
    recResult.UserId = recognizer_it->second->GetUserId();
    recResult.StrRoomId = recognizer_it->second->GetStrRoomId();
  }
  recResult.Status = status;

  std::vector<RegText> result = *recognizer_it->second->GetRegList();
  RegText regText;
  regText.Message = rsp->message;
  regText.Voice = rsp->result.voice_text_str;
  regText.Index = rsp->result.index;
  regText.StartTime = rsp->result.start_time;
  regText.EndTime = rsp->result.end_time;
  result.push_back(regText);
  recResult.Result = result;
  recognizer_it->second->ClearList();

  std::string strRecResult = MarshalJson(recResult);
  recognizer_it->second->Produce(strRecResult);
}

// 开始识别回调函数
std::string gettime()
{
  time_t rawtime;
  struct tm info;
  char buffer[80];
  time(&rawtime);

  localtime_r(&rawtime, &info);

  strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &info);
  return buffer;
}

// 识别开始
void OnRecognitionStart(SpeechRecognitionResponse *rsp)
{
  std::cout << gettime() << "| OnRecognitionStart | " << rsp->voice_id << std::endl;
}

// 识别失败回调
void OnFail(SpeechRecognitionResponse *rsp)
{
  std::cout << gettime() << "| OnFail |" << rsp->code << " failed message"
            << rsp->message << " voice_id " << rsp->voice_id << std::endl;

  // 识别失败结果push进回调队列
  std::string status_fail = "fail";
  PushQueue(status_fail, rsp);
}

// 识别到一句话的开始
void OnSentenceBegin(SpeechRecognitionResponse *rsp)
{
  std::string text = rsp->result.voice_text_str;
#ifdef _WIN32
  text = utf8_to_gbk(text);
#endif
  std::cout << gettime() << "| OnSentenceBegin | rsp text " << text
            << " voice_id " << rsp->voice_id << std::endl;
}

// 识别到一句话的结束
void OnSentenceEnd(SpeechRecognitionResponse *rsp)
{
  std::string text = rsp->result.voice_text_str;
#ifdef _WIN32
  text = utf8_to_gbk(text);
#endif

  std::cout << gettime() << "| OnSentenceEnd | rsp text " << text
            << " voice_id " << rsp->voice_id << std::endl;

  // 一句话识别结果push回调队列
  std::string status_normal = "normal";
  if (!text.empty())
  {
    PushQueue(status_normal, rsp);
  }
}

// 识别结果发生变化回调
void OnRecognitionResultChange(SpeechRecognitionResponse *rsp)
{
  std::string text = rsp->result.voice_text_str;
#ifdef _WIN32
  text = utf8_to_gbk(text);
#endif

  std::cout << gettime() << "| OnRecognitionResultChange | rsp text " << text << " voice_id " << rsp->voice_id << std::endl;

  int64_t sentenceEndTime = GetNowTimestampMS();
  printf("------------------------------\n");
  printf("[%d] sentence asr recognition end timestamp: %lld\n", asrCallbackNum, sentenceEndTime);
  asrCallbackNum++;
  std::string status_normal = "normal";

  // 实时识别结果push回调队列
  // 如果配置了ENV_REALTIME_ENABLE环境变量，将实时结果回调给业务方
  // 否则一句话识别中间结果不返回，只返回一句话识别结束后的结果
  char *enableVar;
  if ((enableVar = getenv(envRealtimeEnable)) && !text.empty())
  {
    RealTimeCompareAndPush(status_normal, rsp);
  }
  // RealTimePushQueue(status_normal, rsp);
}

// 识别完成回调
void OnRecognitionComplete(SpeechRecognitionResponse *rsp)
{
  std::cout << gettime() << "| OnRecognitionComplete | voice_id:" << rsp->voice_id << std::endl;

  // 识别结束后push回调队列
  std::string status_finished = "finished";
  PushQueue(status_finished, rsp);
}

void Consumer::runConsume()
{
  std::string schema = "";
  std::string domain = "";
  std::string path = "";

  ParseUrl(callback_url, schema, domain, path);
  // printf("callback url: scheme:%s, path: %s", schema.c_str(), path.c_str());

  httplib::Client cli(schema);
  cli.enable_server_certificate_verification(false);
  cli.set_keep_alive(true);

  while (!m_stop)
  {
    //    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::lock_guard<std::mutex> lgd(*m_mt);
    if (!m_que->empty())
    {
      std::vector<RegText> Result;
      std::string postData = m_que->front();
      m_que->pop();
      if (m_que->size() > 0)
      {
        printf("message queue size: %d\n", m_que->size());
      }

      // 回调业务后端
      // std::cout << "Consume data:" << postData << std::endl;

      int64_t callbackStartTime = GetNowTimestampMS();
      Post(&cli, path, postData);

      // 整体链路时间戳
      int64_t endTime = GetNowTimestampMS();
      printf("------------------------------\n");
      printf("[%d] sentence end timestamp: %lld\n", sentenceCallbackNum, endTime);

      // 统计回调链路耗时
      printf("------------------------------\n");
      printf("[%d] sentence callback cost time: %lld(ms)\n", sentenceCallbackNum, endTime - callbackStartTime);
      sentenceCallbackNum++;
    }
  }
  //  printf("consume stopped...");
  stop_flag = true;
}

AsrStreamRecognizer::AsrStreamRecognizer(const char *remote_user_id, ASRParam *param)
{
  appid = param->appid;
  secret_id = param->secret_id;
  secret_key = param->secret_key;

  sdk_app_id = param->sdk_app_id;
  room_id = param->room_id;
  str_room_id = param->str_room_id;
  remote_user_id_ = remote_user_id;

  reg_list = new std::vector<RegText>();
  base_voice_text_str = new std::string("");
  base_voice_index = new int(-1);
  change_count = new int(0);

  //  生成voice id
  char str[32] = {0};
  memset(str, 0, 32);
  srand(time(0) + TCloudUtil::gettid()); // 默认根据线程号生成voice_id
  snprintf(voice_id, sizeof(str), "%d", rand());

  // 启动回调消费线程，循环读取队列
  consumer = new Consumer(&m_que, &m_mt, param->callback_url);

  recognizer = new SpeechRecognizer(appid, secret_id, secret_key);
  recognizer->SetOnRecognitionStart(OnRecognitionStart);
  recognizer->SetOnFail(OnFail);
  recognizer->SetOnRecognitionComplete(OnRecognitionComplete);
  recognizer->SetOnRecognitionResultChanged(OnRecognitionResultChange);
  recognizer->SetOnSentenceBegin(OnSentenceBegin);
  recognizer->SetOnSentenceEnd(OnSentenceEnd);

  std::string defaultEngineModelType = "16k_zh";
  char *envEngineModelType;
  if ((envEngineModelType = getenv(envEngineType)) && envEngineModelType == "16k_en")
  {
    defaultEngineModelType = envEngineModelType;
  }
  recognizer->SetEngineModelType(defaultEngineModelType); // 可选16k_zh/16k_en
  recognizer->SetNeedVad(1);
  //  recognizer->SetHotwordId("123456789");
  //  recognizer->SetFilterDirty(1);
  //  recognizer->SetFilterModal(1);
  //  recognizer->SetFilterPunc(1);
  recognizer->SetConvertNumMode(1);
  recognizer->SetWordInfo(0);
  recognizer->SetVoiceId(voice_id);

  // recognizer->SetVadSilenceTime(500);
}

AsrStreamRecognizer::~AsrStreamRecognizer()
{
  std::lock_guard<std::mutex> lock(mutex_);
  recognizer->Stop();
  delete recognizer;
}

// 16k采样率，每次发送200ms时长数据到asr，对应6400字节长数据
void AsrStreamRecognizer::AddAudioFrame(void *frame)
{
  if (frame)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    recognizer->Write(frame, frame_len);
  }
}

// void AsrStreamRecognizer::AddAudioFrame(uint8_t *data, uint32_t length) {
//   if (data) {
//     std::lock_guard <std::mutex> lock(mutex_);

//     if (asr_frame == nullptr) {
//       asr_frame = new std::uint8_t[frame_len];
//     }

//     if (frame_num * length < frame_len ) {
//       std::memcpy(asr_frame + frame_num * length, data, length);
//       frame_num++;
//     } else {
// //      printf(" audio frame len:%d\n", frame_num * length);
//       int64_t startTime = GetNowTimestampMS();
//       printf("------------------------------\n");
//       printf("[%d] sentence start timestamp: %lld\n", sentenceNum, startTime);
//       sentenceNum++;

// //      printf("------------------------------\n");
// //      printf("[%d] sentence asr recognition start timestamp: %lld\n", sentenceNum, startTime);
// //      sentenceNum++;

//       recognizer->Write(asr_frame, frame_len);

//       asr_frame = nullptr;
//       frame_num = 0;
//     }
//   }
// }

void AsrStreamRecognizer::Produce(std::string &message)
{
  std::lock_guard<std::mutex> lgd(m_mt);
  m_que.push(message);
  //  std::cout << "Producer produce message:" << message << std::endl;
}

void AsrStreamRecognizer::Start()
{
  if (consumer)
  {
    consumer->start();
    ;
  }

  if (recognizer)
  {
    recognizer->Start();
  }
}

void AsrStreamRecognizer::Stop()
{
  std::lock_guard<std::mutex> lock(mutex_);
  if (recognizer)
  {
    printf("stop recognizer...");
    recognizer->Stop();
    delete recognizer;
  }

  if (consumer)
  {
    printf("stop consumer...");
    consumer->stop();
  }

  // 等待recognizer和consumer退出
  while (!ConsumeStopped())
  {
    sleep(1);
  }
}

#pragma mark - TencentSDKWarperRecognizer
namespace
{
  constexpr int REASON_FAIL = 2;
  constexpr int REASON_OK = 1;
  constexpr int REASON_UNKNOWN = 0;
}; // namespace

TencentSDKWarperRecognizer::TencentSDKWarperRecognizer(uint32_t sdkappid, ASRParam *param)
    : sdkappid_(sdkappid)
{

  asrParam = param;

  // 初始化房间组件
  room_ = createInstance(sdkappid_);
  room_->setCallback(this);
}

TencentSDKWarperRecognizer::~TencentSDKWarperRecognizer()
{
  if (room_)
  {
    room_->setCallback(nullptr);
    destroyInstance(room_);
    room_ = NULL;
  }

  if (stream_recognizers_.size())
  {
    stream_recognizers_.clear();
  }

  if (voiceUsers_.size())
  {
    voiceUsers_.clear();
  }
}

void TencentSDKWarperRecognizer::StartRecord(TRTCParams &params,
                                             const char *record_roomid,
                                             int output,
                                             int timeout)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (started_)
  {
    return;
  }
  started_ = true;

  timeout_ = timeout;

  if ((output & OUTFILE_TYPE_AUDIO) || (output & OUTFILE_TYPE_AUDIO_VIDEO))
  {
    need_record_audio_ = true;
  }

  if ((output & OUTFILE_TYPE_VIDEO) || (output & OUTFILE_TYPE_AUDIO_VIDEO))
  {
    need_record_video_ = true;
  }

  if (room_)
  {
    room_->enterRoom(params, TRTCAppScene::TRTCAppSceneVideoCall);
  }
}

void TencentSDKWarperRecognizer::StopRecord()
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (!started_)
  {
    return;
  }
  started_ = false;

  if (room_)
  {
    room_->exitRoom();
  }

  for (auto rec : stream_recognizers_)
  {
    rec.second->Stop();
  }

  stream_recognizers_.clear();

  voiceUsers_.clear();
}

bool TencentSDKWarperRecognizer::IsRunning(int &reason)
{
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (exit_reason_ == REASON_FAIL)
  {
    return false;
  }
  /**
   * 超时时间到了且没有人就退出，若没有设置超时，就一直等有人进入。
   */
  if ((exit_reason_ == REASON_OK || timeout_ != 0) && anchor_count_ == 0)
  {
    if (no_anchor_ts_ == 0)
    {
      no_anchor_ts_ = GetTimestamps();
    }

    if (GetTimestamps() - no_anchor_ts_ > timeout_)
    {
      return false;
    }
    return true;
  }

  return true;
}

#pragma mark - ITRTCCloudCallback

void TencentSDKWarperRecognizer::onError(TXLiteAVError error_code,
                                         const char *error_message,
                                         void *arg)
{
  // ITRTCCloud 组件内部报错，说明功能不可用了。建议打印错误信息，然后退出房间。
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  exit_reason_ = REASON_FAIL;
}

void TencentSDKWarperRecognizer::onEnterRoom(uint64_t elapsed)
{
  if (ENABLE_CUSTOM_DECRYPT)
  {
    room_->setDecryptionCallback(this);
  }
}

void TencentSDKWarperRecognizer::onUserEnter(const char *remote_user_id)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    ++anchor_count_;
    exit_reason_ = REASON_OK;
    if (!started_)
    {
      // 未开始或已近结束中，不处理后续流程。
      return;
    }
  }

  // 用户进入房间，创建AsrStreamRecognizer实例并启动consumer线程和recognizer线程
  auto recognizer_it = stream_recognizers_.find(remote_user_id);
  if (recognizer_it == stream_recognizers_.end())
  {
    AsrStreamRecognizer *recognizeWrapper = new AsrStreamRecognizer(remote_user_id, asrParam);
    stream_recognizers_[remote_user_id] = recognizeWrapper;
    voiceUsers_[recognizeWrapper->GetVoiceId()] = recognizeWrapper;
    // 启动consumer线程和recognizer线程
    recognizeWrapper->Start();
  }

  if (room_)
  {
    if (need_record_audio_)
    {
      // 音频格式设置：TRTCAudioFrameFormat_PCM_16KHZ_MONO
      room_->setRemoteAudioRecvCallback(
          remote_user_id, TRTCAudioFrameFormat::TRTCAudioFrameFormat_PCM_16KHZ_MONO, this);
    }
    if (need_record_video_)
    {
      room_->setRemoteVideoRecvCallback(
          remote_user_id, TRTCVideoFrameFormat::TRTCVideoFrameFormat_YUVI420,
          this);
    }
  }
}

void TencentSDKWarperRecognizer::onUserExit(const char *remote_user_id,
                                            int reason)
{
  {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    --anchor_count_;
    if (anchor_count_ == 0)
    {
      no_anchor_ts_ = 0;
    }
  }

  // 不关心 userid 的 音视频数据了，主动设置一下是个好习惯。
  if (room_)
  {
    if (need_record_audio_)
    {
      room_->setRemoteAudioRecvCallback(
          remote_user_id, TRTCAudioFrameFormat::TRTCAudioFrameFormat_Unknown,
          nullptr);
    }
    if (need_record_video_)
    {
      room_->setRemoteVideoRecvCallback(
          remote_user_id, TRTCVideoFrameFormat::TRTCVideoFrameFormat_Unknown,
          nullptr);
    }
  }
  //用户退出房间，停止识别，清理资源
  if (need_record_audio_)
  {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto recognizer_it = stream_recognizers_.find(remote_user_id);
    if (recognizer_it != stream_recognizers_.end())
    {
      if (recognizer_it->second)
      {
        printf("stop recognize and consume...");
        recognizer_it->second->Stop();

        std::string voice_id = recognizer_it->second->GetVoiceId();
        auto user_it = voiceUsers_.find(voice_id);
        std::cout << "voiceUsers_ size:" << voiceUsers_.size() << std::endl;
        voiceUsers_.erase(user_it);
        std::cout << "after erase, voiceUsers_ size:" << voiceUsers_.size() << std::endl;
        std::cout << "stream_recognizers_ size:" << stream_recognizers_.size() << std::endl;
        stream_recognizers_.erase(recognizer_it);
        std::cout << "after erase, stream_recognizers_ size:" << stream_recognizers_.size() << std::endl;
      }
    }
  }
}

bool TencentSDKWarperRecognizer::onVideoFrameDecrypt(
    const char *remote_user_id,
    TRTCVideoStreamType stream_type,
    TRTCCustomEncryptionData *data)
{
  // 必须是同步进行解密。
  // data 的内存由SDK负责申请和释放的。
  // 针对主视频流进行解密，解密算法 按位取反。
  printf("video decrypt len:%d!\n", data->EncryptedDataLength);
  if (stream_type == TRTCVideoStreamType::TRTCVideoStreamTypeBig)
  {
    for (int i = 0; i < data->EncryptedDataLength; i++)
    {
      data->unEncryptedData[i] = ~data->EncryptedData[i];
    }
  }
  // 很重要，加密后的数据的长度。
  data->unEncryptedDataLength = data->EncryptedDataLength;
  return true;
}

bool TencentSDKWarperRecognizer::onAudioFrameDecrypt(
    const char *remote_user_id,
    TRTCCustomEncryptionData *data)
{
  printf(" audio decrypt len:%d\n", data->EncryptedDataLength);
  for (int i = 0; i < data->EncryptedDataLength; i++)
  {
    data->unEncryptedData[i] = ~data->EncryptedData[i];
  }
  data->unEncryptedDataLength = data->EncryptedDataLength;
  return true;
}

#pragma mark - ITRTCAudioRecvCallback

void TencentSDKWarperRecognizer::onRecvAudioFrame(const char *remote_user_id,
                                                  TRTCAudioFrame *frame)
{
  if (!need_record_audio_)
  {
    return;
  }

  AsrStreamRecognizer *recognizer = nullptr;
  {
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto recognizer_it = stream_recognizers_.find(remote_user_id);
    if (recognizer_it == stream_recognizers_.end())
    {
      recognizer = new AsrStreamRecognizer(remote_user_id, asrParam);
      stream_recognizers_[remote_user_id] = recognizer;
      voiceUsers_[recognizer->GetVoiceId()] = recognizer;
    }
    else
    {
      recognizer = recognizer_it->second;
    }
  }

  if (recognizer)
  {
    // printf(" audio frame len:%d\n", frame->length);
    // printf(" audio frame timestamp:%d\n", frame->timestamp);
    // recognizer->AddAudioFrame(frame->data, frame->length);
    recognizer->AddAudioFrame(frame->data);
  }
}

#pragma mark - ITRTCVideoRecvCallback

void TencentSDKWarperRecognizer::onRecvVideoFrame(const char *remote_user_id,
                                                  TRTCVideoStreamType stream_type,
                                                  TRTCVideoFrame *frame)
{
  if (!need_record_video_)
  {
    return;
  }
  printf("receive video frame...");
}

#pragma mark - ITRTCMediaRecorderCallback

void TencentSDKWarperRecognizer::onFinished(const char *user_id,
                                            int output,
                                            const char *file_path)
{
}
