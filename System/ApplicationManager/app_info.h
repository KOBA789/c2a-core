#ifndef APP_INFO_H_
#define APP_INFO_H_

#include "../TimeManager/obc_time.h"

typedef struct
{
  const char* name;
  step_t init_duration; // �A�v���������������ԕۑ��p
  step_t prev;          // �A�v�����s�������ԕۑ��p
  step_t min;           // �A�v�����s�������ԕۑ��p
  step_t max;           // �A�v�����s�������ԕۑ��p
  void (*initializer)(void);
  void (*entry_point)(void);
} AppInfo;

AppInfo create_app_info(const char* name,
                        void (*initializer)(void),
                        void (*entry_point)(void));

#endif // APP_INFO_H_
