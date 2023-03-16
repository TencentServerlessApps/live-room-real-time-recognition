# -*- coding: utf8 -*-
import os
import subprocess
from business.common import TLSSigAPIv2


# 用于签发TRTC服务中必须要使用的UserSig鉴权票据
# 默认过期时间24小时
def gen_sig(sdk_appId, user_id, key):
    api = TLSSigAPIv2.TLSSigAPIv2(sdk_appId, key)
    user_sig = api.genUserSig(user_id, 86400)
    return user_sig


# 获取进程id
def get_pid(process):
    cmd = "ps aux| grep '%s'|grep -v grep " % process
    out = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
    infos = out.stdout.read().splitlines()
    pid_list = []
    if len(infos) >= 1:
        for i in infos:
            pid = i.split()[1]
            if pid not in pid_list:
                pid_list.append(pid)
        return pid_list
    else:
        return -1


# 删除本地文件
def delete_local_file(src):
    if os.path.isfile(src):
        try:
            os.remove(src)
        except:
            pass
    elif os.path.isdir(src):
        for item in os.listdir(src):
            item_src = os.path.join(src, item)
            delete_local_file(item_src)
        try:
            os.rmdir(src)
        except:
            pass


# 根据录制文件名称解析出主播id
def get_anchor_id(path):
    _file = os.path.basename(path)
    params = _file.split('.')[0].split('-')
    user_id = ""
    if len(params) > 4:
        user_id = params[4]

    return user_id


# 录制模式
def record_mode(mode):
    # 录制模式
    scene = 2  # 2.单流录制；4.混流录制
    recorder_mode = 1  # 1. 纯音频文件； 2.纯视频文件；4.音视频文件
    file_type = 0  # 0: MP3文件； 1: MP4文件

    # 单流纯音频录制
    if mode == '00':
        scene = 2
        recorder_mode = 1
        file_type = 0

    # 单流纯视频录制
    if mode == '01':
        scene = 2
        recorder_mode = 2
        file_type = 1

    # 单流音视频录制
    if mode == '02':
        scene = 2
        recorder_mode = 4
        file_type = 1

    return scene, recorder_mode, file_type


def get_value(data, key=''):
    value = None
    if data and len(key) > 0:
        if data.get(key):
            value = data.get(key)

        lower_key = key[0].lower() + key[1:]
        if data.get(lower_key):
            value = data.get(lower_key)

    return value