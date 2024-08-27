#include "tetris.h"
#include <unistd.h>


static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	writeRankFile();
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawNextBlock(nextBlock);
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}



void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		//for the second block
		move(10+i, WIDTH+13);
		for( j=0; j<4; j++) {
			if(block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}



void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);
	
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j] == 1) {
				int blY=i+blockY, blX=j+blockX;
				//field 벗어나지 않는지 확인
				if(blY>=HEIGHT || blX>=WIDTH || blX<0){
					return 0;
				}
				//이미 차있는 곳에 놓이지 않았는지 확인
				if(f[blY][blX]==1) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int pre_blX=blockX, pre_blY=blockY, pre_Rot=blockRotate; //바뀌기 전 현재 블록의 정보를 얻어서 switch문 사용
	switch(command){
		case KEY_UP:
			pre_Rot=(pre_Rot+3)%4;
			break;
		case KEY_DOWN:
			pre_blY -= 1;
			break;
		case KEY_RIGHT:
			pre_blX -= 1;
			break;
		case KEY_LEFT:
			pre_blX += 1;
			break;
		default:
			break;
	}//아전에 그려진 현재 블록을 화면에서 지우기
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][pre_Rot][i][j]==1 && pre_blY>=0) {
				move(i+pre_blY+1, j+pre_blX+1);
				printw(".");
			}
		}
	}//현재 블록을 화면에 그려주기
	DrawBlock(blockY, blockX, nextBlock[0], blockRotate,' '); //현재 블록을 화면에 그려주기
	move(HEIGHT, WIDTH+10); //커서를 필드 밖으로 이동하기
}

void BlockDown(int sig){
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)) {//checktomove 함수를 호출하여 내려갈 수 있는지 확인
		for(int i=0; i<BLOCK_HEIGHT; i++) {
			 for(int j=0; j<BLOCK_WIDTH; j++) {
				if(block[nextBlock[0]][blockRotate][i][j]==1 && blockY+i>=0) {
					move(blockY+i+1, blockX+j+1);
					printw(".");
					}
				}
		}	
		blockY++;//내려갈 수 있으면 y 좌표를 증가시켜주기
		DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');// 한 줄 내린 위치에 다시 그려주기
		
	} else {
		ifsame=0;
		AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX); 
		if (blockY== -1){ //블록의 y좌표가 -1일 경우
			gameOver=1;
		}
		else {
			AddBlockToField(field,nextBlock[0], blockRotate, blockY, blockX); //블록을 field에 합치기
			score+=DeleteLine(field); //line을 지우고 score를 업데이트하기
			
			for(int i=0; i<2; i++) 
				nextBlock[i]=nextBlock[i+1];
			nextBlock[2]=rand()%7;
			DrawNextBlock(nextBlock);
            //초기화
			blockY=-1;
			blockX=WIDTH/2-2;
			blockRotate=0;

			PrintScore(score);
		}
		DrawField(); //화면에 출력
	}
	timed_out=0; //play에서 timed_out이 0일 때 호출됨
}


 //Next 블록을 Next 블록 box안에 그려주고 갱신된 score을 화면에 출력
			//DrawNextBlock(nextBlock);

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, touched=0;
	 for(i=0; i<BLOCK_HEIGHT; i++){
		 for(j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][blockRotate][i][j]==1) {
				if(blockY+i==HEIGHT-1){
					touched++;
				}
				else if(f[blockY+i+1][blockX+j]==1) touched++;
				f[blockY+i][blockX+j]=1;
			}
		}
	}
}



int DeleteLine(char f[HEIGHT][WIDTH]){
	int line_cnt=0;
	//int isFullLine=1;
	for(int i=0; i<HEIGHT; i++) {
		int isFullLine=1;
		for(int j=0; j<WIDTH; j++) {
			if(f[i][j]==0 ){
				isFullLine=0;
				break;
			}
		}//한 줄이 채웠는지 check
		if(isFullLine) {
			line_cnt++;
			//블록의 정보를 한 줄씩 내려주기
			for(int k=i; k>0; k--) {
				for(int l=0; l<WIDTH; l++) {
					f[k][l]=f[k-1][l];
				}
			}
			for(int l=0; l<WIDTH; l++) {
				f[0][l]=0;
			}
		}
	}
	//점수 ((지운 line 개수)^2*100) return
	return line_cnt * line_cnt * 100;
}

///////////////////////////////////////////////////////////////////////////
/*8주차 과제 함수*/

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
    DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
}

void DrawShadow(int y, int x, int blockID, int blockRotate){	
	int i,j;
	if(ifsame!=1) {
		for(shadowY=y; shadowY<=HEIGHT; shadowY++) {
			if(CheckToMove(field, blockID, blockRotate, shadowY+1, x)==0) break;
		}
		DrawBlock(shadowY, x, blockID,blockRotate, '/');
	}
	ifsame=0;
}


