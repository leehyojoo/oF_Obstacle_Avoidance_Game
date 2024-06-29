#pragma once

#include "ofMain.h"
#include <vector>
#include <utility>
#include <random>

struct Item {
    float x, y; // �������� ��ǥ
    float speedX, speedY; // �������� �ӵ�
    int type; // �������� ���� (0: ���� 2��, 1: ���� ���� ����, 2: �ӵ� ����)
    clock_t spawnTime; // ������ ���� �ð�
    double lifespan; // �������� ���� �ð� (��)
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
    std::vector<Item> items; // �����۵��� �����ϴ� ����
    float playerSpeed; // �÷��̾��� �ӵ�

    // Helper functions
    void processOpenFileSelection(ofFileDialogResult openFileResult);
    void gameover();
    int check_road(int x, int y);
    void generateItems(); // �������� �����ϴ� �Լ�
    void updateItems(); // �������� ��ġ�� ������Ʈ�ϴ� �Լ�
    void applyItemEffect(int type); // �������� ȿ���� �����ϴ� �Լ�
};
