# -*- coding: utf8 -*-
import json
import logging
import os
import subprocess
import sys

from business.common.http import err_resp, callback, response
from business.common import const
from business.common.util import get_value

# 命令定义
cmd_path_asr = './AsrSpeechEngine'
# asr实时语音识别命令
cmd_speech = cmd_path_asr + " --appId=%s --secretId=%s --secretKey=%s --callbackUrl=%s --sdkAppId=%s  --roomId=%s --demoSences 2 --userId=%s --userSig=%s --recFiles=1 --timeout=%s --strRoom=%s"

# 日志配置
logging.basicConfig(level=logging.INFO, stream=sys.stdout)
logger = logging.getLogger()
logger.setLevel(level=logging.INFO)


# 主函数
def main_handler(event, context):
    logger.info("start main handler")
    request_id = context.get('request_id')

    # 请求从API网关传递,通过网关获取TRTC参数，在body中获取
    if "requestContext" not in event.keys():
        return {"code": 410, "errorMsg": "event is invalid"}
    if "body" not in event.keys():
        return {"code": 410, "errorMsg": "event is not come from api gateway"}

    # 解析request请求body
    # 参数错误通过回调返回错误，提示校验参数
    req_body = event['body']
    callback_url = ""
    is_str_room = False
    try:
        trtc_param = json.loads(req_body)
        logger.info("trtc params:" + json.dumps(trtc_param))

        callback_url = get_value(trtc_param, 'Callback')
        sdk_app_id = get_value(trtc_param, 'SdkAppId')
        user_id = get_value(trtc_param, 'UserId')
        user_sig = get_value(trtc_param, 'UserSig')
        private_key = get_value(trtc_param, 'PrivateKey')

        # 自定义环境变量
        secret_id = os.environ.get('SECRET_ID')
        secret_key = os.environ.get('SECRET_KEY')
        app_id = os.environ.get('TENCENTCLOUD_APPID')

        # 是否开启debug模式，默认不开启，开启后实时打印日志
        debug = False
        if os.environ.get('DEBUG'):
            debug = os.environ.get('DEBUG')

        # 进房超时时间，默认60秒,可通过环境变量指定
        timeout = 60000
        if os.environ.get('TIMEOUT'):
            timeout = int(os.environ.get('TIMEOUT'))

        if not callback_url:
            logger.error("bad request: callback_url is empty, please check.")
            raise Exception('callback_url is empty, please check.')

        # 如果未配置密钥环境变量，抛出错误
        if not secret_id or not secret_key:
            logger.error("invalid parameter value: env[SECRET_ID] or env[SECRET_KEY] not set, please check.")
            raise Exception('invalid parameter value: env[SECRET_ID] or env[SECRET_KEY] not set, please check.')

        if not sdk_app_id or not user_id or not user_sig:
            logger.error("bad request: SdkAppId or UserId or UserSig is empty, please check.")
            raise Exception('SdkAppId or UserId or UserSig is empty, please check.')

        # 校验， roomId和strRoomId必传一个，生成录制命令
        if not get_value(trtc_param, 'StrRoomId') and not get_value(trtc_param, 'RoomId'):
            logger.error("bad request: roomId is empty, please check.")
            raise Exception('roomId is empty')

        if get_value(trtc_param, 'StrRoomId') and not get_value(trtc_param, 'RoomId'):
            is_str_room = True

        str_room_flag = 0
        if is_str_room:
            str_room_flag = 1
            room_id = get_value(trtc_param, 'StrRoomId')
        else:
            room_id = get_value(trtc_param, 'RoomId')

    except Exception as err:
        logger.error("request err: %s. request body[%s]" % (err, req_body))
        message = "request err: %s. request body[%s]" % (err, req_body)
        resp = err_resp(const.ERROR_CODE_INVALID_PARAMETER, message, request_id)
        callback(callback_url, resp)
        raise err

    # 启动ASR实时语音识别
    try:
        command = cmd_speech % (
            app_id, secret_id, secret_key, callback_url, sdk_app_id, room_id, user_id, user_sig, timeout, str_room_flag)
        if private_key:
            command = command + " --privateKey={0}".format(private_key)
        print("asr speech recognition command:", command)
        p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,shell=True)
        while p.poll() is None:
            if debug:
                line = p.stdout.readline()
                line = line.strip()
                if line:
                    print('{}'.format(line))
        if p.returncode == 0:
            print('asr speech recognize finished.')
        else:
            print('asr speech recognize failed, stderr: ', p.stderr)
    except Exception as e:
        logging.exception(e)
        message = "execute command failed, err:%s" % (str(e),)
        resp = err_resp(const.ERROR_CODE_INTERNAL_ERROR, message, request_id)
        callback(callback_url, resp)
        raise e

    # 清理工作目录和日志目录
    # try:
    #     logger.info("clear work dir...")
    #     delete_local_file("/tmp")
    # except Exception as err:
    #     logging.exception(err)
    #     pass

    resp = response(request_id)
    print("recognition finished, resp:", resp)

    return json.dumps(resp)
