/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 ȫϵ�� ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h7xx.h"

 /* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_queue.h"

/* �弶����ͷ�ļ� */
#include "bsp_led.h"
#include "core_delay.h"  
#include "./key/bsp_key.h" 


/********************************** ������ *************************************/
/* 
 * ��������һ��ָ�룬����ָ��һ�����񣬵����񴴽���֮�����;�����һ��������
 * �Ժ�����Ҫ��������������Ҫͨ�������������������������������Լ�����ô
 * ����������ΪNULL��
 */

/* ���������� */
UINT32 Receive_Task_Handle;
UINT32 Send_Task_Handle;

/********************************** �ں˶����� *********************************/
/*
 * �ź�������Ϣ���У��¼���־�飬�����ʱ����Щ�������ں˵Ķ���Ҫ��ʹ����Щ�ں�
 * ���󣬱����ȴ����������ɹ�֮��᷵��һ����Ӧ�ľ����ʵ���Ͼ���һ��ָ�룬������
 * �ǾͿ���ͨ��������������Щ�ں˶���
 *
 * �ں˶���˵���˾���һ��ȫ�ֵ����ݽṹ��ͨ����Щ���ݽṹ���ǿ���ʵ��������ͨ�ţ�
 * �������¼�ͬ���ȸ��ֹ��ܡ�������Щ���ܵ�ʵ��������ͨ��������Щ�ں˶���ĺ���
 * ����ɵ�
 * 
 */
 /* ������Ϣ���еľ�� */
UINT32 Test_Queue_Handle;
/* ������Ϣ���г��� */
#define  TEST_QUEUE_LEN  	10
#define  TEST_QUEUE_SIZE  10

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */


/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_Receive_Task(void);
static UINT32 Creat_Send_Task(void);

static void Receive_Task(void);
static void Send_Task(void);

extern void BSP_Init(void);

/***************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
			�ڶ���������APPӦ������
			������������LiteOS����ʼ��������ȣ�����ʧ�������������Ϣ
  **************************************************************/
int main(void)
{	
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;  
	
	/* ������س�ʼ�� */
  BSP_Init();
	
	printf("\n\n");
	printf("����KEY1����KEY2���Ͷ�����Ϣ\n");
	printf("Receive_Task������յ���Ϣ�ڴ��ڻ���\n\n");
	
	/* LiteOS �ں˳�ʼ�� */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }
	
	uwRet = AppTaskCreate();
	if (uwRet != LOS_OK)
  {
		printf("AppTaskCreate��������ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }

  /* ����LiteOS������� */
  LOS_Start();
	
	//��������²���ִ�е�����
	while(1);
}


/*********************************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� ���񴴽���Ϊ�˷���������е����񴴽����������Է��������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static UINT32 AppTaskCreate(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;
	
	/* ����һ�����Զ���*/
	uwRet = LOS_QueueCreate("Test_Queue",				/* ���е����� */
													TEST_QUEUE_LEN,			/* ���еĳ��� */
													&Test_Queue_Handle,	/* ���е�ID(���) */
													0,						/* ����ģʽ���ٷ���ʱδʹ�� */
													TEST_QUEUE_SIZE);	/* �ڵ��С����λΪ�ֽ� */
	if (uwRet != LOS_OK)
  {
		printf("Test_Queue���д���ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
    
	uwRet = Creat_Receive_Task();
  if (uwRet != LOS_OK)
  {
		printf("Receive_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }

	uwRet = Creat_Send_Task();
  if (uwRet != LOS_OK)
  {
		printf("Send_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}

/******************************************************************
  * @ ������  �� Creat_Receive_Task
  * @ ����˵���� ����Receive_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Receive_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Receive_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Receive_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}


/*******************************************************************
  * @ ������  �� Creat_Send_Task
  * @ ����˵���� ����Send_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Send_Task()
{
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Send_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Send_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}


/******************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Receive_Task(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;
	UINT32 *r_queue;
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
		uwRet = LOS_QueueRead(Test_Queue_Handle,	/* ��ȡ�����գ����е�ID(���) */
													&r_queue,			/* ��ȡ�����գ������ݱ���λ�� */
													TEST_QUEUE_SIZE,		/* ��ȡ�����գ������ݵĳ��� */
													LOS_WAIT_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */
		if(LOS_OK == uwRet)
		{
			printf("���ν��յ���������%x\n",*r_queue);
		}
		else
		{
			printf("���ݽ��ճ���,�������0x%X\n",uwRet);
		}
	}
}


/******************************************************************
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Send_Task(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;				
	
	//Ҫ���͵���Ϣ
	UINT32 send_data1 = 1;
	UINT32 send_data2 = 2;
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/* K1 ������ */
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )      
		{
			/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
			uwRet = LOS_QueueWrite(	Test_Queue_Handle,	/* д�루���ͣ����е�ID(���) */
															&send_data1,				/* д�루���ͣ������� */
															sizeof(send_data1),	/* ���ݵĳ��� */
															0);									/* �ȴ�ʱ��Ϊ 0  */
			if(LOS_OK != uwRet)
			{
				printf("���ݲ��ܷ��͵���Ϣ���У��������0x%X\n",uwRet);
			}
		}
		
		/* K2 ������ */
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON ) 
		{
			/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
			uwRet = LOS_QueueWrite(	Test_Queue_Handle,	/* д�루���ͣ����е�ID(���) */
															&send_data2,				/* д�루���ͣ������� */
															sizeof(send_data2),	/* ���ݵĳ��� */
															0);									/* �ȴ�ʱ��Ϊ 0  */
			if(LOS_OK != uwRet)
			{
				printf("���ݲ��ܷ��͵���Ϣ���У��������0x%X\n",uwRet);
			}
		}
		/* 20msɨ��һ�� */
		LOS_TaskDelay(20);       
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
