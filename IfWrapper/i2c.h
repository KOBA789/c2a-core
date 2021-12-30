/**
 * @file
 * @brief I2C�ʐM(Master��)�̃��b�p�[�N���X�ł��B
 * @note I2C�ʐM���b�p�[�́A
 *       I2C�̃C���^�[�t�F�[�X���������A
 *       I2C�|�[�g�̏������A�f�[�^���M�A�f�[�^��M���s���A
 *       I2C�ʐM���b�p�[�̃N���X�ł��B
 *       �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp�������Ďg�p���܂��B
 */

#ifndef I2C_H_
#define I2C_H_

#include <src_user/Library/stdint.h>

 /**
  * @struct I2cConfig
  * @brief I2C�|�[�g�̏������A�f�[�^���M�E��M�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\���̂ł��B
  */
typedef struct
{
  uint8_t  ch;                 //!< �p����̋@�킪�Ȃ����Ă���|�[�g�ԍ�
  uint32_t frequency_khz;      //!< I2C�N���b�N�̎��g�� �P��:kHz
  uint8_t  device_address;     //!< �p����̋@��̃A�h���X
  uint8_t  stop_flag;          //!< �Ō��STOP�𑗂邩�ۂ���I�� 0->����Ȃ��C����ȊO->����
  uint32_t timeout_threshold;  //!< ���̒l�𒴂���ƃ^�C���A�E�g�G���[�ƂȂ� �P��:�J�E���g���i���[�U�[�ˑ��j TODO: �����ɂ��邩�v����
  uint32_t rx_length;          //!< I2C�ʐM�ɂ������M�f�[�^�� �P��: Byte
} I2C_Config;

/**
 * @enum  I2C_ERR_CODE
 * @brief I2C�p�̔ėp�G���[�R�[�h
 * @note  �^��int8_t��z�肵�Ă��邪, if_list�Ń��b�v����Ă��邽��I2C�̌��J�֐��̕Ԃ�l��int�ƂȂ��Ă���
 *        ��{���Ȃ̂�RX�̕Ԃ�l�͐���������Byte��, �����G���[�R�[�h�ƂȂ��Ă��邽��
 */
typedef enum
{
  I2C_UNKNOWN_ERR     = -20,  //!< �����s��
  I2C_RX_BUFFER_SMALL = -14,  //!< ��M�o�b�t�@�T�C�Y����M�f�[�^����菬����
  I2C_DATA_NEGA_ERR   = -13,  //!< ����M�f�[�^�T�C�Y�����̃G���[
  I2C_TIMEOUT         = -10,  //!< �ʐM����Timeout
  I2C_YET_ERR         = -4,   //!< �`�����l�����I�[�v��
  I2C_ALREADY_ERR     = -3,   //!< �`�����l���I�[�v���ς�
  I2C_FREQUENCY_ERR   = -2,   //!< ���g���ُ�
  I2C_CH_ERR          = -1,   //!< �`�����l���ُ� (Port_config�ɖ���)
  I2C_OK              = 0,    //!< OK��0�𓥏P
} I2C_ERR_CODE;

/**
 * @brief I2cConfig�\���̂��O���[�o���C���X�^���X�Ƃ��Ē�`���A�|�C���^��n�����ƂŃ|�[�g�����������܂��B
 * @param[in] my_i2c_v ����������I2cConfig�\���̂ւ̃|�C���^
 * @return int I2C_ERR_CODE�Ƃ�������, if_list��int�Ɠ��ꂳ��Ă���B
 * @note I2C�ʐM���b�p�[�̃N���X���g�p���鎞�͋N�����ɕK�����{���Ă��������B
 *       ���̊֐������s����O��I2cConfig�\���̓��̐ݒ�l��ݒ肵�Ă����K�v������܂��B
 */
int I2C_init(void* my_i2c_v);

/**
 * @brief I2cConfig�\���̂ɂĎw�肳�ꂽch����f�[�^����M���܂�
 * @param[in] my_i2c_v    : �ΏۂƂ���I2cConfig�\���̂ւ̃|�C���^
 * @param[in] data_v      : ��M�f�[�^�i�[��ւ̃|�C���^
 * @param[in] buffer_size : ��M�f�[�^�i�[��̃o�b�t�@�T�C�Y�AI2C�ʐM�ɂ���M�f�[�^�����������K�v������
 * @retval 0: ��M�f�[�^�Ȃ�
 * @retval ��: ��M�f�[�^�o�C�g��
 * @retval ��: �G���[. I2C_ERR_CODE �ɏ]��
 * @note ���̊֐�����I2C�ǂݏo���v���𑗐M���Ă��܂�
 */
int I2C_rx(void* my_i2c_v, void* data_v, int buffer_size);

/**
 * @brief I2cConfig�\���̂ɂĎw�肳�ꂽch�փf�[�^�𑗐M���܂�
 * @param[in] my_i2c_v : �ΏۂƂ���I2cConfig�\���̂ւ̃|�C���^
 * @param[in] data_v     : ���M�f�[�^�i�[��ւ̃|�C���^
 * @param[in] count      : ���M�f�[�^�T�C�Y
 * @return int:  I2C_ERR_CODE �ɏ]��
 */
int I2C_tx(void* my_i2c_v, void* data_v, int data_size);

/**
 * @brief �w�肳�ꂽch���J���Ȃ���
 * @param[in] my_i2c_v �J���Ȃ����ۂ̃|�[�g�ݒ�
 * @param[in] reason   reopen���闝�R�D0�͐��푀������� TODO: reason��enum�����H
 * @return int:  I2C_ERR_CODE �ɏ]��
 */
int I2C_reopen(void* my_i2c_v, int reason);

/**
 * @brief stop_flag��ݒ肷��
 * @param[in] stop_flag �ݒ肷��stop_flag
 * @return �Ȃ�
 */
void I2C_set_stop_flag(void* my_i2c_v, const uint8_t stop_flag);

/**
 * @brief rx_length��ݒ肷��
 * @param[in] rx_length  �ݒ肷���M�f�[�^��
 * @return �Ȃ�
 */
void I2C_set_rx_length(void* my_i2c_v, const uint32_t rx_length);


#endif
