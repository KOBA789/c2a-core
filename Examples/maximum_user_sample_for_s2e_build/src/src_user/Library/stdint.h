#ifndef STDINT_H_
#define STDINT_H_

#include "../Settings/sils_define.h"

// HEW��O�����C�u�����w�b�_�[�Ƃ̏d��typedef���K�[�h
#ifndef _TYPEDEFINE_H_

#ifndef SILS_FW
// HEW�ɕW���w�b�_stdint.h�ɑ�������w�b�_���Ȃ��������߁A�K�v�@�\�����O�Œ�`�B
// bit���w��̐����^���ЂƂ܂���`

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed long    int32_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;

#else
#include <stdint.h>
#endif // SILS_FW

#endif // _TYPEDEFINE_H_
#endif // STDINT_H_
