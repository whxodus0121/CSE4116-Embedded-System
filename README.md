CSE4116-Embedded-System

이 저장소는 임베디드 시스템 과목에서 수행한 과제 및 최종 프로젝트를 포함하고 있습니다. 각 과제는 디바이스 드라이버, 타이머, 인터럽트, 그리고 Android와 연동하는 JNI 프로그래밍 등을 다룹니다.

HW1: KVS(Key-Value Store) 구현

개발 목표

디바이스를 이용하여 KVS 자료를 입력, 저장 및 검색하는 기능을 구현

개발 범위 및 내용

개발 범위: 현재 상태 및 입력 중인 값을 디바이스를 통해 표현해야 함

개발 내용:

/dev 아래 디바이스 파일을 활용하여 open(), mmap() 사용

fork()를 이용한 자식 프로세스 생성

프로세스 간 통신을 위한 IPC 구현

HW2: 디바이스 드라이버 및 타이머 기능 포함 모듈 구현

개발 목표

디바이스 드라이버와 타이머 기능을 포함한 하나의 모듈을 구현

개발 범위 및 내용

개발 범위:

디바이스 드라이버 및 타이머 모듈 구현

이를 활용하는 응용 프로그램 작성

개발 내용:

timer_driver.c: 디바이스 등록/해제, open(), release(), ioctl(), read() 구현

device.c: FPGA 디바이스(Dot, FND, LED, LCD) 제어

app.c: 사용자 입력을 받아 드라이버와 ioctl()로 통신 및 read()로 RESET 스위치 값 읽기

HW3: 디바이스 드라이버와 인터럽트를 활용한 스톱워치 구현

개발 목표

Module programming, 디바이스 드라이버, 인터럽트를 활용하여 간단한 스톱워치 프로그램을 작성

개발 범위 및 내용

개발 범위:

디바이스 드라이버와 인터럽트 기능을 활용한 스톱워치 구현

응용 프로그램 작성

개발 내용:

timer_driver.c: 디바이스 등록/해제, 스톱워치 기능 구현

workqueue를 활용한 top half 및 bottom half 인터럽트 처리

device.c: bottom half에서 호출되는 디바이스 출력 함수 구현

Final Project: Android + JNI + Device Driver 활용한 BuckShot Roullete 게임

개발 목표

Android 프로그래밍, JNI, 디바이스 드라이버를 활용하여 BuckShot Roullete 게임을 구현

개발 범위 및 내용

개발 범위:

Android 앱과 연동되는 디바이스 드라이버 개발

게임 로직이 포함된 Android 애플리케이션 개발

JNI 파일을 통한 Android와 드라이버 간 통신

개발 내용:

Roullete_driver: LCD, FND, DOT, MOTOR 디바이스를 이용하여 게임 상황을 표현하는 드라이버

ioctl()을 통해 JNI와 데이터 교환, 기본 설정, HP 변경, 아이템 사용 구현

Android 프로그램 (BuckShot Roullete):

2인용 러시안 룰렛 기반 게임

공포탄과 실탄이 랜덤하게 장전되고, 번갈아가며 쏠 대상 선택

Magnifier, Recovery, Handcuffs, Double Damage 등의 아이템 지원

JNI:

Android 앱에서 입력한 버튼에 따라 드라이버에 ioctl()을 이용해 데이터 전달
