# Vbox vdi > qemu qcow2

> Oracle Virtual box VDI 디스크 형식을 KVM 하이퍼 바이저로 migration 하는 법



## vboxmanage

command line 에서 virtual box 를 컨트롤 가능하다

파일 생성이니 root 권한으로 해주자

```bash
# VDI 디스크 이미지 나열 
:~$ vboxmanage list hdds
```



```bash
# VDI 이미지를 RAW 디스크 형식으로 변환
# copy paste 시 disk UUID 문제가 생기니 vbox의 clonehd 를 사용
# vboxmanage clonehd [source] [destination]

:~$ vboxmanage clonehd --format RAW /home/.../ubuntu.vdi ubuntu.img
```



virty-manager 실행 후 새 가상 머신 만들기에서 가져오기하면 끝



```bash
# system hw 정보 이쁘게 확인하는 법

# lshw 로 hw 정보를 확인할 때 옵션 중 html로 파싱해주는 기능이 있다
:~$ lshw -html

# html 파일로 redirection 해주면 된다
:~$ lshw -html > hw.html
```



```bash
# raw image disk 형식을 KVM 디스크 이미지 형식인 qcow2 로 migration 하기

:~$ qemu-img convert -f raw ubuntu.img -O qcow2 ubuntu.qcow2
```



```bash
# qemu-img 설치
:~$ sudo apt install qemu-utils

# disk image 생성 (5GB size) **-o 는 [options] [filename] 이며 -O 는 output를 뜻한다
:~$ qemu-img create -o size=5G disk.img

# 특정한 포맷으로 disk image 생성  ** default 는 raw, -f 는 format
:~$ qemu-img create -f qcow2 -o size=5G disk.img

# disk 정보 확인
:~$ qemu-img info disk.img

# disk image qcow2 포맷으로 변환(compression)
# 추가적으로 압축후 변환 시 -c 옵션으로 준다(qcow format only)

:~$ qemu-img convert -O qcow2 disk.img backup.img
:~$ ls -lh
total 196k
-rw-r--r-- 1 lsh lsh 193k 2월 4 18:04 backup.img
-rw-r--r-- 1 lsh lsh 5.0G 2월 4 17:50 disk.img

# image 에러 검사. raw 포맷에서는 지원하지 않는다
:~$ qemu-img check disk.img

# disk image resize. -f 옵션으로 이미지 파일의 올바른 포맷을 지정해줘야함
# disk 용량을 조절한 뒤 부팅 후 파티션 정보를 수정해야함.
:~$ qemu-img resize -f raw disk.img+1G
```

windows10 virtyio driver 설치법 찾아서 드라이버 세팅해주면 된다. 아래 인도인 링크 참조

https://linuxhint.com/install_virtio_drivers_kvm_qemu_windows_vm/
