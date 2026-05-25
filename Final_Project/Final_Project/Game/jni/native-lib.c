#include <jni.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <android/log.h>

#define DEVICE_PATH "/dev/roullete_driver"
#define DEVICE_MAJOR_NUMBER 242

#define SET_HP _IOW(DEVICE_MAJOR_NUMBER, 0, int*)
#define SET_BULLET _IO(DEVICE_MAJOR_NUMBER,1)
#define SHOT _IOW(DEVICE_MAJOR_NUMBER,2,struct shot*)
#define MAGNIFIER _IO(DEVICE_MAJOR_NUMBER,3)
#define RECOVERY _IO(DEVICE_MAJOR_NUMBER,4)
#define HANDCUFFS _IO(DEVICE_MAJOR_NUMBER,5)
#define SET _IO(DEVICE_MAJOR_NUMBER,6)
#define GET_END_FLAG _IOR(DEVICE_MAJOR_NUMBER,7,int*)

struct shot {
    int to;
    int damage;
};

JNIEXPORT void JNICALL
Java_com_example_game_MainActivity_callSetCommand(JNIEnv* env, jobject obj) { //LCD,FND,DOT의 기본 값들 출력해주는 함수 start누를 시 기본 값들 출력함
    int fd, ret;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    ret = ioctl(fd, SET); //기본 세팅
    if (ret < 0) {
        perror("Failed to invoke ioctl SET");
    }

    close(fd);
}


JNIEXPORT void JNICALL
Java_com_example_game_MainActivity2_useSelectedHp(JNIEnv* env, jobject obj, jint selectedHp) {// 선택한 Hp를 드라이버에 전달하는 함수
    int fd, ret;

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return;
    }

    ret = ioctl(fd, SET_HP, &selectedHp); //HP값을 전달한다.
    if (ret < 0) {
        perror("Failed to invoke ioctl");
        close(fd);
        return;
    }
    ret = ioctl(fd,SET_BULLET); //공포탄과 실탄을 랜덤으로 장전하기 위한 호출
    if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "JNI", "ioctl SET_BULLET 실패: %d", selectedHp);
		close(fd);
		return;
    }
    close(fd);
}

JNIEXPORT void JNICALL
Java_com_example_game_MainActivity3_sendCommandToNative(JNIEnv* env, jobject obj,jstring player, jint damage) {//드라이버에 명령을 전달하는 함수
	const char* playerStr = (*env)->GetStringUTFChars(env, player, NULL);

    int fd,turn;
    struct shot shot_info;
    fd = open("/dev/roullete_driver", O_RDWR);
    if (fd < 0) {
        // 예외 처리: 장치 열기 실패
        return;
    }

    // ioctl을 사용하여 명령 전달
    if (strcmp(playerStr, "Me") == 0) {
		shot_info.to = 0; // me
		shot_info.damage = damage;
		__android_log_print(ANDROID_LOG_ERROR, "JNI", "ready to shot\n");
		if (ioctl(fd, SHOT, &shot_info) < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "JNI", "SHOT ioctl 실패");
		}
	} else if (strcmp(playerStr, "Enemy") == 0) {
		shot_info.to = 1; // enemy
		shot_info.damage = damage;
		__android_log_print(ANDROID_LOG_ERROR, "JNI", "ready to shot\n");
		if (ioctl(fd, SHOT, &shot_info) < 0) {
			__android_log_print(ANDROID_LOG_ERROR, "JNI", "SHOT ioctl 실패");
		}
    } else if (strcmp(playerStr, "Recovery") == 0) {
        ioctl(fd, RECOVERY);
    } else if (strcmp(playerStr, "Magnifier") == 0) {
        ioctl(fd, MAGNIFIER);
    } else if (strcmp(playerStr, "Handcuffs") == 0) {
        ioctl(fd, HANDCUFFS);
    }

    close(fd);
    (*env)->ReleaseStringUTFChars(env, player, playerStr);
}


JNIEXPORT jint JNICALL
Java_com_example_game_MainActivity3_getEndFlag(JNIEnv* env, jobject obj) {//드라이버에서 end_flag받아오는 함수 endFlag에 저장하여 return한다.
    int fd, endFlag;
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return -1;
    }

    if (ioctl(fd, GET_END_FLAG, &endFlag) < 0) {
        perror("Failed to invoke ioctl GET_END_FLAG");
        close(fd);
        return -1;
    }

    close(fd);
    return endFlag;
}
