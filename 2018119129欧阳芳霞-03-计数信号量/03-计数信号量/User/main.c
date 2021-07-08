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
#include "los_sem.h"

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
UINT32 Pend_Task_Handle;
UINT32 Post_Task_Handle;

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
/* ��������ź����ľ�� */
UINT32 CountSem_Handle;

/******************************* ȫ�ֱ������� ************************************/
/*
 * ��������дӦ�ó����ʱ�򣬿�����Ҫ�õ�һЩȫ�ֱ�����
 */



/* �������� */
static UINT32 AppTaskCreate(void);
static UINT32 Creat_Pend_Task(void);
static UINT32 Creat_Post_Task(void);

static void Pend_Task(void);
static void Post_Task(void);

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
	UINT32 uwRet = LOS_OK;  //����һ�����񴴽��ķ���ֵ��Ĭ��Ϊ�����ɹ�
	
	/* ������س�ʼ�� */
  BSP_Init();
	
	printf("����һ��[Ұ��]-STM32ȫϵ�п�����-LiteOS�����ź���ʵ�飡\n\n");
  printf("��λĬ��ֵΪ5��������KEY1���복λ������KEY2�ͷų�λ��\n\n");
	
	/* LiteOS �ں˳�ʼ�� */
	uwRet = LOS_KernelInit();
	
  if (uwRet != LOS_OK)
  {
		printf("LiteOS ���ĳ�ʼ��ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return LOS_NOK;
  }
	
	 /* ����APPӦ���������е�Ӧ�����񶼿��Է�������������� */
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
	
	/* ����һ�������ź�������ʼ������ֵΪ5*/
	uwRet = LOS_SemCreate (5,&CountSem_Handle);
	if (uwRet != LOS_OK)
	{
		printf("CountSem����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
	}
		
	uwRet = Creat_Pend_Task();
  if (uwRet != LOS_OK)
  {
		printf("Pend_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	
	uwRet = Creat_Post_Task();
  if (uwRet != LOS_OK)
  {
		printf("Post_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		return uwRet;
  }
	return LOS_OK;
}


/******************************************************************
  * @ ������  �� Creat_Pend_Task
  * @ ����˵���� ����Pend_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Pend_Task()
{
	//����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Pend_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Pend_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Pend_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Post_Task
  * @ ����˵���� ����Post_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Post_Task()
{
	// ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Post_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Post_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Post_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

/******************************************************************
  * @ ������  �� Pend_Task
  * @ ����˵���� Pend_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Pend_Task(void)
{
	UINT32 uwRet = LOS_OK;				
	
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		//���KEY1������
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )      
		{
			/* ��ȡһ�������ź������ȴ�ʱ��0 */
			uwRet = LOS_SemPend ( CountSem_Handle,0); 

			if (LOS_OK ==  uwRet)                      
				printf ( "\r\nKEY1�����£��ɹ����뵽ͣ��λ��\r\n" );
			else
				printf ( "\r\nKEY1�����£�������˼������ͣ����������\r\n" );			
				
		}
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}
}
/******************************************************************
  * @ ������  �� Post_Task
  * @ ����˵���� Post_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Post_Task(void)
{
	UINT32 uwRet = LOS_OK;				
   
	while(1)
	{
		//���KEY2������
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON )       
		{
			/* �ͷ�һ�������ź�����LiteOS�ļ����ź�������һֱ�ͷţ��ڱ����ע��һ�¼���*/
			uwRet = LOS_SemPost(CountSem_Handle);    
				
			if ( LOS_OK == uwRet ) 
				printf ( "\r\nKEY2�����£��ͷ�1��ͣ��λ��\r\n" );	
			else
				printf ( "\r\nKEY2�����£������޳�λ�����ͷţ�\r\n" );					
	
		}
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}		
}





/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
