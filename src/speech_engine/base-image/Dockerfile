FROM centos:7.7.1908

# 安装依赖工具
RUN yum -y install \
        perl \
        gcc \
        gcc-c++ \
        glibc-static \
        libstdc++-static \
        python3 \
        make \
        cmake \
        centos-release-scl \
    && yum clean all \
    && ln -s /lib64/libz.so.1.2.7 /lib64/libz.so

RUN yum -y install devtoolset-8-gcc*
ENV PATH /opt/rh/devtoolset-8/root/usr/bin:$PATH

# 创建工作目录
RUN mkdir -p /usr/local/app/asr \
    /tmp

WORKDIR /usr/local/app/asr
COPY tool-libs .

# 安装openssl
WORKDIR /usr/local/app/asr/tools
RUN tar -xvzf openssl-1.1.1g.tar.gz \
    && cd openssl-1.1.1g \
    && ./config shared -fPIC --prefix=/usr/local/openssl --openssldir=/usr/local/openssl \
    && make depend \
    && make -j 2 \
    && make install \
    && cd .. \
    && rm -rf openssl-1.1.1g.tar.gz \
       openssl-1.1.1g

# 创建openssl软链接
RUN rm -rf /usr/lib64/libssl.so.10 \
    /usr/lib64/libcrypto.so.10 \
    && ln -sf /usr/local/openssl/bin/openssl /usr/bin/openssl \
    && ln -sf /usr/local/openssl/include/openssl /usr/include/openssl \
    && ln -sf /usr/local/openssl/lib/libssl.so.1.1 /usr/lib64/libssl.so \
    && ln -sf /usr/local/openssl/lib/libssl.so.1.1 /usr/lib64/libssl.so.1.1 \
    && ln -sf /usr/local/openssl/lib/libcrypto.so.1.1 /usr/lib64/libcrypto.so \
    && ln -sf /usr/local/openssl/lib/libcrypto.so.1.1 /usr/lib64/libcrypto.so.1.1 \
    && echo "/usr/local/openssl/lib" >> /etc/ld.so.conf.d/openssl-renwole.com.conf \
    && ldconfig

# 安装cmake
RUN tar -xvzf cmake-3.9.2.tar.gz \
    && cd cmake-3.9.2 \
    && ./configure \
    && make \
    && make install \
    && hash -r \
    && cd .. \
    && rm -rf cmake-3.9.2.tar.gz \
       cmake-3.9.2

WORKDIR /usr/local/app/asr
# 清理文件
RUN rm -rf tools