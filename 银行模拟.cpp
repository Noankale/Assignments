#include<stdio.h>
#include<stdlib.h>
#include<time.h>

typedef enum{OK,ERROR,OVERFLOW} Status;

typedef struct customer{	// 顾客 
	int type;	// 状态， 0到达1离开 
	int money;	// 金额，正值存款负值取款 
	int arriveTime; // 到达时间
	int tradeTime; // 交易时间  
	int num;	// 编号 
	customer* next;
}customer; 

typedef struct queue{	//队列 
	customer* rear;
	customer* front;
}queue;

int total = 0;	// 银行现存资金总额 
int closeTime = 0;	//  结束营业时间 
int currentTime = 0;	//当前时间  
int minDealTime = 0;	// 最小处理时间
int maxDealTime = 0;	// 最大处理时间
int minInterval = 0;	// 最小间隔时间
int maxInterval = 0;	// 最大间隔时间
int maxMoney = 0;		// 最大交易金额 
int num = 1;	// 客户数 
int lastType = 0;	// 上一位客户事件为,0取款1存款 
queue Q1;	// 队列1
queue Q2;	// 队列2

int waitingTime = 0;	
int deposit_num = 0;
int deposit_suc = 0;
int deposit_over = 0;
int withdraw_num = 0;
int withdraw_suc = 0;
int withdraw_over = 0;



// 初始化队列
Status init_queue(queue &q){
	q.front = q.rear = NULL;
	return OK;
} 

// 获得首位顾客 
customer* get_front(queue &q){
	return q.front;
}

// 顾客入队
Status push_customer(queue &q, customer* x) {
    if (!x) return ERROR;
    
    if (!q.front) {
        q.front = q.rear = x;
        x->next = NULL;
    } else {
        q.rear->next = x;
        q.rear = x;
        x->next = NULL;  // 确保新节点的 next 指向 NULL
    }
    return OK;
}
// 顾客出队
Status pop_customer(queue &q, customer **x) {
//	printf("顾客出队开始\n");
    if (!q.front){
//    	printf("队列中客户不存在。\t\t");
    	return ERROR;
	}
    *x = q.front;
    q.front = q.front->next;
    if (!q.front) {
        q.rear = NULL;
    }
//    printf("顾客出队结束\n");
    return OK;
}
//销毁队列 
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

// 生成到达客户队列 
Status arrive_customer(){
	// 创建客户 
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
//	printf("客户 %d 在时间\t%d\t到达,交易时间\t%d\t金额为\t%d\t。\n",x->num,x->arriveTime,x->tradeTime,x->money);
	// 下一个顾客的到达时间 
	currentTime += minInterval + rand() % ( maxInterval - minInterval + 1); 
	return OK;
} 
//离开
Status leave_customer(customer *p){
	waitingTime += currentTime - p->arriveTime;
	free(p);
	return OK; 
} 
//处理存款客户 
Status deposit (customer *curCx){
//	printf("处理存款开始\n");
//	更新当前时间 
	if(currentTime < curCx->arriveTime) currentTime = curCx->arriveTime;
	if(currentTime + curCx->tradeTime > closeTime) {
		currentTime += curCx->tradeTime;
		printf("第 %d 位客户在 %d 时间到达， 在 %d 时间超时存款完成离开。\n",curCx->num,curCx->arriveTime,currentTime);
		deposit_over++;
		return OK;
	}
	total += curCx->money;
	currentTime += curCx->tradeTime;
	printf("第 %d 位客户在 %d 时间到达， %d 时间完成存款离开。\n",curCx->num,curCx->arriveTime,currentTime);
	Q1.front = curCx->next;
	deposit_suc++;
	leave_customer(curCx);
	lastType = 1;
//	printf("处理存款结束\n");
	return OK;
}
//处理取款客户 
Status withdraw(customer *curCx){
//	printf("处理取款开始\n");
//	更新当前时间 
	if(currentTime < curCx->arriveTime) currentTime = curCx->arriveTime;
	if( total + curCx->money >= 0 ){
//		printf("可以取款的情况下\n");
//		如果超时则失败 
		if(currentTime + curCx->tradeTime > closeTime) {
		currentTime += curCx->tradeTime;
		printf("第 %d 位客户在 %d 时间到达， 在 %d 时间超时取款完成离开。\n",curCx->num,curCx->arriveTime,currentTime);
		withdraw_over++;
		return OK;
	}
		total += curCx->money;
		currentTime += curCx->tradeTime;
		printf("第 %d 位客户在 %d 时间到达， %d 时间完成取款离开。\n",curCx->num,curCx->arriveTime,currentTime);
		withdraw_suc++;
		leave_customer(curCx);
	}
	else{
//		printf("取款失败的情况下"); 
		printf("第 %d 位客户在 %d 时间到达，取款失败，进入Q2等待。\n",curCx->num,currentTime);
		push_customer(Q2,curCx);
	}
//	printf("处理取款结束\n");
	return OK;
} 

