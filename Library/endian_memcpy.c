#pragma section REPRO
/**
 * @file endian_memcpy.c
 * @brief  memcpy�̃G���f�B�A���΍�
 */

/*
memcpy�ŃG���f�B�A���̂����Ŏ��ʂ��Ƃ�����̂ŁC
OBC�ł�memcpy�CSLIS���ł�endian conv������悤�ɃX�C�b�`������D

memcpy(&sci_.sib_rev, CCP_get_param_head(packet), 2);
��
endina_memcpy(&sci_.sib_rev, CCP_get_param_head(packet), 2);
*/

#include "endian_memcpy.h"
#include "endian_conv.h"
#include "string.h" // for memcpy
#include <src_user/Settings/build_settings.h>

#ifdef SILS_FW
#include "stdlib.h" // for malloc
#endif // SILS_FW

void* endian_memcpy(void* dest, const void* src, size_t count)
{
#ifdef IS_LITTLE_ENDIAN
  // ������copy�Ɠ����̂��Ƃ��s���Ă���
  endian_conv(src, dest, (int)count);
#else
  memcpy(dest, src, count);
#endif

  return dest;
}

#pragma section
