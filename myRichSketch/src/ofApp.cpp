#include "ofApp.h"
#include <time.h> 
#include <stdlib.h> // for rand() and srand()

float dot_x; // 움직이는 점의 x좌표
float dot_y; // 움직이는 점의 y좌표
float** line_in; // 선들의 좌표를 저장하는 배열
clock_t start, over, collision_time; // 시간 측정을 위한 변수들
double times; // 경과 시간을 저장하는 변수
bool* collision_flags; // 충돌 여부를 저장하는 배열
bool collision_detected = false; // 충돌이 감지되었는지 여부

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60); // 초당 60프레임으로 설정

    ofBackground(173, 216, 230); // 배경색을 파스텔 블루로 설정
    ofSetLineWidth(5); // 선의 굵기를 5로 설정

    draw_flag = 0; // 그림 그리는 플래그 초기화
    load_flag = 0; // 인풋 파일 로드 플래그 초기화

    // 움직이는 점의 시작 좌표를 초기화
    dot_x = 10;
    dot_y = 50;

    start = clock(); // 타이머 시작
    score = 10000; // 점수 초기화

    // Initialize the random seed
    srand(time(NULL));
    playerSpeed = 10; // 플레이어의 초기 속도 설정
}

//--------------------------------------------------------------
void ofApp::update() {
    if (collision_detected) { // 충돌이 감지되었을 때
        if ((clock() - collision_time) / CLOCKS_PER_SEC >= 2) { // 충돌 후 2초 경과 시
            cout << "Game Over!" << endl;
            _Exit(0); // 프로그램 종료
        }
    }
    else {
        score = max(0, score - 1); // 시간이 지날수록 점수 감소

        if (score == 0) {
            if ((clock() - collision_time) / CLOCKS_PER_SEC >= 2) { // 0점 후 2초 경과 시
                cout << "Game Over!" << endl;
                _Exit(0); // 프로그램 종료
            }
        }

        // 아이템 업데이트
        updateItems();

        // 아이템 수집 체크
        for (auto it = items.begin(); it != items.end();) {
            if (abs(dot_x - it->x) < 10 && abs(dot_y - it->y) < 10) {
                applyItemEffect(it->type); // 아이템 효과 적용
                it = items.erase(it); // 아이템 제거
            }
            else {
                ++it;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    // Draw top and bottom rectangles with gradient effect
    ofSetColor(127, 23, 31); // 브라운 색상 설정
    ofDrawRectangle(0, 0, 1024, 40);
    ofDrawRectangle(0, 728, 1024, 40);

    // 장식 요소 추가
    ofSetColor(255, 182, 193); // 파스텔 핑크 색상 설정
    for (int i = 0; i < 20; i++) {
        ofDrawCircle(ofRandom(1024), ofRandom(40), 3);
        ofDrawCircle(ofRandom(1024), ofRandom(728, 768), 3);
    }

    // draw_flag가 설정되었을 때 선과 점을 그림
    if (draw_flag) {
        for (int i = 0; i < num_of_line; i++) {
            if (collision_flags[i]) {
                ofSetColor(255, 0, 0); // 충돌한 선은 빨간색으로 표시
            }
            else {
                ofSetColor(0, 0, 0); // 일반 선은 검정색으로 표시
            }
            ofDrawLine(line_in[i][0], line_in[i][1], line_in[i][2], line_in[i][3]);
        }

        // 움직이는 점을 그림
        ofSetColor(255, 182, 193); // 파스텔 핑크 색상 설정
        ofDrawCircle(dot_x, dot_y, 10);

        // 목적지 점을 맥동 효과와 함께 그림
        ofSetColor(253, 253, 150); // 노란색 설정
        float pulse = sin(ofGetElapsedTimef() * 5) * 5;
        ofDrawCircle(984, 688, 10 + pulse);

        // 점수와 타이머를 표시
        ofSetColor(0, 0, 0); // 검정색 설정
        ofDrawBitmapString("Score: " + std::to_string(score), 20, 30);
        ofDrawBitmapString("Time: " + std::to_string((clock() - start) / CLOCKS_PER_SEC) + "s", 900, 30);

        // 아이템을 그림
        for (const auto& item : items) {
            if (item.type == 0) {
                ofSetColor(0, 255, 0); // 점수 2배 아이템: 초록색
            }
            else if (item.type == 1) {
                ofSetColor(255, 0, 0); // 점수 감소 아이템: 빨간색
            }
            else if (item.type == 2) {
                ofSetColor(0, 0, 255); // 속도 증가 아이템: 파란색
            }
            ofDrawCircle(item.x, item.y, 10);
        }

        // 충돌이 감지되었을 때 "Game Over" 텍스트를 그림
        if (collision_detected) {
            ofSetColor(255, 0, 0); // 빨간색 설정
            ofDrawBitmapString("GAME OVER", 400, 384);
        }

        // score가 0점이 되면 "Game Over" 텍스트를 그림
        if (score == 0) {
            ofSetColor(255, 0, 0); // 빨간색 설정
            ofDrawBitmapString("GAME OVER", 400, 384);
        }

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (collision_detected) return; // 충돌 후 입력 무시

    switch (key) {
    case 'q': // 프로그램 종료
        draw_flag = 0;
        // 동적으로 할당된 메모리 해제
        for (int i = 0; i < num_of_line; i++) {
            delete[] line_in[i];
        }
        delete[] line_in;
        delete[] collision_flags;
        cout << "동적으로 할당된 메모리가 해제되었습니다." << endl;
        _Exit(0);
        break;
    case 'd': // 그리기 플래그 설정 및 아이템 생성
        if (load_flag) {
            draw_flag = 1;
            generateItems(); // 아이템 생성
        }
        break;
    case OF_KEY_RIGHT: // 오른쪽 키 입력
        if (dot_x + playerSpeed < 1014 && check_road(dot_x + playerSpeed, dot_y) == 0)
            dot_x += playerSpeed;
        break;
    case OF_KEY_LEFT: // 왼쪽 키 입력
        if (dot_x - playerSpeed > 10 && check_road(dot_x - playerSpeed, dot_y) == 0)
            dot_x -= playerSpeed;
        break;
    case OF_KEY_DOWN: // 아래쪽 키 입력
        if (dot_y + playerSpeed < 718 && check_road(dot_x, dot_y + playerSpeed) == 0)
            dot_y += playerSpeed;
        break;
    case OF_KEY_UP: // 위쪽 키 입력
        if (dot_y - playerSpeed > 50 && check_road(dot_x, dot_y - playerSpeed) == 0)
            dot_y -= playerSpeed;
        break;
    }
    cout << "선택된 점의 좌표는 (" << dot_x << ", " << dot_y << ") 입니다." << endl;
    gameover(); // 게임 오버 체크
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == 'o') {
        // 입력 파일을 선택하기 위한 파일 다이얼로그 열기
        ofFileDialogResult openFileResult = ofSystemLoadDialog("게임을 위한 txt 파일을 선택하세요");

        // 선택한 파일 처리
        if (openFileResult.bSuccess) {
            processOpenFileSelection(openFileResult);
            load_flag = 1;
        }
    }
}

void ofApp::processOpenFileSelection(ofFileDialogResult openFileResult) {
    string fileName = openFileResult.getName();
    ofFile file(fileName);

    if (!file.exists()) {
        cout << "타겟 파일이 존재하지 않습니다." << endl;
        return;
    }

    cout << "타겟 파일을 찾았습니다." << endl;

    ofBuffer buffer(file);
    int line_count = 0;

    for (auto& line : buffer.getLines()) {
        vector<string> words = ofSplitString(line, " ");

        if (words.size() == 1) {
            num_of_line = atoi(words[0].c_str());
            if (num_of_line < 10 || num_of_line > 15) {
                cout << "잘못된 선의 개수입니다!" << endl;
                _Exit(0);
            }
            cout << "선의 개수는: " << num_of_line << endl;

            // 선을 위한 메모리 할당
            line_in = new float* [num_of_line];
            for (int i = 0; i < num_of_line; i++) {
                line_in[i] = new float[4];
            }

            // 충돌 플래그를 위한 메모리 할당
            collision_flags = new bool[num_of_line]();
        }
        else {
            float x1 = atoi(words[0].c_str());
            float y1 = atoi(words[1].c_str());
            float x2 = atoi(words[2].c_str());
            float y2 = atoi(words[3].c_str());

            if (x1 <= 1024 && x2 <= 1024 && y1 <= 768 && y2 <= 768) {
                line_in[line_count][0] = x1;
                line_in[line_count][1] = y1;
                line_in[line_count][2] = x2;
                line_in[line_count][3] = y2;
                line_count++;
            }
            else {
                cout << "선의 좌표가 범위를 벗어났습니다!" << endl;
                _Exit(0);
            }
        }
    }
}

void ofApp::gameover() {
    if (dot_x >= 984 && dot_y >= 688) { // 목적지에 도달했을 때
        cout << "게임 클리어!!" << endl;
        over = clock();
        times = static_cast<double>(over - start);
        printf("당신의 기록은 %.3lf 초입니다\n", times / CLOCKS_PER_SEC);
        _Exit(0);
    }
}

int ofApp::check_road(int dot_x, int dot_y) {
    float x = dot_x;
    float y = dot_y;
    int add_x, add_y;

    for (int i = 0; i < num_of_line; i++) {
        float flag_x = line_in[i][0];
        float flag_y = line_in[i][1];

        add_x = (flag_x < line_in[i][2]) - (flag_x > line_in[i][2]);
        add_y = (flag_y < line_in[i][3]) - (flag_y > line_in[i][3]);

        double bigger = max(abs(line_in[i][0] - line_in[i][2]), abs(line_in[i][1] - line_in[i][3]));
        double smaller = min(abs(line_in[i][0] - line_in[i][2]), abs(line_in[i][1] - line_in[i][3]));
        double scl = smaller / bigger;

        bool signal_x = (abs(line_in[i][0] - line_in[i][2]) == bigger);
        bool signal_y = !signal_x;

        for (int k = 0; k < bigger; k++) {
            if (abs(flag_x - x) < 10 && abs(flag_y - y) < 10) {
                collision_flags[i] = true; // 충돌 표시
                collision_detected = true; // 충돌 플래그 설정
                collision_time = clock(); // 충돌 시간 설정
                return -1; // 충돌 상태 반환
            }
            if (signal_x) {
                flag_x += add_x;
                flag_y += scl * add_y;
            }
            else {
                flag_x += scl * add_x;
                flag_y += add_y;
            }
        }
    }
    return 0; // 충돌 없음
}

void ofApp::generateItems() {
    items.clear(); // 기존 아이템 초기화
    for (int i = 0; i < 5; ++i) {
        float item_x = ofRandom(10, 1014);
        float item_y = ofRandom(50, 718);
        int type = rand() % 3; // 0, 1, 2 중 하나의 유형 선택
        float speedX = ofRandom(-0.5, 0.5); // -0.5에서 0.5 사이의 랜덤 속도 (더 느리게 조정)
        float speedY = ofRandom(-0.5, 0.5); // -0.5에서 0.5 사이의 랜덤 속도 (더 느리게 조정)
        clock_t spawnTime = clock(); // 아이템 생성 시간
        double lifespan = ofRandom(10, 20); // 5초에서 15초 사이의 랜덤 생명 시간
        items.push_back({ item_x, item_y, speedX, speedY, type, spawnTime, lifespan }); // 아이템 추가
    }
}

void ofApp::updateItems() {
    for (auto it = items.begin(); it != items.end();) {
        it->x += it->speedX;
        it->y += it->speedY;

        // 경계 조건 체크
        if (it->x < 10 || it->x > 1014) it->speedX *= -1;
        if (it->y < 50 || it->y > 718) it->speedY *= -1;

        // 아이템의 수명 체크
        if ((clock() - it->spawnTime) / CLOCKS_PER_SEC > it->lifespan) {
            it = items.erase(it); // 아이템 제거
        }
        else {
            ++it;
        }
    }
}

void ofApp::applyItemEffect(int type) {
    if (type == 0) {
        score *= 2; // 점수 2배 증가
    }
    else if (type == 1) {
        score = max(0, score / 2); // 점수 절반 감소
    }
    else if (type == 2) {
        playerSpeed *= 2; // 속도 2배 증가
        // 3초 후 속도 복구
        auto speedRecovery = [this]() {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            playerSpeed /= 2;
            };
        std::thread(speedRecovery).detach();
    }
}

