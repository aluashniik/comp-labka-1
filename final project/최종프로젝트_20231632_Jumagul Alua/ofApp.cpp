#include "ofApp.h"
#include <queue>
#include <vector>
#include <unordered_set>

using namespace std;

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);//프레임 속도를 60으로 설정
    gridSize = 20;//그리드의 크기 20으로 설정
    cellSize = 20;//픽셀 단위로 각 셀의 크기 설정
    gameWidth = gridSize * cellSize;//calculate the width of the game area
    gameHeight = gridSize * cellSize;//calculate the height of the game area
    ofSetWindowShape(gameWidth + 200, gameHeight + 100);//윈도우 크기 설정, 게임 영역보다 크게

    //updateInterval = 200;//interval between updates in milliseconds
    gameState = START_SCREEN;//to start with the start screen
    startButton.set(ofGetWidth() / 2 - 50, ofGetHeight() / 2 - 20, 100, 40);//시작 버튼의 위치와 크기 설정

    //속도 선택 버튼 위치랑 크기, 빠른 중간 느린 걸로 3개 선택지 있음
    fastButton.set(ofGetWidth() / 2 - 50, ofGetHeight() / 2 - 60, 100, 40);
    mediumButton.set(ofGetWidth() / 2 - 50, ofGetHeight() / 2, 100, 40);
    slowButton.set(ofGetWidth() / 2 - 50, ofGetHeight() / 2 + 60, 100, 40);
}

//--------------------------------------------------------------
void ofApp::resetGame() {
    snake.clear();//clear the snake vector
    snake.push_back(ofVec2f(gridSize / 2, gridSize / 2));//뱀이 증심부터 움직이게 설정 
    direction = RIGHT;//오른쪽에 방향을 두기
    placeTarget();//place the target randomly 
    gameState = PLAYING;//게임 하는 중으로 설정
    score = 0;//점수를 0으로 초기화
    lastUpdate = ofGetElapsedTimeMillis();//마지막 업데이트 시간을 현재 시간으로 설정
}

//--------------------------------------------------------------
void ofApp::placeTarget() {
    int x, y;
    do {//목표를 뱀과 겹치지 않게 랜덤한 위치에 배치
        x = static_cast<int>(ofRandom(gridSize));//랜덤하게 x 좌표 설정
        y = static_cast<int>(ofRandom(gridSize));//랜덤하게 y 좌표 설정
    } while (check(x, y));//뱀과 겹치지 않을때까지
    target.set(x, y);//목표 위치 설정
}

bool ofApp::check(int x, int y) {
    for (size_t i = 1; i < snake.size(); i++) {  // Start from 1 to skip the head
        if (static_cast<int>(snake[i].x) == x && static_cast<int>(snake[i].y) == y) {//위치가 뱀의 몸과 겹치는지 확인하는 함수
            return true; //겹치면 true 반환
        }
    }
    return false; //겹치지 않으면 false 반환
}


