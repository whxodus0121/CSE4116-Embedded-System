#CSE4116-Embedded-System

본 저장소는 CSE4116 임베디드 시스템 과목에서 수행한 과제들을 포함하고 있습니다.

📌 과제 목록

📝 HW1 - KVS(Key-Value Store) 구현

1. 개발 목표

디바이스를 이용하여 KVS 자료를 입력, 저장, 검색하는 기능을 구현

2. 개발 범위 및 내용

개발 범위

현재 시스템 상태 및 입력값을 디바이스를 통해 시각적으로 표현

개발 내용

/dev 디렉토리 내 디바이스 파일을 open() 및 mmap()을 이용하여 제어

fork()를 사용하여 자식 프로세스 생성 및 IPC를 통한 프로세스 간 통신 구현

각 프로세스별 역할을 분배하여 동작하도록 설계

📝 HW2 - 디바이스 드라이버 및 타이머 모듈 구현

1. 개발 목표

디바이스 드라이버와 타이머 기능을 포함한 하나의 커널 모듈을 구현

2. 개발 범위 및 내용

개발 범위

디바이스 드라이버 및 타이머 기능이 포함된 커널 모듈 개발

해당 모듈을 활용하는 응용 프로그램 개발

개발 내용

timer_driver.c:

디바이스 등록 및 해제

open(), release(), init(), exit() 등의 기본 기능 구현

사용자 프로그램의 ioctl(), read() 요청을 처리

device.c:

FPGA 디바이스(DOT, FND, LED, LCD) 제어

app.c:

사용자 입력값을 ioctl()로 드라이버에 전달

read()를 통해 리셋 스위치 값을 읽고, 스위치가 눌리면 타이머 작동

📝 HW3 - 스톱워치 구현

1. 개발 목표

디바이스 드라이버, 인터럽트 기능을 활용하여 간단한 스톱워치 프로그램 작성

2. 개발 범위 및 내용

개발 범위

디바이스 드라이버와 인터럽트 기능을 활용한 스톱워치 구현

스톱워치를 사용하는 응용 프로그램 개발

개발 내용

timer_driver.c:

디바이스 등록 및 해제

스톱워치 기능 구현

workqueue를 이용하여 top-half(인터럽트 발생 시 처리)와 bottom-half(실제 동작 수행)로 나눠서 구현

device.c:

bottom-half 루틴에서 불리는 디바이스 출력 함수 구현

🎯 Final Project - BuckShot Roulette 게임 개발

1. 개발 목표

Android 프로그래밍, JNI, 디바이스 드라이버(Module Programming)를 포함한 BuckShot Roulette 게임 구현

2. 개발 범위 및 내용

개발 범위

Android 앱에서 사용할 디바이스 드라이버 개발

BuckShot Roulette 게임의 Android 애플리케이션 개발

JNI를 활용한 디바이스 드라이버 연동

개발 내용

Roulette_driver:

LCD, FND, DOT, MOTOR 디바이스를 활용하여 게임 상황을 시각적으로 표현

ioctl()을 통해 JNI와 데이터를 주고받으며 기본 설정, HP 변경, 아이템 사용 기능 구현

Android 프로그램 (BuckShot Roulette):

러시안 룰렛을 응용한 2인용 게임

랜덤 개수의 실탄과 공포탄이 장전되며, 번갈아가며 목표를 선택해 발사

사용 가능한 아이템: Magnifier, Recovery, Handcuffs, Double Damage

게임 UI:

상단: Me, Enemy 버튼

하단: Magnifier, Handcuffs, Double Damage, Recovery 아이템 버튼

JNI:

Android 애플리케이션에서 드라이버와 데이터를 주고받기 위해 ioctl() 활용

⚙️ 실행 방법

각 과제 폴더에 포함된 소스 코드 및 README 파일을 참고하여 실행할 수 있습니다.
