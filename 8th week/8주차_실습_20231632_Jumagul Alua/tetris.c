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
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
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
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
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

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9,WIDTH+10);
	printw("SCORE");
	DrawBox(10,WIDTH+10,1,8);
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
	move(11,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
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
	//바뀌기 전 현재 블록의 정보를 얻어서 switch문 사용
	int pre_blX=blockX, pre_blY=blockY, pre_Rot=blockRotate;
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
	}
	//아전에 그려진 현재 블록을 화면에서 지우기
	for(int i=0; i<BLOCK_HEIGHT; i++) {
		for(int j=0; j<BLOCK_WIDTH; j++) {
			if(block[currentBlock][pre_Rot][i][j]==1 && pre_blY>=0) {
				move(i+pre_blY+1, j+pre_blX+1);
				printw(".");
			}
		}
	}
	//현재 블록을 화면에 그려주기
	DrawBlock(blockY, blockX, nextBlock[0], blockRotate,' ');

	//커서를 필드 밖으로 이동하기
	move(HEIGHT, WIDTH+10);

}

void BlockDown(int sig){
	//checktomove 함수를 호출하여 내려갈 수 있는지 확인
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)) {
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
		
	}
	else {
		//블록의 y좌표가 -1일 경우
		AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		if (blockY== -1){ 
			gameOver=1;
		}
		else {
			//블록을 field에 합치기
			AddBlockToField(field,nextBlock[0], blockRotate, blockY, blockX);
			
			//line을 지우고 score를 업데이트하기
			score+=DeleteLine(field);
			
			for(int i=0; i<2; i++) 
				nextBlock[i]=nextBlock[i+1];
			//nextBlock[0]=nextBlock[1];
			//nextBlock[1]=nextBlock[2];
			//nextBlock[2]=rand()%7;
			nextBlock[2]=rand()%7;
			DrawNextBlock(nextBlock);

            //초기화
			blockY=-1;
			blockX=WIDTH/2-2;
			blockRotate=0;

            //Next 블록을 Next 블록 box안에 그려주고 갱신된 score을 화면에 출력
			//DrawNextBlock(nextBlock);
			PrintScore(score);

		    //화면에 출력
			//DrawField();
		}
		DrawField();
	}
	timed_out=0; //play에서 timed_out이 0일 때 호출됨
}

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	//블록의 정보를 field 배열에 추가하기
	 for(int i=0; i<4; i++){
		 for(int j=0; j<4; j++) {
			if(block[currentBlock][blockRotate][i][j]==1) {
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

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
}

void createRankList() {
	//user code
}

void rank() {
	//user code
}

void writeRankFile() {
	//user code
}

void newRank(int score) {
	//suer code
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