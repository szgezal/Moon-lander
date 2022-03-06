#include <graphics.hpp>
using namespace genv;

int main()
{
    gout.open(600, 600);
    gout << refresh;

    event ev;
    gin.timer(50);
    while (gin >> ev) {
        if (ev.type == ev_timer)
        {

        }
        else if (ev.type == ev_key)
        {
            if (ev.keycode == key_escape)
            {
                return 0;
            }
        }

    }
    return 0;
}
