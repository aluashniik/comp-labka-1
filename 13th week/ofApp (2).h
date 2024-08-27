#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include <deque>
#include <vector>

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool readFile();
	void freeMemory();
	bool DFS();
	void dfsdraw();

	int HEIGHT;//�̷��� ����
	int WIDTH;//�̷��� �ʺ�
	char** input;//�ؽ�Ʈ ������ ��� ������ ��� ������ �迭�̴�.
	int** visited;//�湮���θ� ������ ����
	int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
	int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
	int k;
	int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
	int isDFS;//DFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
	int isBFS;//BFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
	// Menu
	ofxWinMenu* menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	// Used by example app
	ofTrueTypeFont myFont;
	ofImage myImage;
	float windowWidth, windowHeight;
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;
	bool bTopmost;
	bool isdfs;
	bool isOpen;
	// Example functions
	//deque<int> path;

	std::deque<std::string> inpt; // Input maze from file
	std::deque<std::pair<int, int>> root; // DFS path
	//static const int HEIGHT = 200; // Maximum maze height
	//static const int WIDTH = 200; // Maximum maze width
	int width, height;

	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);
	typedef struct stack_DFS {
		int top;
		int* stack_arr;
		int width, height;

		stack_DFS() {
			top = 0;
			height = 0;
			width = 0;
			stack_arr = NULL;
		}

		stack_DFS(int in_height, int in_width) {
			top = 0;
			height = in_height;
			width = in_width;
			stack_arr = (int*)malloc(sizeof(int) * in_height * in_width);
		}

		void init(int in_height, int in_width) {
			top = 0;
			height = in_height;
			width = in_width;
			stack_arr = (int*)malloc(sizeof(int) * in_height * in_width);

			for (int i = 0; i < height * width; i++) {
				stack_arr[i] = -1;
			}
		}

		void push(int ele) {
			stack_arr[top++] = ele;
		}

		int pop() {
			return stack_arr[--top];
		}

		bool is_empty() {
			return top == 0;
		}

		int top_ele() {
			return stack_arr[top - 1];
		}

		int num_ele() {
			return top;
		}
	} Stack_DFS;

};