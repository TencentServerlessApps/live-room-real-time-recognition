FROM live-room-real-time-recognition-base:v1.0.0 as builder

RUN yum clean all

RUN mkdir -p /usr/local/app/asr

WORKDIR /usr/local/app/asr/speech_engine
COPY speech_engine .

# 编译可执行文件
RUN make clean \
    && make

FROM centos:7.7.1908

# 安装依赖工具
RUN yum -y install \
        python3 \
        zip \
    && yum clean all \
    && ln -s /lib64/libz.so.1.2.7 /lib64/libz.so

# 创建工作目录
RUN rm -rf /usr/local/app/asr \
    && mkdir -p /usr/local/app/asr

WORKDIR /usr/local/app/asr
COPY src .
COPY speech_engine/lib ./lib
COPY .env.example .
COPY serverless.yml .
COPY --from=builder /usr/local/app/asr/speech_engine/bin/AsrSpeechEngine .

# 安装python依赖包
RUN pip3 install Flask \
    pymysql \
    requests
ENV PYTHONPATH /usr/local/lib/python3.6:/usr/lib/python3.6:$PYTHONPATH

WORKDIR /usr/local/app/asr

RUN zip -ry live-room-real-time-recognition.zip .

# 执行入口
ENTRYPOINT [ "/bin/bash" ]