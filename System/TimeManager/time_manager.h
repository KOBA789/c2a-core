/** 
  * @file 
  * @brief OBC�̎������� TimeManager �\���̂ɕێ����J�E���g�A�b�v����. ���̑�, �������Z�ɕK�v�Ȋ֐�����������
  */

#ifndef TIME_MANAGER_H_
#define TIME_MANAGER_H_

#include "obc_time.h"
#include "../../CmdTlm/common_tlm_cmd_packet.h"

#define TMGR_DEFAULT_UNIXTIME_EPOCH_FOR_UTL (1577836800.0) /*!< 2020-01-01T00:00:00Z ���_��unixtime
                                                                ����������[���Ƃ��ċN�Z����0.1�b���݂̎�����utl_unixtime�ƒ�`��,
                                                                UTL_cmd�̎��s�������Ƃ��ėp���� */

typedef struct
{
  ObcTime master_clock_;
  OBCT_UnixtimeInfo unixtime_info_;
  double utl_unixtime_epoch_;
  struct
  {
    ObcTime initializing_time;
    uint8_t initializing_flag;
  } init_info_;
} TimeManager;

extern const TimeManager* const time_manager;

/**
 * @brief TimeManager �\���̂�����������
 * @param void
 * @return void
 */
void TMGR_init(void);

/**
 * @brief master_clock_ �� unixtime_info_ ���N���A���S�ă[���ɂ���
 * @param void
 * @return void
 */
void TMGR_clear(void);

/**
 * @brief master_clock_ �� mode_cycle ���[���ɂ���
 * @param void
 * @return void
 */
void TMGR_clear_master_mode_cycle(void);

/**
 * @brief master_clock_ ��step���݂ŃC���N�������g����
 * @param void
 * @return void
 */
void TMGR_count_up_master_clock(void);

/**
 * @brief C2A�̏������I�����ɌĂяo��, ������flag������, TMGR��������x�N���A����
 * @param void
 * @return void
 */
void TMGR_down_initializing_flag(void);

/**
 * @brief ���݂� master_clock_ ���擾����
 * @param void
 * @retval {0, 0, 0} : C2A���������̏ꍇ
 * @retval master_clock_ : ����ȊO
 */
ObcTime TMGR_get_master_clock(void);

/**
 * @brief �������ɂ����������Ԃ����Z���� master_clock_ ��Ԃ�
 * @param void
 * @return master_clock_ + initializing_time
 */
ObcTime TMGR_get_master_clock_from_boot(void);

/**
 * @brief ���݂� total_cycle ��Ԃ�
 * @param void
 * @return master_clock_ �� total_cycle
 */
cycle_t TMGR_get_master_total_cycle(void);

/**
 * @brief ���݂� mode_cycle ��Ԃ�
 * @param void
 * @return master_clock_ �� mode_cycle
 */
cycle_t TMGR_get_master_mode_cycle(void);

/**
 * @brief ���݂� step ��Ԃ�
 * @param void
 * @return master_clock_ �� step
 */
step_t  TMGR_get_master_step(void);

/**
 * @brief ���݂� total_cycle ���~���b�P�ʂŕԂ�
 * @note uint32_t ���I�[�o�[�t���[����\��������̂Œ���
 * @note �v�Z���step���l��
 * @param void
 * @return �~���b�P�ʂ� total_cycle
 */
uint32_t TMGR_get_master_total_cycle_in_msec(void);

/**
 * @brief ���݂� total_cycle ���~���b�P�ʂŕԂ�
 * @note uint32_t ���I�[�o�[�t���[����\��������̂Œ���
 * @note �v�Z���step���l��
 * @return �~���b�P�ʂ� total_cycle
 */
uint32_t TMGR_get_master_mode_cycle_in_msec(void);

/**
 * @brief unixtime_info_ ���擾����
 * @param void
 * @return unixtime_info_
 */
OBCT_UnixtimeInfo TMGR_get_obct_unixtime_info(void);

/**
 * @brief ObcTime �� unixtime �ɕϊ�����
 * @param[in] ObcTime
 * @return unixtime (�b�P��, �����_�ȉ����ێ�)
 */
double TMGR_get_unixtime_from_obc_time(const ObcTime* time);

/**
 * @brief unixtime �� ObcTime �ɕϊ�����
 * @note ������ unixtime �� unixtime_at_ti0 ��菬�����ꍇ�� {0, 0, 0} ��Ԃ�
 * @param[in] unixtime
 * @return ObcTime
 */
ObcTime TMGR_get_obc_time_from_unixtime(const double unixtime);

/**
 * @brief ��ʓI��unixtime��, UTL_cmd�ŗp���� utl_unixtime �ɕϊ�����
 * @note ������ unixtime �� utl_unixtime_epoch_ ��菬�����ꍇ�� 0 ��Ԃ�
 * @param[in] unixtime �ϊ������� unixtime
 * @return utl_unixtime (�f�t�H���g�ł�2020-01-01T00:00:00Z �N�Z, cycle����)
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
void TMGR_update_unixtime_info(const double unixtime, const ObcTime* time);

CCP_EXEC_STS Cmd_TMGR_SET_TIME(const CTCP* packet);
CCP_EXEC_STS Cmd_TMGR_SET_UNIXTIME(const CTCP* packet);

#endif
