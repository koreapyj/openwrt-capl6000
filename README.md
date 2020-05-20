# 한빛전자 LG U+향 CAPL-6000용 OpenWrt v19.07.2
* 저도 잘 모르고 만들었기 때문에 무언가 문제가 있을 것 같습니다.
* MR 환영합니다.

## 작동이 되지 않는 기능
- WiFi
- 그 외 발견하지 못한 기능이 더 있을 수 있음

## 되긴 되는데 뭔가 마음에 안 드는 기능
- ip link up이 느림. 부팅 개시 후 60초 이상 걸리는 것 같음.

## 빌드 방법
* 표준적인 OpenWrt의 빌드 방법을 따르면 됩니다. 제품 이름은 `Hanbit Electronics CAPL-6000`으로 `ar71xx`에 등록되어 있습니다.

## 설치 방법
* 안정된 버전이 아니며 순정 펌웨어로의 부팅이 되지 않을 수 있습니다. 배포자는 이에 대해 유무형을 막론하고 어떠한 책임도 지지 않으므로 본인 부담으로 진행하시기 바랍니다.

1. 관리 페이지에 로그인합니다.
2. 시스템 설정으로 이동한 뒤 F12를 눌러 개발자 도구를 열고 `<body>` 태그 바로 하위에 있는 `<form>`태그를  눌러 다음과 같이 변경합니다.
    ```html
    <script>
      function FirmStat(pForm) {
        var firmware_name;
        var index_1, index_2;

        var upgradeMsg = "ㄹㅇ?";

        firmware_name = pForm.file.value;

        if (((index_2 = firmware_name.lastIndexOf(".")) != -1) &&
          (index_2 > parseInt(index_1 + 1)) &&
          (firmware_name.substring(index_2) == ".bin")) {

          // 경고 문구 입력  
          yesorno = confirm(upgradeMsg);

          if (yesorno == true) {
            document.body.style.cursor = 'wait';
            document.forms[0].submit();
          }
        } else {
          alert("upload 파일이 아닙니다. 파일을 다시 선택해 주세요");
          return;
        }
      }
    </script>
    <form action="upgrade.cgi" method="POST" ENCTYPE="multipart/form-data">
      <div align=center>
        <table border="0" cellpadding="0" cellspacing="1" width="620" bgcolor="#CCCCCC">
          <tr>
            <td width="620" bgcolor="white">
              <table border="0" cellpadding="2" cellspacing="1" width="620">
                <tr>
                  <td class=contents width=165 bgcolor=f0f0f0 align=center>소프트웨어 업그레이드</td>
                  <td bgcolor=f8f8f8><input type="file" name="file" style="background-color:white; value=" image.bin"
                      size=30>
                    <input class=btn type="button" name="upgradebtn" value="업그레이드 시작" style="width:100"
                      Onclick="FirmStat(this.form);">
                  </td>
                </tr>
              </table>
            </td>
          </tr>
        </table>
      </div>
    </form>
    ```
3. `openwrt-ar71xx-generic-capl_6000-initramfs-kernel.bin`를 선택하여 펌웨어 업데이트를 진행합니다. 업데이트가 완료되면 인터넷 연결이 끊기게 되며 약 2~3분 뒤에 다시 인터넷 연결이 가능해집니다.
4. 인터넷에 연결되면 PuTTY등 ssh 클라이언트를 사용하여 `192.168.1.1`로 접속합니다. 계정은 `root`에 패스워드는 없습니다.
5. 정상적으로 접속되면 셸에서 아래 내용을 입력하여 실행시킵니다.
    ```sh
    wget http://api.dcmys.kr/capl6000/openwrt-ar71xx-generic-capl_6000-squashfs-sysupgrade.bin -O - | mtd -e firmware write - firmware
    ```
6. 완료되면 `reboot`를 이용하여 재부팅합니다.
7. 재부팅이 완료되고 인터넷에 연결되면 다시 `192.168.1.1`로 ssh 접속합니다.
8. passwd를 입력하여 root 패스워드를 설정합니다.
9. **이 배포판에는 luci 등 기본적인 패키지가 포함되어 있지 않습니다. 필요한 패키지가 있는 경우 opkg를 이용하여 설치하시기 바랍니다.**

## 벽돌이 된 경우
* 기본적으로 이 제품은 두 개의 파티션으로 구성되어 있습니다. 순정 펌웨어 기준으로 1번 파티션(@0x9f050000/0x770000)은 업데이트를 위한 영역이며 2번 파티션(@0x9f7c0000/0x770000)은 현용 펌웨어를 위한 영역입니다.
* 둘 중 어느 한 쪽에 문제가 생겨(magic number+crc32 mismatch) 부팅을 진행할 수 없게 되면 다른 쪽 파티션으로 부팅을 시도합니다.
* 그러나 두 파티션 모두 문제가 생기는 경우 완전한 부트 불량에 빠지게 되며 UART 연결 없이 복구할 방법은 없습니다.
* 이러한 일이 일어난 경우 USB to UART 장치를 구입하여 부트로더 셸을 통해 tftp로 initramfs를 실행하여 복구하는 것이 가능합니다.
* 기기의 UART 포트에 대해서는 [이곳](https://koreapyj.dcmys.kr/125)을 참고하시기 바랍니다.
