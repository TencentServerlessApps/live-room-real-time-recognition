# 直播房间实时语音识别服务

## 环境准备

```
- Node.js: v12+
- Serverless Framework CLI:
    Framework Core: 2.67.0
    Plugin: 5.5.1
    SDK: 4.3.0
    Components: 3.18.1
```

## 目录说明
```text
- speech_engine -- 实时语音识别核心逻辑
- src -- 云函数源码
```

## 应用部署说明

默认情况下，推荐使用 `Serverless Framework CLI` 开发者工具进行部署。

### 前置条件
1、执行命令`make package`生成源码包  
2、将源码包解压至指定的项目目录

### 源码编译和打包指导
执行以下命令即可在build目录生成完整源码包：
```
export release_version=xxx  // 待发布应用版本号，如1.0.0
make package
```

### 安装 Serverless Framework

```bash
$ npm install -g serverless
```

更详细说明可以参考 [这里](https://cloud.tencent.com/document/product/583/44753)。

### 配置

将项目目录下 `.env.example` 文件修改为 `.env`，内容如下：

```text
# 应用名称
NAME=xxx

# 腾讯云帐号 SecretID
TENCENT_SECRET_ID=xxx
# 腾讯云帐号 SecretID
TENCENT_SECRET_KEY=xxx

# 地域
REGION=ap-chengdu

// 自定义环境变量secretID
TENCENT_CUSTOMER_SECRET_ID=xxx

// 自定义环境变量secretKey
TENCENT_CUSTOMER_SECRET_KEY=xxx
```

> 注意：请保证函数、私有网络、Redis、文件存储和对象存储均在同一个地域。

### 执行部署

直接在项目目录下执行 `sls deploy` 命令即可。

## 测试指导

### 调试流程

1、用户进入TRTC房间

2、调用语音识别接口，录制用户进入相同的TRTC房间发起实时语音识别

3、实时识别结果通过回调接口回调给客户业务后台，客户侧做后续处理。

## 接口说明

通过调用API网关接口启动实时语音识别。接口参数说明：

```
{
	"SdkAppId": 1400000000,
	"RoomId": 43474,
	"UserId": "user_55952145",
	"UserSig": "eJwtzNEKgkAUBNBxxxxxxx",
	"Callback": "https:xxxxxxxx.com/post/xxx"
}
```

参数说明：
```
| 参数      | 类型   | 必填 | 说明                                                         |
| --------- | ------ | ---- | ------------------------------------------------------------ |
| SdkAppId  | Int    | 是   | 应用 ID，用于区分不同 TRTC 应用。                            |
| RoomId    | Int    | 否   | 整型房间号ID，用于在一个 TRTC 应用中唯一标识一个房间。       |
| StrRoomId | String | 否   | 字符串房间号 ID，RoomId 与 StrRoomId 必须配置一项，如果 RoomId 与 StrRoomId 同时配置，使用 RoomId。 |
| UserId    | String | 是   | 录制用户 ID，用于在一个 TRTC 应用中唯一标识一个用户。        |
| UserSig   | String | 是   | 录制用户签名，用于对一个用户进行登录鉴权认证。               |
| Callback  | String | 否   | 录制结束后的回调地址，并使用 POST 方式进行回调。             |
```

### 识别结果回调接口
```
{
	"RequestID": "95941e2c85898384a95b81c2a5******",
	"SdkAppId": 1400000000,
	"RoomId": 43474,
	"UserId": "user_55952145",
	"Status": "recognizing/finished",
	"Result": [{
		"Voice": "实时语音识别",
		"Index": 0,
		"StartTime": 0,
		"EndTime": 1024,
		"Message": "success"
	}]
}
```
参数说明：
```
| 参数      | 类型   | 必填 | 说明                                  |
| --------- | ------ | ---- | ------------------------------------- |
| SdkAppId  | Int    | 是   | 应用 ID。                             |
| RoomId    | int    | 是   | 整型房间 ID。                         |
| UserId    | String | 是   | 识别的用户 ID。                       |
| StrRoomId | String | 是   | 字符串房间 ID。                       |
| Result    | Array  | 是   | 语音识别结果[{},{},{},{}]             |
| Status    | String | 是   | 当前用户语言识别状态，normal/finished |

Result为数组类型，元素封装为 JSON对象，封装格式如下：

| 参数名称  | 类型    | 必选 | 描述                                                     |
| --------- | ------- | ---- | -------------------------------------------------------- |
| Voice     | String  | 是   | 当前一句话文本结果，编码为 UTF8。                        |
| Index     | Integer | 是   | 当前一句话结果在整个音频流中的序号，从0开始逐句递增。    |
| StartTime | Integer | 是   | 当前一句话结果在整个音频流中的起始时间。                 |
| EndTime   | Integer | 是   | 当前一句话结果在整个音频流中的结束时间。                 |
| Message   | String  | 是   | 识别任务的执行结果。例如，识别结束，识别中，识别失败等。 |

```
