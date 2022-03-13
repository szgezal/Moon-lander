#include <iostream>
#include <graphics.hpp>
#include <time.h>
#include <math.h>
using namespace std;
using namespace genv;

const int width = 800, height = 800;
const double gravity = 0.001;


struct Platform {
    Platform(int x_, int y_, int size_x_):
        x(x_), y(y_), size_x(size_x_) {}

    void draw() {
        gout << color(0, 210, 0) << move_to(x - size_x/2, y) << box(size_x, 5);
    }

    int get_x() {
        return x;
    }

    int get_y() {
        return y;
    }

    int get_size_x() {
        return size_x;
    }

private:
    int x, y, size_x;
};


struct Shuttle {

    Shuttle(int x_, int y_, Platform* p_): x(x_), y(y_), p(p_) {}

    void draw() {
        gout << color(210, 210, 210) << move_to(x - 10, y - 10) << box(20, 20) << move_to(x - 10, y + 10)
             << line_to(x - 15, y + 20) << move_to(x + 10, y + 10) << line_to(x + 15, y + 20);
    }

    void movey(double& accel) {

        double dy = (gravity * accel * fabs(accel));

        if (dy <= 1 && dy > 0)
            dy = 1.0;

        if (y <= height - 30 && y >= 10)
            y += dy;

        if (y > height - 30) {
            y = height - 30;
            accel = 0;
        }

        if (y < 10) {
            y = 10;
            accel = 40;
        }

        // interaction with the platform
        if (x > p->get_x() - p->get_size_x()/2 &&
            x < p->get_x() + p->get_size_x()/2 &&
            y < p->get_y() + 15 && y > p->get_y() + 5) {

            y = p->get_y() + 15;
            accel = 40;
        }

        if (x > p->get_x() - p->get_size_x()/2 && x < p->get_x() + p->get_size_x()/2 &&
            y < p->get_y() && y > p->get_y() - 20) {

            y = p->get_y() - 20;
            accel = 0;
        }
    }

    void movex(double hvel) {
        x += hvel;
    }

private:
    int x, y;
    Platform* p;
};


void textpos(canvas& c, string s, int y, int size) {
    c.load_font("LiberationSans-Regular.ttf", size);
    int x = (width - gout.twidth(s)*(size/16.0) - 10)/2 - 1;
    c << color(255, 255, 255) << move_to(x, y) << text(s);
}


// Phase 1
void menu(event ev) {
    canvas c;
    string s;
    c.open(800, 800);

    textpos(c, "Moon-Lander", 99, 50);
    textpos(c, "PLAY", 407, 30);
    textpos(c, "Help: \"WASD\" to move", 499, 16);

    // borders for the "PLAY" button
    c << move_to(width/2-51, height/2-1)
      << line_to(width/2-51, height/2+49)
      << line_to(height/2+49, height/2+49)
      << line_to(height/2+49, height/2-1)
      << line_to(width/2-51, height/2-1)
      << refresh;

    gout << stamp(c, 0, 0) << refresh;

    while (gin >> ev) {
        if ((ev.pos_x >= 350 &&
             ev.pos_x <= 450 &&
             ev.pos_y >= 400 &&
             ev.pos_y <= 450 &&
             ev.button == btn_left) || ev.keycode == key_enter) {
            gout << color(0, 0, 0) << move_to(0, 0) << box(width, height) << refresh;
            break;

        } else if (ev.type == ev_key)
            if (ev.keycode == key_escape)
                exit(0);
    }
}

// Phase 2
void gameloop(event ev) {
    int y0 = rand() % (height - 500) + 50;
    double accel = 0, hvel = 0;
    bool gameover = false;

    Platform platform(rand() % (width - 200) + 100, rand() % (width - 600) + 550, rand() % 40 + 60);
    Shuttle shuttle(rand() % (width - 200) + 100, y0, &platform);

    while (gin >> ev) {

        gout << color(0, 0, 0) << move_to(0, 0) << box(width, height);

        shuttle.draw();
        platform.draw();
        shuttle.movey(accel);
        shuttle.movex(hvel);

        if (ev.type == ev_timer){
            accel += (1.0);

            if (accel >= 100) {
                accel = 100;
            }
        }

        //cout << accel << endl;

        if (ev.type == ev_key){
            if (ev.keycode == 'w') {
                accel -= (10.0);
                shuttle.movey(accel);
            }

            if (ev.keycode == 'a') {
                hvel -= 1.0;
                shuttle.movex(hvel);
            }

            if (ev.keycode == 'd') {
                hvel += 1.0;
                shuttle.movex(hvel);
            }

            if (ev.keycode == key_escape)
                exit(0);
        }

        if (gameover)
            break;

        gout << refresh;
    }
}

//Phase 3
void gameover(event ev) {

}


int main()
{
    srand(time(NULL));

    gout.open(width, height);
    gout << refresh;

    event ev;
    gin.timer(20);

    menu(ev);

    while (ev.button != key_escape) {
        gameloop(ev);
        gameover(ev);
    }

    return 0;

}
