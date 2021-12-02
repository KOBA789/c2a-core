#pragma section REPRO
#include "time_manager.h"
#include <string.h>
#include <src_user/CmdTlm/Ccsds/TCPacket.h>
#include "../TaskManager/task_dispatcher.h"
#include "../../Library/endian_memcpy.h"

static ObcTime master_clock_;

static OBCT_UnixTimeInfo OBCT_unix_time_info_;

static TimeManager time_manager_;
const TimeManager* const time_manager = &time_manager_;

static void TMGR_set_master_total_cycle_(cycle_t total_cycle);

void TMGR_init(void)
{
  OBCT_clear(&time_manager_.initializing_time);
  time_manager_.initializing_flag = 1;
  TMGR_clear();
}

void TMGR_clear(void)
{
  OBCT_clear(&master_clock_);
  OBCT_clear_unix_time_info(&OBCT_unix_time_info_);
}

void TMGR_down_initializing_flag(void)
{
  memcpy(&time_manager_.initializing_time, &master_clock_, sizeof(ObcTime));
  time_manager_.initializing_flag = 0;

  TMGR_clear();
}

void TMGR_clear_master_mode_cycle(void)
{
  master_clock_.mode_cycle = 0;
}

#pragma section _FIX_TMGR
// Pセクションしか割り当てていないので，
// D, Bセクションに乗るような変数定義は禁止！！！
void TMGR_count_up_master_clock(void)
{
  OBCT_count_up(&master_clock_);
}
#pragma section
#pragma section REPRO

uint32_t TMGR_get_master_total_cycle_in_msec(void)
{
  return OBCT_get_total_cycle_in_msec(&master_clock_);
}

uint32_t TMGR_get_master_mode_cycle_in_msec(void)
{
  return OBCT_get_mode_cycle_in_msec(&master_clock_);
}

ObcTime TMGR_get_master_clock(void)
{
  if (time_manager_.initializing_flag)
  {
    return OBCT_create(0, 0, 0);
  }
  else
  {
    return master_clock_;
  }
}

cycle_t TMGR_get_master_total_cycle(void) {
  return OBCT_get_total_cycle(&master_clock_);
}

cycle_t TMGR_get_master_mode_cycle(void) {
  return OBCT_get_mode_cycle(&master_clock_);
}

step_t  TMGR_get_master_step(void) {
  return OBCT_get_step(&master_clock_);
}


CCP_EXEC_STS Cmd_TMGR_SET_TIME(const CTCP* packet)
{
  cycle_t set_value = 0;

  endian_memcpy(&set_value, CCP_get_param_head(packet), 4);

  if (set_value < OBCT_MAX_CYCLE)
  {
    TMGR_set_master_total_cycle_(set_value);
    TDSP_resync_internal_counter();
    return CCP_EXEC_SUCCESS;
  }
  else
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }
}

static void TMGR_set_master_total_cycle_(cycle_t total_cycle)
{
  master_clock_.total_cycle = total_cycle;
}

double TMGR_get_unix_time_from_ObcTime(const ObcTime* time)
{
  ObcTime ti0 = OBCT_create(0, 0, 0);
  return OBCT_unix_time_info_.unix_time_on_ti0 + OBCT_diff_in_sec(&ti0, time);
}

ObcTime TMGR_get_ObcTime_from_unix_time(const double unix_time)
{
  double diff_double = unix_time - OBCT_unix_time_info_.unix_time_on_ti0;
  ObcTime res;
  uint32_t diff;
  cycle_t cycle_diff;
  step_t step_diff;

  if (diff_double < 0)  // あり得ない, おかしい
  {
    return res = OBCT_create(0, 0, 0);
  }

  diff = (uint32_t)(diff_double * 1000.0 + 1e-4); // msオーダーだがそんなに大きくないことを想定, 1e-4は数値誤差対策（.999がj切り捨てられるのを防ぐ）
  cycle_diff = diff / (OBCT_STEP_IN_MSEC * OBCT_STEPS_PER_CYCLE);
  step_diff = (diff - cycle_diff * (OBCT_STEP_IN_MSEC * OBCT_STEPS_PER_CYCLE)) / OBCT_STEP_IN_MSEC;

  res.total_cycle = cycle_diff;
  res.mode_cycle = 0; // 取得出来ないので0とする
  res.step = step_diff;

  return res;
}

void TMGR_modify_unix_time_criteria(const double unix_time, const ObcTime time)
{
  OBCT_modify_unix_time_info(&OBCT_unix_time_info_, unix_time, time);
}

OBCT_UnixTimeInfo TMGR_get_obct_unix_time_info(void)
{
  return OBCT_unix_time_info_;
}

CCP_EXEC_STS Cmd_TMGR_SET_UNIXTIME(const CTCP* packet)
{
  const unsigned char* param = CCP_get_param_head(packet);
  double unix_time;
  ObcTime time;

  endian_memcpy(&unix_time, param, 8);
  endian_memcpy(&time.total_cycle, param + 8, 4);
  endian_memcpy(&time.mode_cycle, param + 12, 4);
  endian_memcpy(&time.step, param + 16, 4);

  TMGR_modify_unix_time_criteria(unix_time, time);

  return CCP_EXEC_SUCCESS;
}

ObcTime TMGR_get_master_clock_from_boot(void)
{
  return OBCT_add(&time_manager_.initializing_time, &master_clock_);
}

#pragma section
