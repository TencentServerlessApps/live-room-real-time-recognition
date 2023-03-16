# -*- coding: utf8 -*-

import requests
from requests.adapters import HTTPAdapter


# 成功响应
def response(request_id):
    resp = {
        'RequestID': request_id
    }
    return resp


# 错误响应
def err_resp(code, message, request_id):
    resp = {
        'ErrorCode': code,
        'ErrorMessage': message,
        'RequestID': request_id
    }
    return resp


# 回调逻辑。
def callback(url, data):
    if not url:
        print("callback url is empty, no need to callback.")
        return

    s = requests.Session()
    s.mount('http://', HTTPAdapter(max_retries=3))
    s.mount('https://', HTTPAdapter(max_retries=3))

    try:
        response = s.post(url, json=data)
        print("callback response:", response.text.encode('utf8'))
    except requests.exceptions.RequestException as e:
        print(e)

    print("callback[%s] finished." % (url,))
