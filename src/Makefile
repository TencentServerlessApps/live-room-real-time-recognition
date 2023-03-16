BUILD_DIR = ./build

commit = $(shell git rev-parse --short HEAD)
date = $(shell date +'%Y%m%d-%H:%M:%S')
default_version = $(commit)-$(date)

NAME = live-room-real-time-recognition
VERSION = $(release_version)
ifeq (${VERSION},)
VERSION = ${default_version}
endif

build-base-image:
	docker build -t ${NAME}-base:v1.0.0  ./speech_engine/base-image

build-version: build-base-image
	docker build -t ${NAME}:${VERSION}  .

start: build-version
	docker run -it --rm -d ${NAME}:${VERSION}

CONTAINER_ID = $(shell docker ps|grep ${NAME}:${VERSION}|awk '{print $$1}'|head -n 1)

package: start
	rm -rf ${BUILD_DIR}
	mkdir ${BUILD_DIR}
	docker cp ${CONTAINER_ID}:/usr/local/app/asr/live-room-real-time-recognition.zip ${BUILD_DIR}/
	mv ${BUILD_DIR}/live-room-real-time-recognition.zip ${BUILD_DIR}/live-room-real-time-recognition-${VERSION}.zip
	docker ps|grep ${NAME}:${VERSION}|awk '{print $$1}' | xargs docker kill

clean:
	rm -rf ${BUILD_DIR}
	docker images|grep ${NAME}|grep ${VERSION}|awk '{print $$3}' |xargs docker rmi -f



