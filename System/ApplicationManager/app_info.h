/**
 * @file
 * @brief App ���\���̒�`�� App �쐬
 */
#ifndef APP_INFO_H_
#define APP_INFO_H_

#include "../TimeManager/obc_time.h"

typedef struct
{
  const char* name;           //!< �A�v���� (C2A �����ł͎g�p����Ă��Ȃ� )
  step_t init_duration;       //!< �A�v����������������
  step_t prev;                //!< �A�v�����s�������ԁi���߁j
  step_t min;                 //!< �A�v�����s�������ԁi�ŏ��l�j
  step_t max;                 //!< �A�v�����s�������ԁi�ő�l�j
  void (*initializer)(void);  //!< �A�v���������֐�
  void (*entry_point)(void);  //!< �A�v�����s�֐��i�G���g���[�|�C���g�j
} AppInfo;

/**
 * @brief  AppInfo �����
 * @param  name: �A�v����
 * @param  initializer: �A�v���������֐�
 * @param  entry_point: �A�v�����s�֐�
 * @return �쐬���ꂽ AppInfo
 */
AppInfo AI_create_app_info(const char* name, 
                           void (*initializer)(void),
                           void (*entry_point)(void));

#endif
