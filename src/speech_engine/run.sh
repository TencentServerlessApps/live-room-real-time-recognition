#!/bin/bash
#
# 参数说明
#  appId        用户ID
#  secretId     密钥ID
#  secretKey    密钥key
#  sdkAppId     应用ID
#  demoSences   1. 推本地MP4文件；2.单流录制；4.混流录制，8.推文件本地MP4/转推直播流
#  userId       用户ID
#  userSig      用户凭证
#  roomId       房间号
#  recDir       录制文件的存放主目录。demoSences 取2或4有效
#  recFiles     1. 纯音频文件； 2.纯视频文件；4.音视频文件。demoSences 取2或4有效
#
#  sdkAppId + userId + userSig 是进入实时音视频房间的钥匙。如果是测试可以在腾讯云官网申请账号，获取。


#
# 启动实时语音识别。
#
#

bin/AsrSpeechEngine --strRoom=0 --appId 12345 --secretId ${secretid} --secretKey ${secretkey} --callbackUrl=https://xxxx/callback --sdkAppId 12345  --roomId 50175 --demoSences 2 --userId user_55952147 --userSig xxxxxx --recFiles=1 --timeout=15000
