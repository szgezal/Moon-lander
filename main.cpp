#include <graphics.hpp>
#include <vector>
#include <time.h>
#include <math.h>
using namespace genv;

const int width = 800, height = 800;
const double gravity = 0.005;


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

    Shuttle(int x_, int y_, Platform* target_): x(x_), y(y_), target(target_) {}

    void draw() {
        gout << color(210, 210, 210) << move_to(x - 10, y[0] - 10) << box(20, 20) << move_to(x - 10, y[0] + 10)
             << line_to(x - 15, y[0] + 20) << move_to(x + 10, y[0] + 10) << line_to(x + 15, y[0] + 20);
    }

    void movex(double& hvel) {
        if (x > width - 16) {
            x = width - 16;
            hvel = 0;
        } else if (x < 16) {
            x = 16;
            hvel = 0;
        }

        // platform interaction
        if (y[0] == height - 30 || (x > target->get_x() - target->get_size_x()/2 - 15 &&
                                 x < target->get_x() + target->get_size_x()/2 + 15 &&
                                 y[0] == target->get_y() - 20)) {
                hvel = 0;
        }

        x += hvel;
    }

    void movey(double& accel) {

        double dy = (gravity * accel * fabs(accel));

        // storing y coordinates between "jumps"
        int n = std::floor(fabs(dy));
        if (n == 0)
            n = 1;
        y = std::vector<int> (n + 1, y[0]);
        for (size_t i = 1; i < y.size(); i++) {
            y[i] += i;
        }

        if (y[0] <= height - 30 && y[0] >= 10)
            y[0] += dy;

        if (y[0] > height - 30) {
            y[0] = height - 30;
            accel = -1;
        }

        if (y[0] < 10) {
            y[0] = 10;
            accel = 20;
        }

        // interaction with the platform
            // above the platform
            if (x > target->get_x() - target->get_size_x()/2 - 15 &&
                x < target->get_x() + target->get_size_x()/2 + 15 &&
                y[0] < target->get_y() + 5 && y[0] > target->get_y() - 20)
                    y[0] = target->get_y() - 20;

            // under the platform
            if (x > target->get_x() - target->get_size_x()/2 - 15 &&
                x < target->get_x() + target->get_size_x()/2 + 15 &&
                y[0] < target->get_y() + 30 && y[0] > target->get_y() - 15) {
                    accel += 15;

            } else if (x > target->get_x() - target->get_size_x()/2 - 15 &&
                       x < target->get_x() + target->get_size_x()/2 + 15 &&
                       y[0] < target->get_y() + 15 && y[0] > target->get_y() - 15) {
                           y[0] = target->get_y() + 15;
                           accel = 20;
            }
    }

    bool speed_is_high(double accel) {
        for (size_t i = 0; i < y.size(); i++) {
            if (x > target->get_x() - target->get_size_x()/2 - 20 &&
                x < target->get_x() + target->get_size_x()/2 + 20 &&
                y[i] == target->get_y() - 20)
                    return accel > 40;
        }
    }

    bool is_landed() {
        if (x > target->get_x() - target->get_size_x()/2 - 15 &&
            x < target->get_x() + target->get_size_x()/2 + 15 &&
            y[0] == target->get_y() - 20)
                return true;
    }

private:
    int x;
    std::vector<int> y;
    Platform* target;
};


void textpos(canvas& c, std::string s, int y, int size, int offset) {
    c.load_font("LiberationSans-Regular.ttf", size);
    int x = (width - gout.twidth(s)*(size/16.0) - 10)/2 - 1 + offset;
    c << color(255, 255, 255) << move_to(x, y) << text(s);
}


// Phase 1
void menu(event ev) {
    canvas c;
    std::string s;
    c.open(800, 800);

    textpos(c, "Moon-Lander", 99, 50, -10);
    textpos(c, "PLAY", 407, 30, 0);
    textpos(c, "Help: \"WASD\" to move", 499, 16, 5);

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
bool gameloop(event ev) {
    int y0 = rand() % (height - 500) + 50;
    double accel = 0, hvel = 0;
    bool crashed = false;

    Platform platform(rand() % (width - 200) + 100, rand() % (width - 600) + 550, rand() % 40 + 60);
    Shuttle shuttle(rand() % (width - 200) + 100, y0, &platform);

    while (gin >> ev) {

        gout << color(0, 0, 0) << move_to(0, 0) << box(width, height);

        shuttle.draw();
        platform.draw();
        shuttle.movey(accel);
        shuttle.movex(hvel);

        accel += (1.0);

        if (accel >= 100) {
            accel = 100;
        }

        if (ev.type == ev_key){
            if (ev.keycode == 'w') {
                accel -= (5.0);
            }

            if (ev.keycode == 'a') {
                hvel -= 1.0;
            }

            if (ev.keycode == 'd') {
                hvel += 1.0;
            }

            if (ev.keycode == key_escape)
                exit(0);
        }

        gout << refresh;

        if (shuttle.speed_is_high(accel)) {
            crashed = true;
            return crashed;
        } else if (shuttle.is_landed() && !crashed)
            return crashed;
    }
}

//Phase 3
void gameover(event ev, bool crashed) {
    canvas c;
    std::string s;
    c.open(800, 800);

    if (crashed) {

        textpos(c, "GAME OVER", 199, 50, -30);
        textpos(c, "TRY AGAIN", 407, 30, 0);

        // borders
        c << move_to(width/2-91, height/2-1)
          << line_to(width/2-91, height/2+49)
          << line_to(height/2+99, height/2+49)
          << line_to(height/2+99, height/2-1)
          << line_to(width/2-91, height/2-1)
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
    } else {
        textpos(c, "SUCCESSFUL LANDING", 199, 50, -50);
        textpos(c, "PLAY AGAIN", 407, 30, 0);

        // borders
        c << move_to(width/2-91, height/2-1)
          << line_to(width/2-91, height/2+49)
          << line_to(height/2+99, height/2+49)
          << line_to(height/2+99, height/2-1)
          << line_to(width/2-91, height/2-1)
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
}


int main()
{
    srand(time(NULL));

    gout.open(width, height);
    gout << refresh;

    event ev;
    gin.timer(50);

    menu(ev);

    while (ev.button != key_escape) {
        gameover(ev, gameloop(ev));
    }

    return 0;
}