Status check_customer(){
//	printf("检查Q2开始\n");
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
//			if(!p->next) printf("顾客不存在2222\n");
			withdraw(p->next);
			p->next = temp;
		}
		p=p->next;
	}
	lastType = 0;
//	printf("检查Q2结束\n");
	return OK;
}
// 处理当前客户业务 
Status deal_customer(){
//	printf("处理客户开始\n");
    customer *curCx = NULL;
    if(lastType == 0){  // 如果上一次执行的是取款操作 
        if (pop_customer(Q1, &curCx) == OK) {  // 先获取队列1中的客户（可能可以存款）
	        if( curCx->money >= 0 ){  // 如果是存款 
	            deposit(curCx); 
	        }
	        else{  // 如果是取款 
	            withdraw(curCx);
	        }
        }
        else {  // 如果队列1中没有客户，则检查队列2中的客户
        	currentTime = closeTime;
        }
    }
    else {  // 如果上一次执行的是存款操作，则先检查队列2中的客户
        if(check_customer() == ERROR) lastType = 0;
    }
//    printf("处理客户结束。\n");
    return OK;
}

 

int main(){
	srand((unsigned)time(NULL));
//	printf("请输入运营时间：");
//	scanf(" %d ",&closeTime);
//	scanf("")
	init_queue(Q1);
	init_queue(Q1);
total = 1000;	// 银行现存资金总额 
closeTime = 600;	//  结束营业时间 
currentTime = 0;	//当前时间  
minDealTime = 3;	// 最小处理时间
maxDealTime = 300;	// 最大处理时间
minInterval = 5;	// 最小间隔时间
maxInterval = 120;	// 最大间隔时间
maxMoney = 10000;		// 最大交易金额 
	
	
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
	printf("未完成交易的客户列表：\n");
	if(is_empty(Q1)&&is_empty(Q2)) printf("无。");
	while(!is_empty(Q2)){
		customer *curCx = NULL;
		pop_customer(Q2,&curCx);
		if(curCx){
			printf("第% d 位客户在 %d 时间到达， %d 时间离开。\n",curCx->num,curCx->arriveTime,currentTime);
			leave_customer(curCx);
		}
	}
	while(!is_empty(Q1)){
		customer *curCx = NULL;
		pop_customer(Q1,&curCx);
		if(curCx){
			printf("第% d 位客户在 %d 时间到达， %d 时间离开。\n",curCx->num,curCx->arriveTime,currentTime);
			leave_customer(curCx);
		}
		
	}
	printf("\n");
	
	printf("银行结业余额：%d\n",total);
	printf("人均等待时长：%d\n",waitingTime/num);
	printf("需要存款的人数：%d\n",deposit_num);
	printf("需要取款的人数：%d\n",withdraw_num);
	printf("存款成功的人数(不包含超时人数)：%d\n",deposit_suc);
	printf("取款成功的人数(不包含超时人数)：%d\n",withdraw_suc);
	printf("存款超时的人数：%d\n",deposit_over);
	printf("取款超时的人数：%d\n",withdraw_over);
	destroy_queue(Q1);
	destroy_queue(Q2);
	
	return 0;
}
