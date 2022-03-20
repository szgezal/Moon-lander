#include <graphics.hpp>
#include <vector>
#include <time.h>
#include <math.h>
#include <fstream>
#include <sstream>
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
    Shuttle(int x_, int y_, Platform* target_): x(x_), target(target_) {
        y[0] = y_;
    }

    void draw() {
        gout << color(150, 150, 150) << move_to(x - 10, y[0] - 10) << box(20, 20) << move_to(x - 10, y[0] + 10) << color(255, 255, 0)
             << line_to(x - 15, y[0] + 20) << move_to(x + 10, y[0] + 10) << line_to(x + 15, y[0] + 20);
        for (int i = 1; i <= 10; i++) {
            gout << move_to(x - 10 + i, y[0] - 10 - i) << line(20 - 2*i, 0);
        }
    }

    void movex(double& hvel) {
        if (x > width - 20 || x + hvel > width - 20) {
            x = width - 20;
            hvel = 0;
        } else if (x < 20 || x + hvel < 20) {
            x = 20;
            hvel = 0;
        }

        // platform interaction
        if (y[0] == height - 20 || (x > target->get_x() - target->get_size_x()/2 - 15 &&
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

        if (y[0] <= height - 20 && y[0] >= 20)
            y[0] += dy;

        if (y[0] > height - 20) {
            y[0] = height - 20;
            accel = -1;
        }

        if (y[0] < 20) {
            y[0] = 20;
            accel = 20;
        }

        // interaction with the platform
            // above the platform
            for (size_t i = 0; i < y.size(); i++)
                if (x > target->get_x() - target->get_size_x()/2 - 20 &&
                    x < target->get_x() + target->get_size_x()/2 + 20 &&
                    y[i] == target->get_y() - 20)
                        y[0] = target->get_y() - 20;

            // under the platform
            if (x > target->get_x() - target->get_size_x()/2 - 15 &&
                x < target->get_x() + target->get_size_x()/2 + 15 &&
                y[0] < target->get_y() + 30 && y[0] > target->get_y() - 20) {
                    accel += 20;

            }
            if (x > target->get_x() - target->get_size_x()/2 - 15 &&
                x < target->get_x() + target->get_size_x()/2 + 15 &&
                y[0] < target->get_y() + 15 && y[0] > target->get_y() - 15) {
                    y[0] = target->get_y() + 25;
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

    int get_x() {
        return x;
    }

    int get_y() {
        return y[0];
    }

private:
    int x;
    std::vector<int> y = std::vector<int> (1, 0);
    Platform* target;
};


struct Rgb {
    int r, g, b;
};


void textpos(canvas& c, std::string s, int y, int size, int offset) {
    c.load_font("LiberationSans-Regular.ttf", size);
    int x = (width - gout.twidth(s)*(size/16.0) - 10)/2 - 1 + offset;
    c << color(255, 255, 255) << move_to(x, y) << text(s);
}

std::vector<std::vector<Rgb>> background(std::string img_name) {
    std::ifstream f(img_name);

    int wid, hei;
    f >> hei >> wid;

    std::vector<std::vector<Rgb>> img = std::vector<std::vector<Rgb>>(hei, std::vector<Rgb>(wid));

    for (int y = 0; y < wid; y++) {
        for (int x = 0; x < hei; x++) {
            f >> img[x][y].r >> img[x][y].g >> img[x][y].b;
        }
    }

    f.close();

    return img;
}

// Phase 1
void menu(event ev, std::vector<std::vector<Rgb>> img) {
    canvas c;
    std::string s;
    c.open(800, 800);

    textpos(c, "Moon-Lander", 99, 50, -5);
    textpos(c, "PLAY", 320, 30, 0);
    textpos(c, "Help: - Use \"W\", \"A\" and \"D\" to move", 499, 16, -34);
    textpos(c, "- Try to keep your vertical velocity below 40", 519, 16, 44);
    textpos(c, "- You can start the game by pressing ENTER", 539, 16, 32);
    textpos(c, "or clicking the play button", 559, 16, -18);
    textpos(c, "- Exit with the Esc key", 579, 16, -44);

    // borders for the "PLAY" button
    c << move_to(width/2-51, height/2-88)
      << line_to(width/2-51, height/2-38)
      << line_to(width/2+49, height/2-38)
      << line_to(width/2+49, height/2-88)
      << line_to(width/2-51, height/2-88)
      << refresh;

    gout << stamp(c, 0, 0) << refresh;

    while (gin >> ev) {
        if ((ev.pos_x >= width/2-51 &&
             ev.pos_x <= width/2+49 &&
             ev.pos_y >= height/2-88 &&
             ev.pos_y <= height/2-38 &&
             ev.button == btn_left) || ev.keycode == key_enter) {
            gout << color(0, 0, 0) << move_to(0, 0) << box(width, height) << refresh;

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    gout << move_to(x, y) << color(img[x][y].r, img[x][y].g, img[x][y].b) << dot;
                }
            }

            break;

        } else if (ev.type == ev_key)
            if (ev.keycode == key_escape)
                exit(0);
    }
}

// Phase 2
bool gameloop(event ev, std::vector<std::vector<Rgb>> img) {
    gout.showmouse(false);
    canvas c;
    std::string s;
    c.open(800, 800);

    double accel = -10, hvel = 0;
    bool crashed = false;

    Platform platform(rand() % (width - 200) + 100, rand() % (height - 600) + 550, rand() % 40 + 60);
    Shuttle shuttle(rand() % (width - 200) + 100, rand() % (height - 500) + 50, &platform);

    while (gin >> ev) {
        if (ev.type != ev_mouse) {

            for (int y = 0; y < 30; y++) {
                for (int x = 0; x < 300; x++) {
                    gout << move_to(x, y) << color(img[x][y].r, img[x][y].g, img[x][y].b) << dot;
                }
            }

            std::stringstream ss;
            ss << "Vertical velocity: " << accel;
            if (accel <= 40) {
                gout << color(0, 255, 0) << move_to(10, 20) << text(ss.str());
            } else {
                gout << color(255, 0, 0) << move_to(10, 20) << text(ss.str());
            }

            for (int y = shuttle.get_y() - 20; y <= shuttle.get_y() + 20; y++) {
                for (int x = shuttle.get_x() - 15; x <= shuttle.get_x() + 15; x++) {
                    gout << move_to(x, y) << color(img[x][y].r, img[x][y].g, img[x][y].b) << dot;
                }
            }

            shuttle.movey(accel);
            shuttle.movex(hvel);
            shuttle.draw();
            platform.draw();

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
}

//Phase 3
std::vector<std::vector<Rgb>> gameover(event ev, std::vector<std::vector<Rgb>> img,
                                                 std::vector<std::vector<Rgb>> img1,
                                                 std::vector<std::vector<Rgb>> img2,
                                                 std::vector<std::vector<Rgb>> img3, bool crashed) {
    gout.showmouse(true);
    canvas c;
    std::string s;
    c.open(800, 800);

    if (crashed) {

        textpos(c, "GAME OVER", 199, 50, -30);
        textpos(c, "TRY AGAIN", 407, 30, 0);
        textpos(c, "?", 760, 30, 387);

    } else {

        textpos(c, "SUCCESSFUL LANDING", 199, 50, -50);
        textpos(c, "PLAY AGAIN", 407, 30, 0);
        textpos(c, "?", 760, 30, 387);
    }

    // borders
    c << move_to(width/2-91, height/2-1)
      << line_to(width/2-91, height/2+49)
      << line_to(height/2+99, height/2+49)
      << line_to(height/2+99, height/2-1)
      << line_to(width/2-91, height/2-1)
      << move_to(width - 1, 750)
      << line_to(width - 41, 750)
      << line_to(width - 41, height - 1)
      << refresh;

    gout << stamp(c, 0, 0) << refresh;

    while (gin >> ev) {
        if ((ev.pos_x >= 350 &&
            ev.pos_x <= 450 &&
            ev.pos_y >= 400 &&
            ev.pos_y <= 450 &&
            ev.button == btn_left) || ev.keycode == key_enter) {
                gout << color(0, 0, 0) << move_to(0, 0) << box(width, height) << refresh;
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        gout << move_to(x, y) << color(img[x][y].r, img[x][y].g, img[x][y].b) << dot;
                    }
                }
                return img;

        } else if (ev.type == ev_key)
            if (ev.keycode == key_escape)
                exit(0);

        // background selection screen
        if (ev.pos_x >= width - 41 &&
            ev.pos_x <= width - 1 &&
            ev.pos_y >= 750 &&
            ev.pos_y <= height - 1 &&
            ev.button == btn_left) {
                c << color(0, 0, 0) << move_to(0, 0) << box(width, height);
                textpos(c, "Press \"1\", \"2\" or \"3\"", 400, 30, 40);
                gout << stamp(c, 0, 0) << refresh;

                while (gin >> ev) {
                    if (ev.keycode == '1') {
                        gout << color(0, 0, 0) << move_to(0, 0) << box(width, height) << refresh;
                        for (int y = 0; y < height; y++) {
                            for (int x = 0; x < width; x++) {
                                gout << move_to(x, y) << color(img1[x][y].r, img1[x][y].g, img1[x][y].b) << dot;
                            }
                        }
                        return img1;
                    }
                    else if (ev.keycode == '2') {
                        gout << color(0, 0, 0) << move_to(0, 0) << box(width, height) << refresh;
                        for (int y = 0; y < height; y++) {
                            for (int x = 0; x < width; x++) {
                                gout << move_to(x, y) << color(img2[x][y].r, img2[x][y].g, img2[x][y].b) << dot;
                            }
                        }
                        return img2;
                    }
                    else if (ev.keycode == '3') {
                        gout << color(0, 0, 0) << move_to(0, 0) << box(width, height) << refresh;
                        for (int y = 0; y < height; y++) {
                            for (int x = 0; x < width; x++) {
                                gout << move_to(x, y) << color(img3[x][y].r, img3[x][y].g, img3[x][y].b) << dot;
                            }
                        }
                        return img3;
                    }
                    else if (ev.type == ev_key)
                        if (ev.keycode == key_escape)
                            exit(0);
                }
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

    std::vector<std::vector<Rgb>> img1 = background("background.kep");
    std::vector<std::vector<Rgb>> img2 = background("moon.kep");
    std::vector<std::vector<Rgb>> img3 = background("csirke.kep"); // :)
    std::vector<std::vector<Rgb>> img = img1;
    menu(ev, img1);

    while (ev.button != key_escape) {
        img = gameover(ev, img, img1, img2, img3, gameloop(ev, img));
    }

    return 0;
}
