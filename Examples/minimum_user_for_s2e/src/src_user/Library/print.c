/**
 * @file
 * @brief  HW�ˑ�Print
 */

// ������BootLoader�ɔz�u����̂ŁC���v���\�̈�̂��̂�include���Ȃ����ƁI�I

// print��HW�ˑ������������߁C�w�b�_�[��core�ɂ����C�I�[�o�[���C�h����
// �r���h�ΏۂɂđI������
#include <src_core/Library/print.h>

#include <stdio.h>
#include <stdarg.h>

#include "../Settings/sils_define.h"

#ifndef SILS_FW
#include <src_core/System/WatchdogTimer/watchdog_timer.h>


/*
 *  ���O���̃t�H�[�}�b�g�o�́i�f�o�b�O�o�́j
 */
char printf_buffer[512];

void Printf(char* format, ...)
{
  va_list argptr;

  WDT_clear_wdt();         // 2019/03/10 �ǉ�

  va_start(argptr, format);
  vsprintf(printf_buffer, format, argptr);

  tt_printf("%s", printf_buffer);
  // Printf_org("%s", printf_buffer);  // LVTTL UART ch1�ł̏o�́D�ז��Ȃ̂ŏ������ƂƂ��ɖ����� (2019-04-09)
  va_end(argptr);

  WDT_clear_wdt();         // 2019/03/10 �ǉ�
}

#else

void Printf(char* format, ...)
{
#ifdef SHOW_DEBUG_PRINT_ON_SILS
  char printf_buffer[512];   // �o�b�t�@�T�C�Y���ł��������񂪗���Ǝ��ʁi���C����͎��@���ł��������j
  va_list args;
  va_start(args, format);
  vsprintf(printf_buffer, format, args);

  printf("%s", printf_buffer);
  fflush(stdout);

  va_end(args);
#else
  // �Ȃɂ��\�����Ȃ�
#endif
}

#endif