/////////////////////////////////////////////////////////////////////////
/*9주차 실습 함수*/
void createRankList(){
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	FILE *fp;
	int i;

	//1. 파일 열기
	fp = fopen("rank.txt", "r");
	if(fp==NULL) {
		return;
	}
	// 2. 정보읽어오기
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문
	if(fscanf(fp, "%d", &score_number)==EOF) {
		score_number=0;
	}
		
	for(i=0; i<score_number; i++) {
		char *playerName=malloc(sizeof(char) * 17);
		int playerScore;

		if (fscanf(fp, "%s %d", playerName, &playerScore) != EOF) {
			Node *newNode = malloc(sizeof(Node));
            strcpy(newNode->name, playerName);
            newNode->score = playerScore;
            newNode->next = NULL;

            if (liststart == NULL) {
                liststart = newNode;
                listend = newNode;
            } else {
                listend->next = newNode;
                listend = listend->next;
            }
		}
		else {
			printw("Error!\n");
			exit(1);
		}
		free(playerName);
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank(){
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	//1. 문자열 초기화
	int X=1, Y=score_number;
	int choice, i, j, count=0;
	clear();

	//메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//get the user's choice
	choice = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (choice == '1') {
		echo();
		printw("X: ");
		scanw("%d", &X);
		noecho();
		echo();
		printw("Y: ");
		scanw("%d", &Y);
		noecho();

		if(Y<=X||Y>score_number||X<1) {
			printw("search failure: no rank in the list");
		}
		else {
			count=0;
			Node *currentNode=liststart;
			printw("      name      |   score\n");
			printw("----------------------------\n");
			for(i=1; i<=Y; i++) {
				if(i>=X) {
					count++;
					printw("%-16s| %d\n", currentNode->name, currentNode->score);
					currentNode = currentNode->next;
				}
				else {
					currentNode = currentNode->next;
				}
			}
		}/*2주차 과제*/
	} else if (choice == '2') { //by a specific name
		char name[NAMELEN+1];
		Node *currentNode = liststart;
		Node *prevNode = NULL;
		//int check = 0;
		int found = 0;

		echo();
		printw("Input the name: ");
		scanw("%s", name);
        noecho();

		printw("      name      |   score\n");
		printw("----------------------------\n");
		//printw("%s", playerName);
		while(currentNode!=NULL){
			if(!strcmp(currentNode->name, name)){
				found=1;
				printw("%-16s| %d\n", currentNode->name, currentNode->score);
				if (prevNode == NULL) {
					liststart = currentNode->next;
				} else {
					prevNode->next = currentNode->next;
            }
            Node* tempNode = currentNode;
            currentNode = currentNode->next;
            free(tempNode);
            score_number--;
        } else {
            prevNode = currentNode;
            currentNode = currentNode->next;
		}
		}
		if(!found){
		printw("search failure: no rank in the list\n");
	}
	} else if (choice == '3') {//delete a specific rank 
		int number;
		echo();
		printw("Input the rank: ");
		scanw("%d", &number);
		noecho();
		if (number<1 || number>score_number){
			printw("\nsearch failure: no rank in the list\n");
		}
		else {
			Node *currentNode = liststart;
		    Node *prevNode = NULL;
			Node *tempNode = NULL;

			for(int i=1; i<number; i++){
				prevNode = currentNode;
				currentNode = currentNode -> next;
			}
			if(prevNode==NULL){
				liststart = currentNode -> next;
			}else{
			    prevNode->next = currentNode->next;
			}
			free(currentNode);
			score_number--;
		}
	}
	getch();

}

void writeRankFile(){
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int scoreCnt, i;
	//"rank.txt" 연다 
	FILE *fp = fopen("rank.txt", "w");

	//2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_number);

	//3. 탐색할 노드가 더 있는지 체크하고 있으면 다음 노드로 이동, 없으면 종료
	Node *currentNode=liststart;
	for(i=0; i<score_number; i++) {
		if(currentNode==NULL) {
			printw("rank file error");
			exit(1);
		}
		fprintf(fp, "%s %d\n", currentNode->name, currentNode->score);
		currentNode = currentNode->next;
	}
	fclose(fp);
}

void newRank(int score){
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char playerName[NAMELEN+1];
	int i, j;
	clear();

	//1. 사용자 이름을 입력받음
	echo();
	printw("your name: ");
	scanw("%s", playerName);
	noecho();

	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	Node *currentNode=liststart;
	Node *newNode=malloc(sizeof(Node));
	strcpy(newNode->name, playerName);
	newNode->score=score;
	newNode->next=NULL;

	//update
	if(liststart==NULL) {
		liststart=newNode;
	}
	else {
		if(currentNode->score < score) {
			newNode->next=liststart;
			liststart=newNode;
		}
		else {
			Node *prevNode=currentNode;
			currentNode = currentNode->next;
			while (prevNode->next != NULL) {
                if (currentNode->score < score) {
                    prevNode->next = newNode;
                    newNode->next = currentNode;
                    break;
                }
                currentNode = currentNode->next;
                prevNode = prevNode->next;
            }
            if (newNode->next == NULL) {
                prevNode->next = newNode;
                listend = newNode;
            }
        }
    }
	score_number++;
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	//user code
}

void recommendedPlay(){
	// user code
}
