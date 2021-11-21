#ifndef TIME_MANAGER_H_
#define TIME_MANAGER_H_

#include "obc_time.h"
#include "../../CmdTlm/common_tlm_cmd_packet.h"

// 2020-05-26
// master_clock �̌��J�𒆎~�����D
// ����́C
//   + ObcTime TMGR_get_master_clock();
//   + cycle_t TMGR_get_master_cycle();
// �Ȃǂ�p���ăA�N�Z�X�iread only�j����
// extern const ObcTime* master_clock;

void TMGR_init(void);
void TMGR_clear_master_mode_cycle(void);
void TMGR_count_up_master_clock(void);
uint32_t TMGR_get_master_total_cycle_in_msec(void);  // �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
uint32_t TMGR_get_master_mode_cycle_in_msec(void);   // �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
ObcTime TMGR_get_master_clock(void);
cycle_t TMGR_get_master_total_cycle(void);
cycle_t TMGR_get_master_mode_cycle(void);
step_t  TMGR_get_master_step(void);
double TMGR_get_unix_time_from_ObcTime(const ObcTime* time);
ObcTime TMGR_get_ObcTime_from_unix_time(const double unix_time);
void TMGR_modify_unix_time_criteria(const double unix_time, const ObcTime time);
OBCT_UnixTimeInfo TMGR_get_obct_unix_time_info(void);

CCP_EXEC_STS Cmd_TMGR_SET_TIME(const CTCP* packet);
CCP_EXEC_STS Cmd_TMGR_SET_UNIXTIME(const CTCP* packet);

#endif // TIME_MANAGER_H_
