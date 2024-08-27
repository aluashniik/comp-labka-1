#include "ofApp.h"
#include <iostream>
#include <deque>
#include <utility>

deque<string> inpt;
using namespace std;

deque<pair<int, int>> root;

#define INDEX(y, x) ((y) * width + (x))
#define INDEX_Y(index) ((index) / width)
#define INDEX_X(index) ((index) % width)

struct Cell {
    bool top;    // 위쪽 벽
    bool right;  // 오른쪽 벽
    bool bottom; // 아래쪽 벽
    bool left;   // 왼쪽 벽
    Cell() : top(false), right(false), bottom(false), left(false) {}
};

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
    ofSetFrameRate(15);
    ofBackground(255, 255, 255);
    // Get the window size for image loading
    windowWidth = ofGetWidth();
    windowHeight = ofGetHeight();
    isdfs = false;
    isOpen = 0;
    // Centre on the screen
    ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

    // Load a font rather than the default
    myFont.load("verdana.ttf", 12, true, true);

    // Window handle used for topmost function
    hWnd = WindowFromDC(wglGetCurrentDC());

    // Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
    ofSetEscapeQuitsApp(false);

    //
    // Create a menu using ofxWinMenu
    //

    // A new menu object with a pointer to this class
    menu = new ofxWinMenu(this, hWnd);

    // Register an ofApp function that is called when a menu item is selected.
    // The function can be called anything but must exist. 
    // See the example "appMenuFunction".
    menu->CreateMenuFunction(&ofApp::appMenuFunction);

    // Create a window menu
    HMENU hMenu = menu->CreateWindowMenu();

    //
    // Create a "File" popup menu
    //
    HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

    //
    // Add popup items to the File menu
    //

    // Open a maze file
    menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

    // Final File popup menu item is "Exit" - add a separator before it
    menu->AddPopupSeparator(hPopup);
    menu->AddPopupItem(hPopup, "Exit", false, false);

    //
    // View popup menu
    //
    hPopup = menu->AddPopupMenu(hMenu, "View");

    bShowInfo = true;  // screen info display on
    menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
    bTopmost = false; // app is topmost
    menu->AddPopupItem(hPopup, "Show BFS"); // Not checked (default)
    bFullscreen = false; // not fullscreen yet
    menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

    //
    // Help popup menu
    //
    hPopup = menu->AddPopupMenu(hMenu, "Help");
    menu->AddPopupItem(hPopup, "About", false, false); // No auto check

    // Set the menu to the window
    menu->SetWindowMenu();
} // end Setup

//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {
    ofFileDialogResult result;
    string filePath;
    size_t pos;

    //
    // File menu
    //
    if (title == "Open") {
        readFile();
    }
    if (title == "Exit") {
        ofExit(); // Quit the application
    }

    //
    // Window menu
    //
    if (title == "Show DFS") {
        if (isOpen) {
            isdfs = DFS();  // DFS를 호출하고 결과를 isdfs에 저장
            printf("DFS Executed\n");
            bShowInfo = bChecked;
        }
        else {
            cout << "You must open file first" << endl;
        }
    }

    if (title == "Show BFS") {
        doTopmost(bChecked); // Use the checked value directly
    }

    if (title == "Full screen") {
        bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
        doFullScreen(bFullscreen); // But also take action immediately
    }

    //
    // Help menu
    //
    if (title == "About") {
        ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
    }

} // end appMenuFunction

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
    char str[256];
    //ofBackground(0, 0, 0, 0);
    ofSetColor(100);
    ofSetLineWidth(5);

    ///////////////////////////////////////////////////////////////////////

    height = inpt.size();
    if (height == 0) {
        return;
    }
    width = inpt[0].size();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char cell = inpt[i][j];
            int drawX = j * 10;
            int drawY = i * 10;
            switch (cell) {
            case '-':
                ofDrawLine(drawX - 10, drawY, drawX + 10, drawY);
                break;
            case '|':
                ofDrawLine(drawX, drawY - 10, drawX, drawY + 10);
                break;
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////

    if (isdfs) {
        ofSetColor(255, 0, 0); // 경로를 빨간색으로 그리기
        ofSetLineWidth(5);
        if (isOpen)
            dfsdraw();
        else
            cout << "You must open file first" << endl;
    }
    if (bShowInfo) {
        // Show keyboard duplicates of menu functions
        sprintf(str, " comsil project");
        myFont.drawString(str, 15, ofGetHeight() - 20);
    }
} // end Draw

void ofApp::doFullScreen(bool bFull) {
    // Enter full screen
    if (bFull) {
        // Remove the menu but don't destroy it
        menu->RemoveWindowMenu();
        // hide the cursor
        ofHideCursor();
        // Set full screen
        ofSetFullscreen(true);
    }
    else {
        // return from full screen
        ofSetFullscreen(false);
        // Restore the menu
        menu->SetWindowMenu();
        // Restore the window size allowing for the menu
        ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
        // Centre on the screen
        ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
        // Show the cursor again
        ofShowCursor();
        // Restore topmost state
        if (bTopmost) doTopmost(true);
    }
} // end doFullScreen

