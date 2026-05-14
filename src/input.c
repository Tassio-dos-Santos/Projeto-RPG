#include "utils/input.h"

char update_keyboard_input(){
    while(!_kbhit());
    keyboard_input = _getch();
    keyboard_input = toupper(keyboard_input);

    return keyboard_input;
}