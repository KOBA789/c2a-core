/**
 * @file  event_logger_group.h
 * @brief event_logger �� �C�x���g����ID���`
 * @note  �O���� C2A core �Ŏg���邽�߁CID�̂��Ԃ�ɒ��ӂ��邱��
 */
#ifndef EVENT_LOGGER_GROUP_H_
#define EVENT_LOGGER_GROUP_H_

/**
 * @enum  EL_GROUP
 * @brief event_logger �� Event Group �� user ��`����
 * @note  uint32_t ��z��
 * @note  �O���� id �� C2A core ���� EL_CORE_GROUP �Ƃ��Ē�`����Ă��邱�Ƃɒ��ӂ���
 */
typedef enum
{
  // C2A CORE: 0-19
  // src_core/System/EventManager/event_logger.h �� EL_CORE_GROUP �ɂĒ�`

  // CDH: 20-39
  EL_GROUP_UART = 20,         // TODO
  EL_GROUP_UART_REOPEN,

  // TEST
  EL_GROUP_TEST  = 0xf0,         // pytest�p
  EL_GROUP_TEST1 = 0xf1,         // pytest�p

  // �Ƃ肠�����ő�l��0xff�ɁI
  EL_GROUP_MAX = 0x100       // ����͂�����Ȃ��I
} EL_GROUP;

#endif
