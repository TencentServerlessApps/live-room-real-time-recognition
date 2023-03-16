#ifndef _TENCENT_SDK_WAPPER_RECORD_H__
#define _TENCENT_SDK_WAPPER_RECORD_H__

#include <stdio.h>
#include <string.h>
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
#include <queue>
#include "ITRTCMediaRecorder.h"
#include "TRTCCloud.h"
#include "httplib.h"
#include "speech_recognizer.h"

enum RecordMode {
    /**
     * the recorded stream will be affected by audio mute or video mute. only use
     * recorder moudle.
     */
    kRecordWithoutMix = 0,

    /**
     * compatibility better than kRecordWithoutMix. using mixer moudle + recorder
     * moudle.
     */
    kRecordWithMix = 1,
};

// ASRParam
typedef struct {
    std::string appid;
    std::string secret_id;
    std::string secret_key;
    int sdk_app_id;
    int room_id;
    std::string str_room_id;
    std::string callback_url;
} ASRParam;

// 回调封装结构
typedef struct {
    std::string Voice;
    int Index;
    int StartTime;
    int EndTime;
    std::string Message;
} RegText;

typedef struct {
    int SdkAppId;
    int RoomId;
    std::string UserId;
    std::string StrRoomId;
    std::string Status;
    std::vector<RegText> Result;
} RecResult;

/*
*单缓冲同步队列，加锁读取
*/
class Consumer {
public:
    Consumer(std::queue<std::string> * que_, std::mutex * mt_, std::string &callback_url_) {
      m_mt = mt_;
      m_que = que_;
      m_stop = false;
      callback_url = callback_url_;
    }

    // 循环消费队列，回调客户后端
    void runConsume();

    void join() {
      m_trd->join();
      m_trd.reset();
    }
    void start() {
      m_trd.reset(new std::thread(std::bind(std::mem_fun(&Consumer::runConsume), this)));
    }
    void stop() {
      m_stop = true;
    }

    bool stopped() {
      return stop_flag;
    }
private:
    std::mutex * m_mt;
    std::queue<std::string> * m_que;
    volatile bool m_stop;
    std::shared_ptr<std::thread> m_trd;
    std::string callback_url;
    bool stop_flag{false};
};

// ASR实时语音识别类，每个用户创建一个实例做识别
class AsrStreamRecognizer {
public:
    AsrStreamRecognizer(const char *remote_user_id, ASRParam *param);

    ~AsrStreamRecognizer();

    /**
     * 实时识别结果基准message，迭代更新message
     */
    std::string *base_voice_text_str;

    /**
     * 实时识别结果index，迭代更新index
     */
    int *base_voice_index;

    /**
     * 计数器，用来判断实时识别结果变化的快慢
     */
    int *change_count;

    std::string GetAppId() {
      return appid;
    }

    std::string GetSecretId() {
      return secret_id;
    }

    std::string GetSecretKey() {
      return secret_key;
    }

    int GetSdkAppId() {
      return sdk_app_id;
    }

    int GetRoomId() {
      return room_id;
    }

    std::string GetStrRoomId() {
      return str_room_id;
    }

    std::string GetUserId() {
      return remote_user_id_;
    }

    char* GetVoiceId() {
      return voice_id;
    }

    /**
     * add audio frame to asr recognizer.
     */
    void AddAudioFrame(void *frame);


    void Produce(std::string &message);

    /**
     * stop asr recognize. start with |AddAudioFrame|
     */
    void Stop();

    // 启动consumer和recognizer
    void Start();

    // 获取consumer结束flag
    bool ConsumeStopped() {
      if (consumer) {
        return consumer->stopped();
      }
      return true;
    }

    void PushBack(RegText &regText) {
      reg_list->push_back(regText);
    }

    void ClearList() {
      reg_list->clear();
    }

    std::vector<RegText>* GetRegList() {
      return reg_list;
    };

    int RegListSize() {
      return reg_list->size();
    }

private:
    std::string appid;
    std::string secret_id;
    std::string secret_key;
    int sdk_app_id;
    int room_id;
    std::string str_room_id;
    std::string remote_user_id_;
    /**
     * 每个用户对应一个voice_id，用于封装识别回调
     */
    char voice_id[32] = { 0 };

    std::mutex mutex_;

    SpeechRecognizer *recognizer{nullptr};

    Consumer *consumer{nullptr};

    std::mutex m_mt;
    /**
     * 回调队列，生产-消费模式回调识别结果
     */
    std::queue<std::string> m_que;

    /**
     * 保存一组识别结果，批量push进mq
     */
    std::vector<RegText> *reg_list{nullptr};

