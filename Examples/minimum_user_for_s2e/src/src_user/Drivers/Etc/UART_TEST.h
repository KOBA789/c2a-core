/**
* @file   UART_TEST.h
* @brief  �e�X�g�pUART��Driver
*/

#ifndef UART_TEST_H
#define UART_TEST_H

#include <src_core/IfWrapper/uart.h>
#include <src_core/Drivers/Super/driver_super.h>


/**
 * @struct UART_TEST_Info
 * @brief
 */
typedef struct
{
  uint8_t flag;
} UART_TEST_Info;


/**
 * @struct  UART_TEST_Driver
 * @brief
 */
typedef struct
{
  struct
  {
    DriverSuper super;                  //!< DriverSuper class
    UART_Config uart_config;           //!< RS422 class
  } driver;
  UART_TEST_Info info;
} UART_TEST_Driver;


// ��{�֐�

/**
 * @brief  UART_TEST������
 *
 *         UART_TEST_Driver�\���̂̃|�C���^��n�����ƂŃ|�[�g�����������CUART_TEST_Driver�̊e�����o������������
 * @param  *uart_test_instance : ����������UART_TEST_Driver�\���̂ւ̃|�C���^
 * @param  ch    : UART_TEST���ڑ�����Ă���UART�|�[�g�ԍ�
 * @return DS_INIT_ERR_CODE
 */
DS_INIT_ERR_CODE UART_TEST_init(UART_TEST_Driver* uart_test_instance, unsigned char ch);


/**
 * @brief  UART_TEST�̃f�[�^�i�e�����j��M
 * @param  *uart_test_instance : UART_TEST_Driver�\���̂ւ̃|�C���^
 * @return DS_REC_ERR_CODE
 */
DS_REC_ERR_CODE UART_TEST_rec(UART_TEST_Driver* uart_test_instance);


/**
 * @brief  UART_TEST�ւ̃R�}���h���M
 * @param  *uart_test_instance   : UART_TEST_Driver�\���̂ւ̃|�C���^
 * @param  id      : Cmd id
 * @return DS_CMD_ERR_CODE
 */
DS_CMD_ERR_CODE UART_TEST_send(UART_TEST_Driver* uart_test_instance, uint8_t id);


#endif
