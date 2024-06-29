#include "ofApp.h"
#include <time.h> 
#include <stdlib.h> // for rand() and srand()

float dot_x; // �����̴� ���� x��ǥ
float dot_y; // �����̴� ���� y��ǥ
float** line_in; // ������ ��ǥ�� �����ϴ� �迭
clock_t start, over, collision_time; // �ð� ������ ���� ������
double times; // ��� �ð��� �����ϴ� ����
bool* collision_flags; // �浹 ���θ� �����ϴ� �迭
bool collision_detected = false; // �浹�� �����Ǿ����� ����

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60); // �ʴ� 60���������� ����

    ofBackground(173, 216, 230); // ������ �Ľ��� ���� ����
    ofSetLineWidth(5); // ���� ���⸦ 5�� ����

    draw_flag = 0; // �׸� �׸��� �÷��� �ʱ�ȭ
    load_flag = 0; // ��ǲ ���� �ε� �÷��� �ʱ�ȭ

    // �����̴� ���� ���� ��ǥ�� �ʱ�ȭ
    dot_x = 10;
    dot_y = 50;

    start = clock(); // Ÿ�̸� ����
    score = 10000; // ���� �ʱ�ȭ

    // Initialize the random seed
    srand(time(NULL));
    playerSpeed = 10; // �÷��̾��� �ʱ� �ӵ� ����
}

