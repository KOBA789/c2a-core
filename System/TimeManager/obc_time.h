#ifndef OBC_TIME_H_
#define OBC_TIME_H_

#include <src_user/Library/stdint.h>

// step, cycle�ɂ��Ă�TimeManager���Q�Ƃ̂���
#define OBCT_STEP_IN_MSEC (1)                                                    //!< 1step�ŉ�ms��
#define OBCT_STEPS_PER_CYCLE (100)                                               //!< ��step��1cycle��
#define OBCT_CYCLES_PER_SEC (1000 / OBCT_STEP_IN_MSEC / OBCT_STEPS_PER_CYCLE)    //!< 1s�ŉ�cycle��
#define OBCT_MAX_CYCLE (0xfffffff0u)                                             //!< �ő�cycle���D�܂�TI�������ŃI�[�o�[�t���[���邩
/**
 * @def
 * 2020-1-1-00:00:00UTC ���_��unixtime
 * ����������[���Ƃ��ċN�Z����0.1�b���݂̎�����utl_unixtime�ƒ�`��, UTL_cmd�̎��s�������Ƃ��ėp����
 */
#define OBCT_UNIXTIME_EPOCH_FOR_UTL (1577836800.0)

#include <src_user/Settings/System/obc_time_params.h>

typedef uint32_t cycle_t;
typedef uint32_t step_t;

/**
 * @struct ObcTime
 * @brief OBC�̎�������ێ�����\����
 */
typedef struct
{
  cycle_t total_cycle; //!< ti�̂���. OBC���N�����Ă���i���m�ɂ́C�\���̂�����������Ă���j�̌o�ߎ���
  cycle_t mode_cycle;  //!< �Ō��mode��ς��Ă���̌o�ߎ���
  step_t  step;        //!< ����step�DTimeLine�ł̏������ԕ����Ɏg����
} ObcTime;

/**
 * @struct OBCT_UnixtimeInfo
 * @brief unixtime��ObcTime��R�Â���\����
 */
typedef struct
{
  double unixtime_at_ti0;    //!< �ϑ���񂩂�v�Z����, master_clock �� {0, 0, 0} �̎��� unixtime
  cycle_t ti_at_last_update; //!< UnixtimeInfo ���Ō�ɍX�V�������Ɏg����(GPS�Ȃǂ�)�����ϑ��������_��total_cycle
} OBCT_UnixtimeInfo;


/**
 * @brief �������� ObcTime ���쐬����
 * @param[in] total_cycle
 * @param[in] mode_cycle
 * @param[in] step
 * @return �V�K�ɍ쐬���� ObcTime �\����
 */
ObcTime OBCT_create(cycle_t total_cycle,
                    cycle_t mode_cycle,
                    step_t step);

/**
 * @brief ObcTime ���N���A���S�ă[���ɂ���
 * @param[in] time �N���A����� ObcTime �\����
 */
void OBCT_clear(ObcTime* time);

/**
 * @brief ObcTime��step���݂ŃC���N�������g����
 * @param[in] time �J�E���g�A�b�v����� ObcTime �\����
 */
void OBCT_count_up(ObcTime* time);

/**
 * @brief ObcTime ����肤��ő�l��Ԃ�
 * @return �e�����o�[���ő�l�ɂ��ĐV�K�쐬���� ObcTime �\����
 */
ObcTime OBCT_get_max(void);

/**
 * @brief �����Ŏw�肵�� ObcTime �� total_cycle ��Ԃ�
 * @return total_cycle
 */
cycle_t OBCT_get_total_cycle(const ObcTime* time);

/**
 * @brief �����Ŏw�肵�� ObcTime �� mode_cycle ��Ԃ�
 * @return mode_cycle
 */
cycle_t OBCT_get_mode_cycle(const ObcTime* time);

/**
 * @brief �����Ŏw�肵�� ObcTime �� step ��Ԃ�
 * @return step
 */
step_t  OBCT_get_step(const ObcTime* time);

/**
 * @brief �����Ŏw�肵�� ObcTime �� total_cycle ���~���b�P�ʂŕԂ�
 * @note �v�Z���step���l��
 * @return �~���b�P�ʂ� total_cycle
 */
uint32_t OBCT_get_total_cycle_in_msec(const ObcTime* time);

/**
 * @brief �����Ŏw�肵�� ObcTime �� mode_cycle ���~���b�P�ʂŕԂ�
 * @note �v�Z���step���l��
 * @return �~���b�P�ʂ� mode_cycle
 */
uint32_t OBCT_get_mode_cycle_in_msec(const ObcTime* time);

/**
 * @brief �����Ŏw�肵�� ObcTime �� total_cycle ��b�P�ʂŕԂ�
 * @note �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
 * @return �b�P�ʂ� total_cycle. �����_�ȉ����ێ�
 */
