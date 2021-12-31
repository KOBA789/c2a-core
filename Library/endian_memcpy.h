/**
 * @file
 * @brief  memcpy�̃G���f�B�A���΍�
 */

/*
memcpy�ŃG���f�B�A���̂����Ŏ��ʂ��Ƃ�����̂ŁC
OBC�ł�memcpy�CSLIS���ł�endian conv������悤�ɃX�C�b�`������D

memcpy(&sci_.sib_rev, CCP_get_param_head(packet), 2);
��
endina_memcpy(&sci_.sib_rev, CCP_get_param_head(packet), 2);
*/

#ifndef ENDIAN_MEMCPY_INCLUDE_
#define ENDIAN_MEMCPY_INCLUDE_


#include <stddef.h> // for size_t

// #pragma inline  endian_memcpy
void* endian_memcpy(void* dest, const void* src, size_t count);

#endif // ENDIAN_MEMCPY_INCLUDE_
