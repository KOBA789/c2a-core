#pragma section REPRO
/**
 * @file  event_utility.c
 * @brief �C�x���g�����̒�����s�̂��߂�App
 * @note  �����I�ɂ� event_manager �������s���邾��
 */
#include "event_utility.h"

#include <stddef.h> // for NULL

static void EVENT_UTIL_update_(void);

AppInfo EVENT_UTIL_create_app(void)
{
  return create_app_info("event", NULL, EVENT_UTIL_update_);
}

static void EVENT_UTIL_update_()
{
  // TODO: ��������
  // EM_update_by_event_utility_app();
}

#pragma section
