#pragma once

#include "ofMain.h"
#include <vector>
#include <utility>
#include <random>

struct Item {
    float x, y; // 아이템의 좌표
    float speedX, speedY; // 아이템의 속도
    int type; // 아이템의 유형 (0: 점수 2배, 1: 점수 절반 감소, 2: 속도 증가)
    clock_t spawnTime; // 아이템 생성 시간
    double lifespan; // 아이템의 생명 시간 (초)
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);

private:
    // Flag variables
    int draw_flag;
    int load_flag;

    // Number of lines
    int num_of_line;

    // Dot coordinates
    float dot_x;
    float dot_y;

    // Line coordinates
    float** line_in;

    // Timing variables
    clock_t start, over, collision_time;
    double times;

    // Collision variables
    bool* collision_flags;
    bool collision_detected;

    // Scoring and timing
    int score;

    // Items
    std::vector<Item> items; // 아이템들을 저장하는 벡터
    float playerSpeed; // 플레이어의 속도

    // Helper functions
    void processOpenFileSelection(ofFileDialogResult openFileResult);
    void gameover();
    int check_road(int x, int y);
    void generateItems(); // 아이템을 생성하는 함수
    void updateItems(); // 아이템의 위치를 업데이트하는 함수
    void applyItemEffect(int type); // 아이템의 효과를 적용하는 함수
};
