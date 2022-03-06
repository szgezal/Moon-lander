#include <iostream>
#include <graphics.hpp>
using namespace std;
using namespace genv;

const int szelesseg = 800, magassag = 800;

int main()
{
    gout.open(szelesseg, magassag);
    gout << refresh;

    canvas c;
    c.open(800, 800);
    c.load_font("C:\\Users\\szgeza\\Documents\\GraphicsKitQt\\LiberationSans-Regular.ttf", 50);
    c << color(255, 255, 255) << move_to(250, 150) << text("Moon-Lander");
    c.load_font("C:\\Users\\szgeza\\Documents\\GraphicsKitQt\\LiberationSans-Regular.ttf", 30);
    c << color(255, 255, 255) << move_to(364, 408) << text("PLAY");
    c << move_to(350, 400) << line_to(350, 450) << line_to(450, 450) << line_to(450, 400) << line_to(350, 400);
    c.load_font("C:\\Users\\szgeza\\Documents\\GraphicsKitQt\\LiberationSans-Regular.ttf", 16);
    c << color(255, 255, 255) << move_to(340, 500) << text("\"WASD\" to move") << refresh;
    gout << stamp(c, 0, 0) << refresh;

    event ev;
    gin.timer(50);
    while (gin >> ev) {

        if (ev.pos_x >= 350 && ev.pos_x <= 450 && ev.pos_y >= 400 && ev.pos_y <= 450 && ev.button == btn_left) {
            cout << "o" << endl;
            gout << color(0, 0, 0) << move_to(0, 0) << box(szelesseg, magassag) << refresh;
            break;
        }
        else if (ev.type == ev_key)
        {
            if (ev.keycode == key_escape)
            {
                return 0;
            }
        }
    }

    while (gin >> ev) {

    if (ev.type == ev_timer){

    }
    else if (ev.type == ev_key){
        if (ev.keycode == key_escape){
            return 0;
        }
    }

    }

    return 0;
}