void ofApp::doTopmost(bool bTop) {
    if (bTop) {
        // get the current top window for return
        hWndForeground = GetForegroundWindow();
        // Set this window topmost
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOW);
    }
    else {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(hWnd, SW_SHOW);
        // Reset the window that was topmost before
        if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
            SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        else
            SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
} // end doTopmost

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

    // Escape key exit has been disabled but it can be checked here
    if (key == VK_ESCAPE) {
        // Disable fullscreen set, otherwise quit the application as usual
        if (bFullscreen) {
            bFullscreen = false;
            doFullScreen(false);
        }
        else {
            ofExit();
        }
    }

    // Remove or show screen info
    if (key == ' ') {
        bShowInfo = !bShowInfo;
        // Update the menu check mark because the item state has been changed here
        menu->SetPopupItem("Show DFS", bShowInfo);
    }

    if (key == 'f') {
        bFullscreen = !bFullscreen;
        doFullScreen(bFullscreen);
        // Do not check this menu item
        // If there is no menu when you call the SetPopupItem function it will crash
    }
    if (key == 'd') {
        if (isOpen) {
            if (!isdfs) {
                isdfs = DFS();
            }
        }
        else {
            cout << "You must open file first" << endl;
        }
    }
} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

bool ofApp::readFile() {
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file");
    string filePath;
    size_t pos;
    // Check whether the user opened a file
    if (openFileResult.bSuccess) {
        ofLogVerbose("User selected a file");

        // We have a file, check it and process it
        string fileName = openFileResult.getName();
        // string fileName = "maze0.maz";
        printf("file name is %s\n", fileName.c_str());
        filePath = openFileResult.getPath();
        printf("Open\n");
        pos = filePath.find_last_of(".");
        if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

            ofFile file(fileName);

            if (!file.exists()) {
                cout << "Target file does not exists." << endl;
                return false;
            }
            else {
                cout << "We found the target file." << endl;
                isOpen = 1;
            }

            ofBuffer buffer(file);

            // Input_flag is a variable for indication the type of input.
            // If input_flag is zero, then work of line input is progress.
            // If input_flag is one, then work of dot input is progress.
            int input_flag = 0;

            // Idx is a variable for index of array.
            int idx = 0;

            // Read file line by line
            int cnt = 0;

            // TO DO
            // .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
            //////////////////////////////////////////////////////////////////////////

            freeMemory();
            for (auto row : buffer.getLines()) {
                inpt.push_back(row);
            }
            return true;
            /////////////////////////////////////////////////////////////////////////
        }
        else {
            printf("  Needs a '.maz' extension\n");
            return false;
        }
    }
    return false;
}

void ofApp::freeMemory() {
    // TO DO
    // malloc한 memory를 free해주는 함수
    inpt.clear();
    root.clear();
    // troot.clear();
    // isdfs = false;
}

bool ofApp::DFS() {
    if (inpt.empty()) return false;

    height = inpt.size();
    width = inpt[0].size();

    stack_DFS stack;
    stack.init(height, width);

    deque<deque<bool>> visited(height, deque<bool>(width, false));
    int cury = 0, curx = 0;

    stack.push(INDEX(cury, curx));
    visited[cury][curx] = true;


    root.push_back({ cury, curx });

    while (!stack.is_empty()) {
        int current_index = stack.top_ele();
        cury = INDEX_Y(current_index);
        curx = INDEX_X(current_index);

        if (inpt[cury][curx] == 'E') {
            return true;
        }

        int adj_y, adj_x;
        bool exist_adj = false;
        int direction = -1;
        if (cury > 0 && inpt[cury - 1][curx] != '-' && !visited[cury - 1][curx]) {
            direction = 3;
        }
        if (curx > 0 && inpt[cury][curx - 1] != '|' && !visited[cury][curx - 1]) {
            direction = 2;
        }
        if (cury < height - 1 && inpt[cury + 1][curx] != '-' && !visited[cury + 1][curx]) {
            direction = 1;
        }
        if (curx < width - 1 && inpt[cury][curx + 1] != '|' && !visited[cury][curx + 1]) {
            direction = 0;
        }

        if (direction != -1) {
            switch (direction) {
            case 0: // right
                adj_y = cury;
                adj_x = curx + 1;
                break;
            case 1: // bottom
                adj_y = cury + 1;
                adj_x = curx;
                break;
            case 2: // left
                adj_y = cury;
                adj_x = curx - 1;
                break;
            case 3: // top
                adj_y = cury - 1;
                adj_x = curx;
                break;
            default:
                adj_y = cury;
                adj_x = curx;
                break;
            }
            stack.push(INDEX(adj_y, adj_x));
            visited[adj_y][adj_x] = true;
            root.push_back({ adj_y, adj_x });
            cury = adj_y;
            curx = adj_x;
        }
        else {
            stack.pop();
            if (!root.empty()) {
                root.pop_back();
            }
        }
    }
    return false;
}

void ofApp::dfsdraw() {
    if (root.empty()) return;

    int maze_size = 30;
    int maze_centrize = maze_size / 2;

    for (size_t i = 0; i < root.size() - 1; ++i) {
        int first_x = root[i].second * maze_size + maze_centrize;
        int first_y = root[i].first * maze_size + maze_centrize;
        int second_x = root[i + 1].second * maze_size + maze_centrize;
        int second_y = root[i + 1].first * maze_size + maze_centrize;

        ofSetColor(255, 0, 0);
        ofDrawLine(first_x, first_y, second_x, second_y);
    }
}
