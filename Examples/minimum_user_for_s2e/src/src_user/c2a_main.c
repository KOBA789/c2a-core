#include <src_core/Library/print.h>
#include <src_core/c2a_core_main.h>
#include <src_core/System/TimeManager/time_manager.h>
#include <src_core/System/WatchdogTimer/watchdog_timer.h>
#include "./Settings/sils_define.h"

void main(void);
static void address_fixed_main_(void);
static void C2A_init_(void);
static void C2A_main_(void);
static void timer_setting_(void);

void main(void)
{
  address_fixed_main_();
}


// RAM��ł̃A�h���X�Œ�main�֐�
#pragma section _FIX_MAIN
// P�Z�N�V�����������蓖�ĂĂ��Ȃ��̂ŁC
// D, B�Z�N�V�����ɏ��悤�ȕϐ���`�͋֎~�I�I�I
static void address_fixed_main_(void)
{
#ifndef SILS_FW
  // BOOT_LOADER_main();
#endif
  C2A_init_();
  C2A_main_();
}
#pragma section


#pragma section REPRO
static void C2A_main_(void)
{
  while (1)       // while��user����B�����������C���R�x�Ƃ̃g���[�h�I�t�D�D�D�H
  {
    C2A_core_main();

    // ���[�U�[��`loop�����͂����ɓ����
  }

  // �����ɗ��邱�Ƃ͂Ȃ��͂��Ȃ̂ŁC������WDT��Reset��������悤�ɂ���H
}
#pragma section


#pragma section REPRO
// C2A�֘A�̏�����
// HW�֘A�����i�^�C�}�[�C���荞�ݐݒ�Ȃǁj�̂ݓƎ��ɏ�����
// Printf������ WDT_clear_wdt(); ���Ă΂�Ă邱�Ƃɒ��ӁI
static void C2A_init_(void)
{
  WDT_init();
  TMGR_init();                // Time Manager
                              // AM_initialize_all_apps �ł̎��Ԍv���̂��߂ɂ����ŏ�����
  Printf("C2A_init: TMGR_init done.\n");
  timer_setting_();           // Timer ���荞�݊J�n
  Printf("C2A_init: timer_setting_ done.\n");

  C2A_core_init();

  // TaskDispatcher�ł̑�ʂ̃A�m�}��������邽�߂ɁA��x����������������B
  TMGR_clear();
  Printf("C2A_init: TMGR_init done.\n");
}
#pragma section


static void timer_setting_(void)
{
}
