#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef enum{OK,ERROR,OVERFLOW} Status;

typedef struct customer{	// �˿� 
	int type;	// ״̬�� 0����1�뿪 
	int money;	// ����ֵ��ֵȡ�� 
	int arriveTime; // ����ʱ��
	int tradeTime; // ����ʱ��  
	int num;	// ��� 
	customer* next;
}customer; 

typedef struct queue{	//���� 
	customer* rear;
	customer* front;
}queue;

int total = 0;	// �����ִ��ʽ��ܶ� 
int closeTime = 0;	//  ����Ӫҵʱ�� 
int currentTime = 0;	//��ǰʱ��  
int minDealTime = 0;	// ��С����ʱ��
int maxDealTime = 0;	// �����ʱ��
int minInterval = 0;	// ��С���ʱ��
int maxInterval = 0;	// �����ʱ��
int maxMoney = 0;		// ����׽�� 
int num = 1;	// �ͻ��� 
int lastType = 0;	// ��һλ�ͻ��¼�Ϊ,0ȡ��1��� 
queue Q1;	// ����1
queue Q2;	// ����2

int waitingTime = 0;	
int deposit_num = 0;
int deposit_suc = 0;
int deposit_over = 0;
int withdraw_num = 0;
int withdraw_suc = 0;
int withdraw_over = 0;



// ��ʼ������
Status init_queue(queue &q){
	q.front = q.rear = NULL;
	return OK;
} 

// �����λ�˿� 
customer* get_front(queue &q){
	return q.front;
}

// �˿����
Status push_customer(queue &q, customer* x) {
    if (!x) return ERROR;
    
    if (!q.front) {
        q.front = q.rear = x;
        x->next = NULL;
    } else {
        q.rear->next = x;
        q.rear = x;
        x->next = NULL;  // ȷ���½ڵ�� next ָ�� NULL
    }
    return OK;
}
// �˿ͳ���
Status pop_customer(queue &q, customer **x) {
//	printf("�˿ͳ��ӿ�ʼ\n");
    if (!q.front){
//    	printf("�����пͻ������ڡ�\t\t");
    	return ERROR;
	}
    *x = q.front;
    q.front = q.front->next;
    if (!q.front) {
        q.rear = NULL;
    }
//    printf("�˿ͳ��ӽ���\n");
    return OK;
}
//���ٶ��� 
Status destroy_queue(queue &q) {
    if (!q.front && !q.rear) return OK;
    customer* p = q.front;
    while (p) {
    	customer *temp = p->next;
        free(p);
        p = temp;
    }
    q.rear = NULL;
    return OK;
}

bool is_empty(queue &Q){
	if(Q.front == NULL ) return true;
	return false;
}

// ���ɵ���ͻ����� 
Status arrive_customer(){
	// �����ͻ� 
	customer* x = (customer*)malloc(sizeof(customer));
	if(!x) return OVERFLOW;
	
	x->arriveTime = currentTime;
	x->money = ((rand() % 2) * 2 - 1)*(maxMoney - rand() % (maxMoney + 1));
	if(x->money > 0) deposit_num++;
	else withdraw_num++;
	x->next = NULL;
	x->tradeTime = minDealTime + rand() % ( maxDealTime - minDealTime + 1);
	x->type = 0;
	x->num = num++;
	push_customer(Q1,x);
//	printf("�ͻ� %d ��ʱ��\t%d\t����,����ʱ��\t%d\t���Ϊ\t%d\t��\n",x->num,x->arriveTime,x->tradeTime,x->money);
	// ��һ���˿͵ĵ���ʱ�� 
	currentTime += minInterval + rand() % ( maxInterval - minInterval + 1); 
	return OK;
} 
//�뿪
Status leave_customer(customer *p){
	waitingTime += currentTime - p->arriveTime;
	free(p);
	return OK; 
} 
//������ͻ� 
Status deposit (customer *curCx){
//	printf("�����ʼ\n");
//	���µ�ǰʱ�� 
	if(currentTime < curCx->arriveTime) currentTime = curCx->arriveTime;
	if(currentTime + curCx->tradeTime > closeTime) {
		currentTime += curCx->tradeTime;
		printf("�� %d λ�ͻ��� %d ʱ�䵽� �� %d ʱ�䳬ʱ�������뿪��\n",curCx->num,curCx->arriveTime,currentTime);
		deposit_over++;
		return OK;
	}
	total += curCx->money;
	currentTime += curCx->tradeTime;
	printf("�� %d λ�ͻ��� %d ʱ�䵽� %d ʱ����ɴ���뿪��\n",curCx->num,curCx->arriveTime,currentTime);
	Q1.front = curCx->next;
	deposit_suc++;
	leave_customer(curCx);
	lastType = 1;
//	printf("���������\n");
	return OK;
}
//����ȡ��ͻ� 
Status withdraw(customer *curCx){
//	printf("����ȡ�ʼ\n");
//	���µ�ǰʱ�� 
	if(currentTime < curCx->arriveTime) currentTime = curCx->arriveTime;
	if( total + curCx->money >= 0 ){
//		printf("����ȡ��������\n");
//		�����ʱ��ʧ�� 
		if(currentTime + curCx->tradeTime > closeTime) {
		currentTime += curCx->tradeTime;
		printf("�� %d λ�ͻ��� %d ʱ�䵽� �� %d ʱ�䳬ʱȡ������뿪��\n",curCx->num,curCx->arriveTime,currentTime);
		withdraw_over++;
		return OK;
	}
		total += curCx->money;
		currentTime += curCx->tradeTime;
		printf("�� %d λ�ͻ��� %d ʱ�䵽� %d ʱ�����ȡ���뿪��\n",curCx->num,curCx->arriveTime,currentTime);
		withdraw_suc++;
		leave_customer(curCx);
	}
	else{
//		printf("ȡ��ʧ�ܵ������"); 
		printf("�� %d λ�ͻ��� %d ʱ�䵽�ȡ��ʧ�ܣ�����Q2�ȴ���\n",curCx->num,currentTime);
		push_customer(Q2,curCx);
	}
//	printf("����ȡ�����\n");
	return OK;
} 

