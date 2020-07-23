#ifndef KEYBOARD_H
#define KEYBOARD_H

enum KeyboardState {RELEASED, BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3};
enum KeyboardState eKeyboard_Read(void);
void Keyboard_Init(void);

#endif
