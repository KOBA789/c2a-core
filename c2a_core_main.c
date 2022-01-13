#pragma section REPRO
#include "c2a_core_main.h"

#include "./Library/print.h"
#include "./System/TaskManager/task_dispatcher.h"
#include "./System/ApplicationManager/app_manager.h"
#include "./System/EventManager/event_manager.h"
#include "./System/AnomalyLogger/anomaly_logger.h"
#include "./System/TimeManager/time_manager.h"
#include "./System/ModeManager/mode_manager.h"
#include "./System/WatchdogTimer/watchdog_timer.h"
#include "./TlmCmd/packet_handler.h"
#include "./TlmCmd/block_command_table.h"
#include "./TlmCmd/command_analyze.h"
#include "./TlmCmd/telemetry_frame.h"

#include <src_user/Applications/app_registry.h>

void C2A_core_init(void)
{
  CA_initialize();            // Cmd Analyze
  Printf("C2A_init: CA_initialize done.\n");
  TF_initialize();            // TLM frame
  Printf("C2A_init: TF_initialize done.\n");
  PH_init();                  // Packet Handler
  Printf("C2A_init: PH_init done.\n");
  EM_initialize();            // Event Manager�DApp Manager����ɏ���������ׂ�
  Printf("C2A_init: EM_initialize done.\n");
  AL_initialize();            // Anomaly Logger�DApp Manager����ɏ���������ׂ�
  Printf("C2A_init: AL_initialize done.\n");
  AM_initialize();            // App Manager
  Printf("C2A_init: AM_initialize done.\n");
  AR_load_initial_settings(); // App Registry
  Printf("C2A_init: AR_load_initial_settings done.\n");
  AM_initialize_all_apps();   // App Manager�ɓo�^����Ă�S�A�v���̏�����
  Printf("C2A_init: AM_initialize_all_apps done.\n");
  BCT_initialize();           // Block Cmd Table
                              // BC_load_defaults() �������ŌĂ΂��
  Printf("C2A_init: BCT_initialize done.\n");
  MM_initialize();            // Mode Manager
                              // ������START UP to INITIAL�̃��[�h�J�ڂ��s����
  Printf("C2A_init: MM_initialize done.\n");
  TDSP_initialize();          // Task Dispatcher
                              // MM�������������Ƃɍs����K�v������
  Printf("C2A_init: TDSP_initialize done.\n");
  // DebugOutInit();             // Debug��init          // LVTTL UART ch1�ł̏o�́D�ז��Ȃ̂�Printf�̒��g�ƂƂ��ɖ����� (2019-04-09)
  // Printf("C2A_init: DebugOutInit done.\n");

  TMGR_down_initializing_flag();
}

void C2A_core_main(void)
{
  // �����ł���task dispatcher�́CTL0�Ƃ��ł͂Ȃ��C
  // task list��BlockCommand��dispatch���Ă���D
  // TL0�Ȃǂ�PL��dispatch���Ă���̂́Ctlc_dispatcher @ App/timeline_command_dispatcher �ł���D
  // �Ȃ��CPL (packetList) ��PL_info�Ƃ́C����dispatch���Ă����p�P�b�g�����n��ɂȂ�ׂ�linked list�ł���D
  TDSP_execute_pl_as_task_list();
  WDT_clear_wdt();
}

#pragma section