vector<ofVec2f> ofApp::BFS(ofVec2f start, ofVec2f target, int gridSize) {
    queue<ofVec2f> queue; //탐색을 위한 큐
    unordered_set<int> visited; //방문한 노드를 추적

    //방향 벡터 (up, down, right, left)
    vector<ofVec2f> directions = { ofVec2f(0, -1), ofVec2f(0, 1), ofVec2f(-1, 0), ofVec2f(1, 0) };

    //큐 초기화
    queue.push(start);//시작 위를 큐에 추가
    visited.insert(start.x + start.y * gridSize);//방문한 것으로 표시

    unordered_map<int, ofVec2f> prev; //각 노드의 이전 노드를 기록하여 경로 재구성라기 위한 맵

    while (!queue.empty()) {//큐가 비어있지 않은 동안
        ofVec2f current = queue.front();//현재 노드를 가져옴
        queue.pop();//큐에서 현재 노드를 제거

        if (current == target) {//목표물에 도달했는지 확인
            vector<ofVec2f> path;//경로를 저장할 벡터
            for (ofVec2f at = target; at != start; at = prev[at.x + at.y * gridSize]) {//목표물에서 시작 위치까지 경로를 추적
                path.push_back(at);//경로에 현재 노드를 추가
            }
            path.push_back(start);//시작 위치를 경로에 추가
            reverse(path.begin(), path.end()); //경로를 뒤집어 올바른 순서로 만듦
            return path;//찾은 경로를 반환
        }

        //네 방향으로 이동
        for (const auto& direction : directions) {
            ofVec2f next = current + direction; //현재 위치에서 다음 위치로 이동
          //next.x = <float>((<int>(next.x) + gridSize) % gridSize);
         // next.y = <float>((<int>(next.y) + gridSize) % gridSize);

            next.x = static_cast<float>((static_cast<int>(next.x) + gridSize) % gridSize); //그리드 밖으로 나가면 반대편으로 이동
            next.y = static_cast<float>((static_cast<int>(next.y) + gridSize) % gridSize);  //그리드 밖으로 나가면 반대편으로 이동

            //방문하지 않은 위치만 탐색
            int nextkey = static_cast<int>(next.x) + static_cast<int>(next.y) * gridSize;
            if (visited.find(nextkey) == visited.end() && !check(static_cast<int>(next.x), static_cast<int>(next.y))) {
                queue.push(next);//다음 위치를 큐에 추가
                visited.insert(static_cast<int>(next.x) + static_cast<int>(next.y) * gridSize);//다음 위치를 방문한 것으로 표시
                prev[static_cast<int>(next.x) + static_cast<int>(next.y) * gridSize] = current; //다음 위치의 이전 노드를 현재 위치로 설정
            }
        }
    }

    // 경로를 찾지 못했을 때 빈 벡터 반환
    return vector<ofVec2f>();
}

