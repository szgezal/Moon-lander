#include <iostream>
#include <graphics.hpp>
#include <time.h>
#include <math.h>
using namespace std;
using namespace genv;

const int width = 800, height = 800;
const double gravity = 5;


struct Shuttle {

    Shuttle(int x_, int y_): x(x_), y(y_) {}

    void draw() {
        gout << color(210, 210, 210) << move_to(x - 10, y - 10) << box(20, 20) << move_to(x - 10, y + 10)
             << line_to(x - 15, y + 20) << move_to(x + 10, y + 10) << line_to(x + 15, y + 20);
    }

    void movey(double accel) {


        if (y <= height - 35 && y >= 15)
            y += (gravity * accel * fabs(accel));
        else if (y >= height - 35) {
            y = height - 35;
            y += (gravity * accel * fabs(accel));
        }
        else if (y <= 16) {
            y = 16;
            y += (gravity * accel * fabs(accel));
        }
    }

    void movex() {

    }

private:
    int x, y;
};


void textpos(canvas& c, string s, int y, int size) {
    c.load_font("LiberationSans-Regular.ttf", size);
    int x = (width - gout.twidth(s)*(size/16.0) - 10)/2 - 1;
    c << color(255, 255, 255) << move_to(x, y) << text(s);
}

void menu(event ev) {
    canvas c;
    string s;
    c.open(800, 800);

    textpos(c, "Moon-Lander", 99, 50);
    textpos(c, "PLAY", 407, 30);
    textpos(c, "Help: \"WASD\" to move", 499, 16);

    // borders for the "PLAY" button
    c << move_to(width/2-51, height/2-1) << line_to(width/2-51, height/2+49) << line_to(height/2+49, height/2+49)
                                         << line_to(height/2+49, height/2-1) << line_to(width/2-51, height/2-1) << refresh;

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

void gameloop(event ev) {
    int y0 = rand() % (height - 500) + 50;
    double accel = 0;
    bool gameover = false;

    Shuttle shuttle(rand() % (width - 200) + 100, y0);

    while (gin >> ev) {

        accel += (1.0/50.0);

        if (accel >= 1) {
            accel = 1;
        }

        if (ev.type == ev_timer){

            gout << color(0, 0, 0) << move_to(0, 0) << box(width, height);

            shuttle.draw();
            shuttle.movey(accel);

        } else if (ev.type == ev_key){
            if (ev.keycode == 'w') {
                accel -= (5.0/50.0);
                shuttle.movey(accel);
            } else if (ev.keycode == 'a')
                shuttle.movex();
             else if (ev.keycode == 'd')
                shuttle.movex();
        }

        if (ev.type == ev_key)
            if (ev.keycode == key_escape)
                exit(0);

        if (gameover)
            break;

        gout << refresh;
    }
}

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
