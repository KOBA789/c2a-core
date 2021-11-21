/**
 * @file  wdt.h
 * @brief watchdog timer �� IF wapper
 */
#ifndef WDT_H_
#define WDT_H_

#include <src_user/Library/stdint.h>

typedef struct
{
  int     timer_setting;
  uint8_t is_wdt_enable;
  uint8_t is_clear_enable;
} WDT_Config;

/**
 * @brief  WDT�̏�����
 * @param  *wdt_config: WDT_Config �\���̂ւ̃|�C���^
 * @retval 0:     ����I��
 * @retval 0�ȊO: �ُ�I��
 */
int WDT_initialize(void* wdt_config);

/**
 * @brief  WDT�̃J�E���^�N���A
 * @param  *wdt_config: WDT_Config �\���̂ւ̃|�C���^
 * @retval 0:     ����I��
 * @retval 0�ȊO: �ُ�I���i�p�����^�G���[��WDT�������Ȃǁj
 */
int WDT_clear(void* wdt_config);

/**
 * @brief  WDT�̗L����
 * @param  *wdt_config: WDT_Config �\���̂ւ̃|�C���^
 * @retval 0:     ����I��
 * @retval 0�ȊO: �ُ�I���i�p�����^�G���[�₷�ł�WDT���L���ȏꍇ�Ȃǁj
 */
int WDT_enable(void* wdt_config);

/**
 * @brief  WDT�̗L����
 * @param  *wdt_config: WDT_Config �\���̂ւ̃|�C���^
 * @retval 0:     ����I��
 * @retval 0�ȊO: �ُ�I���i�p�����^�G���[�₷�ł�WDT�������ȏꍇ�Ȃǁj
 */
int WDT_disable(void* wdt_config);

/**
 * @brief  WDT�̎��Ԑݒ�
 * @param  *wdt_config: WDT_Config �\���̂ւ̃|�C���^
 * @retval 0:     ����I��
 * @retval 0�ȊO: �ُ�I���i�p�����^�G���[�₷�ł�WDT���쒆�Ȃǁj
 */
int WDT_set_timer(void* wdt_config, int time);

#endif
