#ifndef ANOMALY_GROUP_H_
#define ANOMALY_GROUP_H_

// #include <src_core/System/AnomalyLogger/anomaly_logger.h>
// ��anomaly_logger.h����include�����ׂ��ł����āC�����������include���Ȃ�

// anomaly_logger.h �ŁC�R�A����AL_GROUP�����ɒ�`���Ă���D
typedef enum
{
  // C2A CORE: 0-9
  // anomaly_logger.h ��AL_GROUP�Q��

  // OBC: 10-29
  AL_GROUP_UART = 10,
  AL_GROUP_UTIL_CNT,
  AL_GROUP_SAMPLE,

  // �Ƃ肠�����ő�l��0xff�ɁI
  AL_GROUP_MAX = 0x100       // ����͂�����Ȃ��I�i���ƁC8�̔{���ł��邱�Ƃ�v���j
} AL_GROUP;

#endif
