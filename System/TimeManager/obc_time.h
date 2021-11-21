#ifndef OBC_TIME_H_
#define OBC_TIME_H_

#include <src_user/Library/stdint.h>

// step, cycle�ɂ��Ă�TimeManager���Q�Ƃ̂���
#define OBCT_STEP_IN_MSEC (1)                                                    //!< 1step�ŉ�ms��
#define OBCT_STEPS_PER_CYCLE (100)                                               //!< ��step��1cycle��
#define OBCT_CYCLES_PER_SEC (1000 / OBCT_STEP_IN_MSEC / OBCT_STEPS_PER_CYCLE)    //!< 1s�ŉ�cycle��
#define OBCT_MAX_CYCLE (0xfffffff0u)                                             //!< �ő�cycle���D�܂�TI�������ŃI�[�o�[�t���[���邩

#include <src_user/Settings/System/obc_time_params.h>

typedef uint32_t cycle_t;
typedef uint32_t step_t;

typedef struct
{
  cycle_t total_cycle;   // OBC���N�����Ă���i���m�ɂ́C�\���̂�����������Ă���j�̌o�ߎ��� (cycle)
  cycle_t mode_cycle;    // �Ō��mode��ς��Ă���̌o�ߎ��� (cycle)
  step_t  step;          // ����step�DTimeLine�ł̏������ԕ����Ɏg���� (step)
} ObcTime;

/**
 * @brief unix_time��ObcTime��R�Â���\����
 */
typedef struct
{
  double unix_time_on_ti0; //!< �ϑ���񂩂�v�Z����master_clock��(0, 0)�̎���unix_time
  cycle_t last_update_ti;  //!< �Ō�ɍX�V�������Ɏg����unix_time�̏���(GPS�Ȃǂ���)�ϑ���������ObcTime
} OBCT_UnixTimeInfo;

ObcTime OBCT_create(cycle_t total_cycle,
                    cycle_t mode_cycle,
                    step_t step);
ObcTime OBCT_get_max(void);
void OBCT_clear(ObcTime* time);
void OBCT_count_up(ObcTime* time);
cycle_t OBCT_get_total_cycle(const ObcTime* time);
cycle_t OBCT_get_mode_cycle(const ObcTime* time);
step_t  OBCT_get_step(const ObcTime* time);
uint32_t OBCT_get_total_cycle_in_msec(const ObcTime* time);  // �v�Z���step���l��
uint32_t OBCT_get_mode_cycle_in_msec(const ObcTime* time);   // �v�Z���step���l��
float    OBCT_get_total_cycle_in_sec(const ObcTime* time);   // �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
float    OBCT_get_mode_cycle_in_sec(const ObcTime* time);    // �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
cycle_t OBCT_sec2cycle(uint32_t sec);
uint32_t OBCT_cycle2sec(cycle_t cycle);
ObcTime OBCT_diff(const ObcTime* before,
                  const ObcTime* after);
step_t OBCT_diff_in_step(const ObcTime* before,
                         const ObcTime* after);
uint32_t OBCT_diff_in_msec(const ObcTime* before,
                               const ObcTime* after);
float OBCT_diff_in_sec(const ObcTime* before,
                       const ObcTime* after);
void OBCT_print(const ObcTime* time);

/**
 * @brief OBCT_UnixTimeInfo �쐬�֐�
 *
 *        �ϑ����� UnixTime �Ƃ�����ϑ��������� ObcTime ���󂯎�� OBCT_UnixTimeInfo �֕ς���.
 * @param[in] time (GPS ������) unix_time ���ϑ��������� ObcTime
 * @param[in] unix_time (GPS ������ϑ�����) unix_time
 * @return OBCT_UnixTimeInfo ��������v�Z���� OBCT_UnixTimeInfo
 */
OBCT_UnixTimeInfo OBCT_create_unix_time_info(const double unix_time, const ObcTime* time);
/**
 * @brief OBCT_UnixTimeInfo �N���A�֐�, �S��0�ɂ���
 * @param[in] uti �N���A���� OBCT_UnixTimeInfo
 */
void OBCT_clear_unix_time_info(OBCT_UnixTimeInfo* uti);
/**
 * @brief OBCT_UnixTimeInfo �C���֐�
 * @param[in] uti �C������ OBCT_UnixTimeInfo
 * @param[in] time (GPS ������) unix_time ���ϑ��������� ObcTime
 * @param[in] unix_time (GPS ������ϑ�����) unix_time
 */
void OBCT_modify_unix_time_info(OBCT_UnixTimeInfo* uti, const double unix_time, const ObcTime time);

#endif
