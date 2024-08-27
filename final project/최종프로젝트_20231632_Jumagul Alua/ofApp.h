#pragma once

#include "ofMain.h"
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

using namespace std;

enum GameState {//게임 상태를 나타남
    START_SCREEN,//시작 화면
    SPEED_SELECTION,//속도 선택 화면
    PLAYING,//게임 중인 상태
    PAUSED, //게임 일지 정지
    GAME_OVER,//게임 종료 상태
    RESTART_OPTION //재시작 옵션 
};

enum Direction {//뱀의 이동 방향
    UP,//위쪽
    DOWN,//아래족
    LEFT,//왼쪽
    RIGHT//오른쪽
};

class ofApp : public ofBaseApp {
public:
    void setup();//초기 설정
    void update();//업데이트
    void draw();//화면에 그리기
    void keyPressed(int key);//키 입력
    void mousePressed(int x, int y, int button);//마우스 클릭

private:
    void resetGame();//게임 재설정
    void placeTarget();//목표물의 위치
    bool check(int x, int y); //설정될 위치가 뱀과 겹치는 지 확인

    vector<ofVec2f> BFS(ofVec2f start, ofVec2f target, int gridSize); // BFS를 사용하여 경로 찾기

    // Game parameters
    int gridSize;//그리드의 크기
    int cellSize;//각 셀의 크기
    int gameWidth;//게임 영역의 너비
    int gameHeight;//게임 영역의 높이
    int updateInterval;//게임 업데이트 간격
    unsigned long lastUpdate;//마지막 업데이트 시간

    deque<ofVec2f> snake; //뱀의 몸을 나타내는 덱, (from vector to deque)
    ofVec2f target;//목표무르이 위치
    Direction direction;//뱀의 이동 방향
    int score;///현재 점수

    GameState gameState; //현재 게임 상태
    ofRectangle startButton; //시작 버튼의 위치와 크기
    ofRectangle fastButton;//빠른 속도 버튼의 위치와 크기
    ofRectangle mediumButton;//중간 속도 버튼의 위치와 크기
    ofRectangle slowButton;//느린 속도 버튼의 위치와 크기
};
