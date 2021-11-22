@ECHO OFF
REM @brief sils_define�̃R�����g�A�E�g��L��������

REM �X�N���v�g���s���Asils_define.h����
REM ```
REM #define SILS_FW //������define����I
REM ```
REM �Ə����ꂽ�s�����o���A�R�����g�A�E�g����B
REM �ȉ��̃T�C�g���Q�l�ɂ����B
REM   ������u��: https://qiita.com/wagase/items/2180d8911dcc2a748fd3
REM   ��s����  : https://blogs.yahoo.co.jp/kerupani/15344574.html

cd /d %~dp0
setlocal enabledelayedexpansion

for /f "tokens=1* delims=:" %%A in ('findstr /n "^" ..\Settings\sils_define.h') do (
  set line0=%%B

  REM ��̍s��echo�ŕ\���ł��Ȃ��̂ŕ��򂵂Ă���
  if "%%B" == "" (
    echo.>>..\Settings\SILS_DEFINE.tmp
  ) else (
    set line0=!line0:#define SILS_FW //������define����I=//#define SILS_FW //������define����I!
    set line0=!line0:////=//!
    echo !line0!>>..\Settings\SILS_DEFINE.tmp
  )
)

REM �ꎞ�t�@�C�����폜���A�ύX�𔽉f
rem move SILS_DEFINE.tmp sils_define.h
move ..\Settings\SILS_DEFINE.tmp ..\Settings\sils_define.h
