#pragma section REPRO
/**
 * @file  event_manager.c
 * @brief C2A�S�̂̃C�x���g���Ǘ�����
 * @note  event_logger �� event_handler ���Ǘ�����
 */
#include "event_manager.h"
#include "event_logger.h"
#include "event_handler.h"

void EM_initialize(void)
{
  EL_initialize();
}

#pragma section