Status check_customer(){
//	printf("���Q2��ʼ\n");
	if(is_empty(Q2)){
		return ERROR;
	}
	customer *p = Q2.front;
	while( p && total + p->money >=0){
		
		Q2.front = p->next;
		withdraw(p);
		p=Q2.front;
	}
	while( p && p->next){
		if(total + p->next->money >=0 ){
			customer* temp = p->next->next;
//			if(!p->next) printf("�˿Ͳ�����2222\n");
			withdraw(p->next);
			p->next = temp;
		}
		p=p->next;
	}
	lastType = 0;
//	printf("���Q2����\n");
	return OK;
}
// ����ǰ�ͻ�ҵ�� 
Status deal_customer(){
//	printf("����ͻ���ʼ\n");
    customer *curCx = NULL;
    if(lastType == 0){  // �����һ��ִ�е���ȡ����� 
        if (pop_customer(Q1, &curCx) == OK) {  // �Ȼ�ȡ����1�еĿͻ������ܿ��Դ�
	        if( curCx->money >= 0 ){  // ����Ǵ�� 
	            deposit(curCx); 
	        }
	        else{  // �����ȡ�� 
	            withdraw(curCx);
	        }
        }
        else {  // �������1��û�пͻ����������2�еĿͻ�
        	currentTime = closeTime;
        }
    }
    else {  // �����һ��ִ�е��Ǵ����������ȼ�����2�еĿͻ�
        if(check_customer() == ERROR) lastType = 0;
    }
//    printf("����ͻ�������\n");
    return OK;
}

 

int main(){
	srand((unsigned)time(NULL));
//	printf("��������Ӫʱ�䣺");
//	scanf(" %d ",&closeTime);
//	scanf("")
	init_queue(Q1);
	init_queue(Q1);
total = 1000;	// �����ִ��ʽ��ܶ� 
closeTime = 600;	//  ����Ӫҵʱ�� 
currentTime = 0;	//��ǰʱ��  
minDealTime = 3;	// ��С����ʱ��
maxDealTime = 300;	// �����ʱ��
minInterval = 5;	// ��С���ʱ��
maxInterval = 120;	// �����ʱ��
maxMoney = 10000;		// ����׽�� 
	
	
	while(currentTime < closeTime)	{
		arrive_customer();
	}
	currentTime = 0;
	
	while((!is_empty(Q1) || !is_empty(Q2)) && (currentTime < closeTime )){
//		printf("%d\t%d\t%d\n",currentTime,closeTime,(currentTime < closeTime ));
		deal_customer();
	}
	
	currentTime = closeTime;
	printf("\n");
	printf("δ��ɽ��׵Ŀͻ��б�\n");
	if(is_empty(Q1)&&is_empty(Q2)) printf("�ޡ�");
	while(!is_empty(Q2)){
		customer *curCx = NULL;
		pop_customer(Q2,&curCx);
		if(curCx){
			printf("��% d λ�ͻ��� %d ʱ�䵽� %d ʱ���뿪��\n",curCx->num,curCx->arriveTime,currentTime);
			leave_customer(curCx);
		}
	}
	while(!is_empty(Q1)){
		customer *curCx = NULL;
		pop_customer(Q1,&curCx);
		if(curCx){
			printf("��% d λ�ͻ��� %d ʱ�䵽� %d ʱ���뿪��\n",curCx->num,curCx->arriveTime,currentTime);
			leave_customer(curCx);
		}
		
	}
	printf("\n");
	
	printf("���н�ҵ��%d\n",total);
	printf("�˾��ȴ�ʱ����%d\n",waitingTime/num);
	printf("��Ҫ����������%d\n",deposit_num);
	printf("��Ҫȡ���������%d\n",withdraw_num);
	printf("���ɹ�������(��������ʱ����)��%d\n",deposit_suc);
	printf("ȡ��ɹ�������(��������ʱ����)��%d\n",withdraw_suc);
	printf("��ʱ��������%d\n",deposit_over);
	printf("ȡ�ʱ��������%d\n",withdraw_over);
	destroy_queue(Q1);
	destroy_queue(Q2);
	
	return 0;
}
