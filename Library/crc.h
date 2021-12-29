/**
 * @file
 * @brief CRC�p�̃��C�u����
 */
#ifndef CRC_H_
#define CRC_H_

#include <stddef.h> // for size_t
#include <src_user/Library/stdint.h>

/**
 * @brief CRC-16-IBM
 *
 *        ����������: x^16 + x^15 + x^2 + 1
 *        �r�b�g����: �E����, POLLY: 0xa001
 *        �ǂݏo��:   1byte(8 bit)
 * @param[in] crc: CRC�����l
 * @param[in] c: CRC���v�Z����byte��
 * @param[in] n: ��̒���
 * @param[in] rev_flag: ���]���邩�ǂ���
 * @return uint16_t: �v�Z����
 */
uint16_t crc_16_ibm_right(uint16_t crc, const unsigned char* c, size_t n, int rev_flag);

/**
 * @brief CRC-16-CCITT
 *
 *        ����������: x^16 + x^12 + x^5 + 1
 *        �r�b�g����: ������, POLLY: 0x1021
 *        �ǂݏo��:   1byte(8 bit)
 * @param[in] crc: CRC�����l
 * @param[in] c: CRC���v�Z����byte��
 * @param[in] n: ��̒���
 * @param[in] rev_flag: ���]���邩�ǂ���
 * @return uint16_t: �v�Z����
 */
uint16_t crc_16_ccitt_left(uint16_t crc, const unsigned char* c, size_t n, int rev_flag);

/**
 * @brief CRC-16-CCITT
 *
 *        ����������: x^16 + x^12 + x^5 + 1
 *        �r�b�g����: �E����, POLLY: 0x8408
 *        �ǂݏo��:   1byte(8 bit)
 * @param[in] crc: CRC�����l
 * @param[in] c: CRC���v�Z����byte��
 * @param[in] n: ��̒���
 * @param[in] rev_flag: ���]���邩�ǂ���
 * @return uint16_t: �v�Z����
 */
uint16_t crc_16_ccitt_right(uint16_t crc, const unsigned char* c, size_t n, int rev_flag);

/**
 * @brief crc�p�̃e�[�u���쐬
 *
 *        �ǂݏo��:   1byte(8bit)(sizeof(table) = 256)
 *        �r�b�g����: �E����
 * @param[out] table: �e�[�u��
 * @param[in] crc_poly: ����������
 * @param[in] shift: �E:1, ��:0
 * @note �g��Ȃ��Ƃ͎v���܂���...
 */
void make_crc_8_table(uint8_t* table, uint8_t crc_poly, uint8_t shift);
void make_crc_16_table(uint16_t* table, uint16_t crc_poly, uint8_t shift);
void make_crc_32_table(uint32_t* table, uint32_t crc_poly, uint8_t shift);

// �g��Ȃ��Ǝv���܂����f�o�b�O�p��
// uint16_t crc_16_right_calc(uint16_t crc, uint16_t crc_poly, const unsigned char* c, size_t n, int rev_flag);
// uint16_t crc_16_left_calc(uint16_t crc, uint16_t crc_poly, const unsigned char* c, size_t n, int rev_flag);

#endif