    /**
     * 每次上传音频数据字节数，保持1:1实施率，否则识别不准确
     */
    size_t frame_len{6400};

    /**
     * 每次上传音频数据
     */
    std::uint8_t *asr_frame{nullptr};

    /**
     * 上传音频数据批量数
     */
    int frame_num{0};

    /**
     * 纯音频，纯视频和音视频模式，默认纯音频模式。
     */
    bool need_record_audio_{true};
    bool need_record_video_{true};
};

/**
 *  语音识别封装类，继承ITRTC接口类。
 */
using CMapStreamRecognizers = std::map<std::string, AsrStreamRecognizer*>;

class TencentSDKWarperRecognizer : public ITRTCCloudCallback,
                               public ITRTCAudioRecvCallback,
                               public ITRTCVideoRecvCallback,
                               public ITRTCDecryptionCallback,
                               public ITRTCMediaRecorderCallback {
public:
    TencentSDKWarperRecognizer(uint32_t sdkappid, ASRParam *param);

    virtual ~TencentSDKWarperRecognizer();

    /**
     * 启动语音识别
     * params   房间参数。
     * output   编码文件类型，支持同时录纯音频，纯视频和音视频文件
     */
    void StartRecord(TRTCParams &params,
                     const char *record_roomid,
                     int output,
                     int timeout);

    /**
     * 停止所有正在识别的流，退出房间。
     */
    void StopRecord();

    /**
     * 当前识别状态
     *  reason 若返回false，reason存放导致false的原因。
     *   1. 主播全部退出房间自动结束，
     *   2. 进房失败退出
     */
    bool IsRunning(int &reason);

protected:
    // ITRTCCloudCallback.
    void onError(TXLiteAVError error_code,
                 const char *error_message,
                 void *arg) override;

    void onWarning(TXLiteAVWarning warn_code,
                   const char *warn_message,
                   void *arg) override {}

    void onEnterRoom(uint64_t elapsed) override;

    void onExitRoom(int reason) override {}

    void onUserEnter(const char *remote_user_id) override;

    void onUserExit(const char *remote_user_id, int reason) override;

    void onFirstVideoFrame(const char *remote_user_id,
                           uint32_t width,
                           uint32_t height) override {}

    void onFirstAudioFrame(const char *remote_user_id) override {}

    void onUserVideoAvailable(const char *remote_user_id,
                              bool available) override {};

    void onUserAudioAvailable(const char *remote_user_id,
                              bool available) override {};

    void onConnectionLost() override {};

    void onTryToReconnect() override {};

    void onConnectionRecovery() override {};

    void onRecvSEIMsg(const char *remote_user_id,
                      const unsigned char *data,
                      int data_length) override {}

    // ITRTCAudioRecvCallback
    void onRecvAudioFrame(const char *remote_user_id,
                          TRTCAudioFrame *frame) override;

    // ITRTCVideoRecvCallback
    void onRecvVideoFrame(const char *remote_user_id,
                          TRTCVideoStreamType stream_type,
                          TRTCVideoFrame *frame) override;

    // ITRTCMediaRecorderCallback
    void onFinished(const char *remote_user_id,
                    int output,
                    const char *file_path) override;

    // ITRTCDecryptionCallback
    bool onAudioFrameDecrypt(const char *remote_user_id,
                             TRTCCustomEncryptionData *data) override;

    bool onVideoFrameDecrypt(const char *remote_user_id,
                             TRTCVideoStreamType stream_type,
                             TRTCCustomEncryptionData *data) override;

private:
    /**
     * SDK 提供的房间组件
     */
    ITRTCCloud *room_{nullptr};

    /**
     * 客户sdkappid，ITRTCMediaRecorder 需要用到。
     */
    uint32_t sdkappid_{0};

    /**
     * 房间主播人数（不包括自己）
     */
    uint32_t anchor_count_{0};

    /**
     * 退房原因
     */
    uint32_t exit_reason_{0};

    std::recursive_mutex mutex_;

    /**
     * 组件启动状态，做防重入。start 或 stop 连续调用两次，第二次直接返回失败。
     */
    bool started_{false};

    /**
     * 录制输出要求。
     */
    bool need_record_audio_{false};
    bool need_record_video_{false};

    /**
     * 语音识别实例map。
     */
    CMapStreamRecognizers stream_recognizers_;

    /**
     * 录制模式，参见，默认是补帧录制。
     */
    RecordMode mode_{kRecordWithMix};

    int timeout_{0};

    uint64_t no_anchor_ts_{0};

    ASRParam *asrParam{nullptr};
};

#endif  //_TENCENT_SDK_WAPPER_RECOGNIZER_H__
