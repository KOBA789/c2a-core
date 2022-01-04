#ifndef TIME_MANAGER_H_
#define TIME_MANAGER_H_

#include "obc_time.h"
#include "../../CmdTlm/common_tlm_cmd_packet.h"

typedef struct
{
  ObcTime master_clock_;
  OBCT_UnixtimeInfo unixtime_info_;
  struct
  {
    ObcTime initializing_time;
    uint8_t initializing_flag;
  } init_info_;
} TimeManager;

extern const TimeManager* const time_manager;

/** @brief TimeManager �\���̂����������� */
void TMGR_init(void);

/** @brief master_clock_ �� unixtime_info_ ���N���A���S�ă[���ɂ��� */
void TMGR_clear(void);

/** @brief master_clock_ �� mode_cycle ���[���ɂ��� */
void TMGR_clear_master_mode_cycle(void);

/** @brief master_clock_ ��step���݂ŃC���N�������g���� */
void TMGR_count_up_master_clock(void);

/** @brief C2A�̏������I�����ɌĂяo��, ������flag������, TMGR��������x�N���A���� */
void TMGR_down_initializing_flag(void);

/**
 * @brief ���݂� master_clock_ ���擾����
 * @retval {0, 0, 0} : C2A���������̏ꍇ
 * @retval master_clock_ : ����ȊO
 */
ObcTime TMGR_get_master_clock(void);

/**
 * @brief �������ɂ����������Ԃ����Z���� master_clock_ ��Ԃ�
 * @return master_clock_ + initializing_time
 */
ObcTime TMGR_get_master_clock_from_boot(void);

/** @brief ���݂� total_cycle ��Ԃ� */
cycle_t TMGR_get_master_total_cycle(void);

/** @brief ���݂� mode_cycle ��Ԃ� */
cycle_t TMGR_get_master_mode_cycle(void);

/** @brief ���݂� step ��Ԃ� */
step_t  TMGR_get_master_step(void);

/**
 * @brief ���݂� total_cycle ���~���b�P�ʂŕԂ�
 * @note �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
 * @return �~���b�P�ʂ� total_cycle
 */
uint32_t TMGR_get_master_total_cycle_in_msec(void);

/**
 * @brief ���݂� total_cycle ���~���b�P�ʂŕԂ�
 * @note �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
 * @return �~���b�P�ʂ� total_cycle
 */
uint32_t TMGR_get_master_mode_cycle_in_msec(void);

/** @brief unixtime_info ���擾���� */
OBCT_UnixtimeInfo TMGR_get_obct_unixtime_info(void);

/**
 * @brief ObcTime �� unixtime �ɕϊ�����
 * @param[in] ObcTime
 * @return unixtime (�b�P��, �����_�ȉ����ێ�)
 */
double TMGR_get_unixtime_from_obc_time(const ObcTime* time);

/**
 * @brief unixtime �� ObcTime �ɕϊ�����
 * @param[in] unixtime
 * @return ObcTime
 */
ObcTime TMGR_get_obc_time_from_unixtime(const double unixtime);

/**
 * @brief ��ʓI��unixtime��, UTL_cmd�ŗp���� utl_unixtime �ɕϊ�����
 * @param[in] unixtime �ϊ������� unixtime
 * @return utl_unixtime (2020-1-1-00:00:00UTC �N�Z, 0.1�b���݂Ȃ̂ŒP�ʂ�cycle_t)
 */
cycle_t TMGR_get_utl_unixtime_from_unixtime(const double unixtime);

/**
 * @brief �����Ŏw�肳�ꂽ utl_unixtime �ɑΉ����� ti ��Ԃ�
 * @note UTL_cmd �Ŏ��s�������� ti �ɕϊ�����ۂɗp����
 * @param[in] utl_unixtime
 * @return ti (total_cycle�̂���)
 */
cycle_t TMGR_get_ti_from_utl_unixtime(const cycle_t utl_unixtime);

/**
 * @brief unixtime_info_ ���ϑ�����p���čX�V����
 * @param[in] unixtime (GPS ������ϑ�����) unixtime
 * @param[in] time (GPS ������) unixtime ���ϑ��������� ObcTime
 */
void TMGR_update_unixtime_info(const double unixtime, const ObcTime time);

CCP_EXEC_STS Cmd_TMGR_SET_TIME(const CTCP* packet);
CCP_EXEC_STS Cmd_TMGR_SET_UNIXTIME(const CTCP* packet);

#endif