//--------------------------------------------------------------
void ofApp::update() {
    if (collision_detected) { // �浹�� �����Ǿ��� ��
        if ((clock() - collision_time) / CLOCKS_PER_SEC >= 2) { // �浹 �� 2�� ��� ��
            cout << "Game Over!" << endl;
            _Exit(0); // ���α׷� ����
        }
    }
    else {
        score = max(0, score - 1); // �ð��� �������� ���� ����

        if (score == 0) {
            if ((clock() - collision_time) / CLOCKS_PER_SEC >= 2) { // 0�� �� 2�� ��� ��
                cout << "Game Over!" << endl;
                _Exit(0); // ���α׷� ����
            }
        }

        // ������ ������Ʈ
        updateItems();

        // ������ ���� üũ
        for (auto it = items.begin(); it != items.end();) {
            if (abs(dot_x - it->x) < 10 && abs(dot_y - it->y) < 10) {
                applyItemEffect(it->type); // ������ ȿ�� ����
                it = items.erase(it); // ������ ����
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
    ofSetColor(127, 23, 31); // ���� ���� ����
    ofDrawRectangle(0, 0, 1024, 40);
    ofDrawRectangle(0, 728, 1024, 40);

    // ��� ��� �߰�
    ofSetColor(255, 182, 193); // �Ľ��� ��ũ ���� ����
    for (int i = 0; i < 20; i++) {
        ofDrawCircle(ofRandom(1024), ofRandom(40), 3);
        ofDrawCircle(ofRandom(1024), ofRandom(728, 768), 3);
    }

    // draw_flag�� �����Ǿ��� �� ���� ���� �׸�
    if (draw_flag) {
        for (int i = 0; i < num_of_line; i++) {
            if (collision_flags[i]) {
                ofSetColor(255, 0, 0); // �浹�� ���� ���������� ǥ��
            }
            else {
                ofSetColor(0, 0, 0); // �Ϲ� ���� ���������� ǥ��
            }
            ofDrawLine(line_in[i][0], line_in[i][1], line_in[i][2], line_in[i][3]);
        }

        // �����̴� ���� �׸�
        ofSetColor(255, 182, 193); // �Ľ��� ��ũ ���� ����
        ofDrawCircle(dot_x, dot_y, 10);

        // ������ ���� �Ƶ� ȿ���� �Բ� �׸�
        ofSetColor(253, 253, 150); // ����� ����
        float pulse = sin(ofGetElapsedTimef() * 5) * 5;
        ofDrawCircle(984, 688, 10 + pulse);

        // ������ Ÿ�̸Ӹ� ǥ��
        ofSetColor(0, 0, 0); // ������ ����
        ofDrawBitmapString("Score: " + std::to_string(score), 20, 30);
        ofDrawBitmapString("Time: " + std::to_string((clock() - start) / CLOCKS_PER_SEC) + "s", 900, 30);

        // �������� �׸�
        for (const auto& item : items) {
            if (item.type == 0) {
                ofSetColor(0, 255, 0); // ���� 2�� ������: �ʷϻ�
            }
            else if (item.type == 1) {
                ofSetColor(255, 0, 0); // ���� ���� ������: ������
            }
            else if (item.type == 2) {
                ofSetColor(0, 0, 255); // �ӵ� ���� ������: �Ķ���
            }
            ofDrawCircle(item.x, item.y, 10);
        }

        // �浹�� �����Ǿ��� �� "Game Over" �ؽ�Ʈ�� �׸�
        if (collision_detected) {
            ofSetColor(255, 0, 0); // ������ ����
            ofDrawBitmapString("GAME OVER", 400, 384);
        }

        // score�� 0���� �Ǹ� "Game Over" �ؽ�Ʈ�� �׸�
        if (score == 0) {
            ofSetColor(255, 0, 0); // ������ ����
            ofDrawBitmapString("GAME OVER", 400, 384);
        }

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (collision_detected) return; // �浹 �� �Է� ����

    switch (key) {
    case 'q': // ���α׷� ����
        draw_flag = 0;
        // �������� �Ҵ�� �޸� ����
        for (int i = 0; i < num_of_line; i++) {
            delete[] line_in[i];
        }
        delete[] line_in;
        delete[] collision_flags;
        cout << "�������� �Ҵ�� �޸𸮰� �����Ǿ����ϴ�." << endl;
        _Exit(0);
        break;
    case 'd': // �׸��� �÷��� ���� �� ������ ����
        if (load_flag) {
            draw_flag = 1;
            generateItems(); // ������ ����
        }
        break;
    case OF_KEY_RIGHT: // ������ Ű �Է�
        if (dot_x + playerSpeed < 1014 && check_road(dot_x + playerSpeed, dot_y) == 0)
            dot_x += playerSpeed;
        break;
    case OF_KEY_LEFT: // ���� Ű �Է�
        if (dot_x - playerSpeed > 10 && check_road(dot_x - playerSpeed, dot_y) == 0)
            dot_x -= playerSpeed;
        break;
    case OF_KEY_DOWN: // �Ʒ��� Ű �Է�
        if (dot_y + playerSpeed < 718 && check_road(dot_x, dot_y + playerSpeed) == 0)
            dot_y += playerSpeed;
        break;
    case OF_KEY_UP: // ���� Ű �Է�
        if (dot_y - playerSpeed > 50 && check_road(dot_x, dot_y - playerSpeed) == 0)
            dot_y -= playerSpeed;
        break;
    }
    cout << "���õ� ���� ��ǥ�� (" << dot_x << ", " << dot_y << ") �Դϴ�." << endl;
    gameover(); // ���� ���� üũ
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    if (key == 'o') {
        // �Է� ������ �����ϱ� ���� ���� ���̾�α� ����
        ofFileDialogResult openFileResult = ofSystemLoadDialog("������ ���� txt ������ �����ϼ���");

        // ������ ���� ó��
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
        cout << "Ÿ�� ������ �������� �ʽ��ϴ�." << endl;
        return;
    }

    cout << "Ÿ�� ������ ã�ҽ��ϴ�." << endl;

    ofBuffer buffer(file);
    int line_count = 0;

    for (auto& line : buffer.getLines()) {
        vector<string> words = ofSplitString(line, " ");

        if (words.size() == 1) {
            num_of_line = atoi(words[0].c_str());
            if (num_of_line < 10 || num_of_line > 15) {
                cout << "�߸��� ���� �����Դϴ�!" << endl;
                _Exit(0);
            }
            cout << "���� ������: " << num_of_line << endl;

            // ���� ���� �޸� �Ҵ�
            line_in = new float* [num_of_line];
            for (int i = 0; i < num_of_line; i++) {
                line_in[i] = new float[4];
            }

            // �浹 �÷��׸� ���� �޸� �Ҵ�
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
                cout << "���� ��ǥ�� ������ ������ϴ�!" << endl;
                _Exit(0);
            }
        }
    }
}

void ofApp::gameover() {
    if (dot_x >= 984 && dot_y >= 688) { // �������� �������� ��
        cout << "���� Ŭ����!!" << endl;
        over = clock();
        times = static_cast<double>(over - start);
        printf("����� ����� %.3lf ���Դϴ�\n", times / CLOCKS_PER_SEC);
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
                collision_flags[i] = true; // �浹 ǥ��
                collision_detected = true; // �浹 �÷��� ����
                collision_time = clock(); // �浹 �ð� ����
                return -1; // �浹 ���� ��ȯ
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
    return 0; // �浹 ����
}

void ofApp::generateItems() {
    items.clear(); // ���� ������ �ʱ�ȭ
    for (int i = 0; i < 5; ++i) {
        float item_x = ofRandom(10, 1014);
        float item_y = ofRandom(50, 718);
        int type = rand() % 3; // 0, 1, 2 �� �ϳ��� ���� ����
        float speedX = ofRandom(-0.5, 0.5); // -0.5���� 0.5 ������ ���� �ӵ� (�� ������ ����)
        float speedY = ofRandom(-0.5, 0.5); // -0.5���� 0.5 ������ ���� �ӵ� (�� ������ ����)
        clock_t spawnTime = clock(); // ������ ���� �ð�
        double lifespan = ofRandom(10, 20); // 5�ʿ��� 15�� ������ ���� ���� �ð�
        items.push_back({ item_x, item_y, speedX, speedY, type, spawnTime, lifespan }); // ������ �߰�
    }
}

void ofApp::updateItems() {
    for (auto it = items.begin(); it != items.end();) {
        it->x += it->speedX;
        it->y += it->speedY;

        // ��� ���� üũ
        if (it->x < 10 || it->x > 1014) it->speedX *= -1;
        if (it->y < 50 || it->y > 718) it->speedY *= -1;

        // �������� ���� üũ
        if ((clock() - it->spawnTime) / CLOCKS_PER_SEC > it->lifespan) {
            it = items.erase(it); // ������ ����
        }
        else {
            ++it;
        }
    }
}

void ofApp::applyItemEffect(int type) {
    if (type == 0) {
        score *= 2; // ���� 2�� ����
    }
    else if (type == 1) {
        score = max(0, score / 2); // ���� ���� ����
    }
    else if (type == 2) {
        playerSpeed *= 2; // �ӵ� 2�� ����
        // 3�� �� �ӵ� ����
        auto speedRecovery = [this]() {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            playerSpeed /= 2;
            };
        std::thread(speedRecovery).detach();
    }
}