void ofApp::update() {
    if (gameState != PLAYING)
        return;

    unsigned long currentTime = ofGetElapsedTimeMillis();
    if (currentTime - lastUpdate < updateInterval)
        return;

    lastUpdate = currentTime;
    //BFS를 사용하여 목표물까지의 경로를 찾기
    vector<ofVec2f> path = BFS(snake.front(), target, gridSize);

    if (!path.empty() && path.size() > 1) {
        ofVec2f nextPosition = path[1]; //[0]은 현재 위치이므로 [1]이 다음 위치
        //현재 위치가 뱀의 몸과 겹치는지 확인
        if (check(nextPosition.x, nextPosition.y)) {
            gameState = GAME_OVER;
            return;
        }

        snake.push_front(nextPosition);
        // 목표물을 먹었는지 확인
        if (nextPosition == target) {
            placeTarget();
            score++;
        }
        else {
            snake.pop_back();
        }
    }
    else { //경로를 찾지 못했을 때의 처리
        ofVec2f head = snake.front();
        ofVec2f nextPosition;

        switch (direction) {
        case UP:    
            nextPosition = head + ofVec2f(0, -1); 
            break;
        case DOWN:  
            nextPosition = head + ofVec2f(0, 1);  
            break;
        case LEFT:  
            nextPosition = head + ofVec2f(-1, 0); 
            break;
        case RIGHT: 
            nextPosition = head + ofVec2f(1, 0);  
            break;
        }

        nextPosition.x = fmod(nextPosition.x + gridSize, gridSize);
        nextPosition.y = fmod(nextPosition.y + gridSize, gridSize);

        if (check(nextPosition.x, nextPosition.y)) {
            gameState = GAME_OVER;
            return;
        }

        snake.push_front(nextPosition);
        snake.pop_back();
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);//배경을 검정색으로 설정

    if (gameState == START_SCREEN) {//초기화면에서
        ofSetColor(255, 0, 0);//빨간색으로
        ofDrawBitmapString("! Snake Game !", ofGetWidth() / 2 - 50, ofGetHeight() / 2 - 40);//텍스트 그리기
        ofSetColor(255, 0, 0);//버튼도 빨간색으로
        ofDrawRectangle(startButton);//시작 버튼 그리기
        ofSetColor(255);//안에 있는 텍스트 횐색으로
        ofDrawBitmapString("Start", startButton.x + 30, startButton.y + 25);//버튼 텍스트
    }
    else if (gameState == SPEED_SELECTION) {//속도 선택로 화면 그리기
        ofSetColor(255, 0, 0);//빨간색으로 설정
        ofDrawBitmapString("Select Speed:", ofGetWidth() / 2 - 50, ofGetHeight() / 2 - 100);

        ofSetColor(255, 0, 0);//빨간색으로
        ofDrawRectangle(fastButton);//빠른 속도로 움직이게끔
        ofDrawBitmapString("Fast", fastButton.x + 30, fastButton.y + 25);//1번째 순서에 놓기

        ofSetColor(255, 153, 0);//주황색으로
        ofDrawRectangle(mediumButton);//중간 속도로
        ofDrawBitmapString("Medium", mediumButton.x + 20, mediumButton.y + 25);//중간에 놓기

        ofSetColor(255, 255, 204);//노란색으로
        ofDrawRectangle(slowButton);//느린 속도로
        ofDrawBitmapString("Slow", slowButton.x + 30, slowButton.y + 25);//3번재로 넣기
    }
    else if (gameState == PLAYING) {//게임 플레이 중 상태에서
        ofPushMatrix(); //행령에 저장
        ofTranslate(100, 50);//그리기 영역을 게임 영역으로 이동

        ofSetColor(100);//set color for the game area border
        ofNoFill();//the border of the game area
        ofDrawRectangle(0, 0, gameWidth, gameHeight);
        ofFill();

        ofSetColor(0, 255, 0);//setting green color for the snake
        for (const auto& segment : snake) {
            ofDrawRectangle(segment.x * cellSize, segment.y * cellSize, cellSize, cellSize);
        }

        ofSetColor(255, 0, 0);//setting red color for the target
        ofDrawRectangle(target.x * cellSize, target.y * cellSize, cellSize, cellSize);//draw the target

        ofPopMatrix();//restore the prev matrix

        ofSetColor(255);//score text
        ofDrawBitmapString("Score: " + ofToString(score), 110, 30);
    }
    else if (gameState == GAME_OVER) {//게임이 종료될때
        ofSetColor(255, 0, 0);//빨간색으로 게임 종려 텍스트 출력
        ofDrawBitmapString("Game Over!", ofGetWidth() / 2 - 60, ofGetHeight() / 2 - 40);

        ofSetColor(255);//나머지 텍스트 횐색으로 하나씩 나타내기
        ofDrawBitmapString("Final Score: " + ofToString(score), ofGetWidth() / 2 - 50, ofGetHeight() / 2 - 20);//최종 점수
        ofDrawBitmapString("Press 'R' to Restart", ofGetWidth() / 2 - 50, ofGetHeight() / 2);//R 누르면 다시 시작
        ofDrawBitmapString("Press 'Q' to Quit", ofGetWidth() / 2 - 50, ofGetHeight() / 2 + 20);//Q 누르면 게임 완전히 나가기
    }
    else if (gameState == PAUSED) {//제임이 일시 정지 상태이면
        ofSetColor(255);//횐색으로
        ofDrawBitmapString("PAUSED", ofGetWidth() / 2 - 30, ofGetHeight() / 2);//텍스트 출력
        ofDrawBitmapString("Press 'P' to resume", ofGetWidth() / 2 - 70, ofGetHeight() / 2 + 20);//P 누르면 다시 게임 계속
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (gameState == GAME_OVER) {
        if (key == 'q' || key == 'Q') {
            ofExit(); //exit the application
        }
        else if (key == 'r' || key == 'R') {
            resetGame();//restart the game
        }
    } 
    if (gameState == PLAYING) {
        if (key == 'p' || key == 'P') {
            gameState = PAUSED; // 게임 일시 정지 기능
        }
    } else if (gameState == PAUSED) {
        if (key == 'p' || key == 'P') {
            gameState = PLAYING; // 게임 재개
        }
    }
}

void ofApp::mousePressed(int x, int y, int button) {
    if (gameState == START_SCREEN && startButton.inside(x, y)) {//시작 버튼 클릭 할 떄 속도 선택 화면이 나옴
        gameState = SPEED_SELECTION;
    }
    else if (gameState == SPEED_SELECTION) {
        if (fastButton.inside(x, y)) {
            updateInterval = 100; //빠른 속도
            resetGame();
        }
        else if (mediumButton.inside(x, y)) {
            updateInterval = 200; //중간 속도
            resetGame();
        }
        else if (slowButton.inside(x, y)) {
            updateInterval = 400; //느린 속도
            resetGame();
        } //숫자가 작아질수록 게임이 느려짐
    }
}