float    OBCT_get_total_cycle_in_sec(const ObcTime* time);

/**
 * @brief �����Ŏw�肵�� ObcTime �� mode_cycle ��b�P�ʂŕԂ�
 * @note �v�Z���step���l���i�I�[�o�[�t���[�ɒ��Ӂj
 * @return �b�P�ʂ� mode_cycle. �����_�ȉ����ێ�
 */
float    OBCT_get_mode_cycle_in_sec(const ObcTime* time);

/**
 * @brief �~���b�� cycle �ɕϊ�����
 * @return cycle (�[���͐؂�̂�)
 */
cycle_t  OBCT_msec2cycle(uint32_t msec);

/**
 * @brief cycle ���~���b�ɕϊ�����
 * @return �~���b�P�ʂɕϊ����ꂽ cycle
 */
uint32_t OBCT_cycle2msec(cycle_t cycle);

/**
 * @brief �b�� cycle �ɕϊ�����
 * @return cycle
 */
cycle_t  OBCT_sec2cycle(uint32_t sec);

/**
 * @brief cycle ��b�ɕϊ�����
 * @return �b�P�ʂɕϊ����ꂽcycle (�[���͐؂�̂ĂĐ����ɂ���)
 */
uint32_t OBCT_cycle2sec(cycle_t cycle);       // �K���Ɋۂ߂��邱�Ƃɒ���

/**
 * @brief ObcTime �̈����Z���s��
 * @param[in] before ���� ObcTime
 * @param[in] after ������� ObcTime
 * @return after ���� before �����������ʂ� ObcTime
 */
ObcTime OBCT_diff(const ObcTime* before,
                  const ObcTime* after);

/**
 * @brief ObcTime �̈����Z�� step �P�ʂōs��
 * @return after - before �� step �P�ʂɕϊ���������
 */
step_t OBCT_diff_in_step(const ObcTime* before,
                         const ObcTime* after);

/**
 * @brief ObcTime �̈����Z���~���b�P�ʂōs��
 * @return after - before ���~���b�P�ʂɕϊ���������
 */
uint32_t OBCT_diff_in_msec(const ObcTime* before,
                           const ObcTime* after);

/**
 * @brief ObcTime �̈����Z��b�P�ʂōs��
 * @return after - before ��b�P�ʂɕϊ���������
 */
float OBCT_diff_in_sec(const ObcTime* before,
                       const ObcTime* after);

/**
 * @brief ObcTime �̑����Z���s��
 * @param[in] left  �ꍀ��
 * @param[in] right �񍀖�
 * @return �����𑫂����킹�����ʂ� ObcTime
 */
ObcTime OBCT_add(const ObcTime* left, const ObcTime* right);

/**
 * @brief ObcTime �̔�r
 * @note  mode_cycle �͌��Ȃ�
 * @param[in] t1: ��r�Ώ�
 * @param[in] t2: ��r�Ώ�
 * @retval 1  : t1 < t2
 * @retval 0  : t1 == t2
 * @retval -1 : t2 > t1
 */
int OBCT_compare(const ObcTime* t1, const ObcTime* t2);

void OBCT_print(const ObcTime* time);

/**
 * @brief �ϑ����� UnixTime �Ƃ�����ϑ��������� ObcTime ���󂯎�� OBCT_UnixtimeInfo ���쐬����
 * @param[in] time (GPS ������) unixtime ���ϑ��������� ObcTime
 * @param[in] unixtime (GPS ������ϑ�����) unixtime
 * @return ��������v�Z���� OBCT_UnixtimeInfo
 */
OBCT_UnixtimeInfo OBCT_create_unixtime_info(const double unixtime, const ObcTime* time);

/**
 * @brief OBCT_UnixtimeInfo ���N���A���đS��0�ɂ���
 * @param[in] uti �N���A���� OBCT_UnixtimeInfo
 */
void OBCT_clear_unixtime_info(OBCT_UnixtimeInfo* uti);

/**
 * @brief ��ʓI��unixtime��, UTL_cmd�ŗp���� utl_unixtime �ɕϊ�����
 * @param[in] unixtime �ϊ������� unixtime
 * @return utl_unixtime (2020-1-1-00:00:00UTC �N�Z, 0.1�b���݂Ȃ̂ŒP�ʂ�cycle_t)
 */
cycle_t OBCT_get_utl_unixtime_from_unixtime(const double unixtime);

/**
 * @brief OBCT_UnixtimeInfo ���ϑ�����p���čX�V����
 * @param[in] uti �X�V���� OBCT_UnixtimeInfo
 * @param[in] unixtime (GPS ������ϑ�����) unixtime
 * @param[in] time (GPS ������) unixtime ���ϑ��������� ObcTime
 */
void OBCT_update_unixtime_info(OBCT_UnixtimeInfo* uti, const double unixtime, const ObcTime time);

#endif
