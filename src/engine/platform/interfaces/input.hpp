#pragma once

namespace platform {
    
    class input : public properties {
    public:
        enum type {
            POINTER,
            KEY,
            GAMEPAD,
            TIMER,
            EVENT
        };

        enum action {
            DOWN,
            HELD,
            UP,
            DOUBLE,
            MOVE,
            PINCH,
            DRAG,
            RELEASE,
            WHEEL,
            SELECT,
            UNSELECT,
            CUSTOM // Marks the last pre-defined event
        };

        typedef struct event {
            type input;
            action gesture;
            int identifier;
            time_t delta;
            spatial::vector::type_t travel;
            spatial::vector point;
            std::vector<spatial::vector> points;
            void* caller = NULL;
            void* reference = NULL;
        };

        typedef std::function<void(const event&)> callback_t;

        typedef struct pointer {
            time_t pressed;
            spatial::vector point;
            const char* label;
            int code;
            const char* description;
        };

        typedef struct key {
            time_t pressed;
            const char* character;
            const char* meta;
            const char* label;
            int code;
            int xref; // associates a HID to a keycode
            int vkey;
            const char* description;
        };

        typedef struct gamepad {
            time_t pressed;
            const char* character;
            const char* label;
            int code;
            const char* description;
        };

        virtual void handler(type t, action a, callback_t c, int identifier = 0) {
            callbacks[t][a][identifier] = c;
        }

        virtual void raise(const event& ev) {
            if (callbacks.find(ev.input) != callbacks.end()) {
                if (callbacks[ev.input].find(ev.gesture) != callbacks[ev.input].end()) {
                    if (callbacks[ev.input][ev.gesture].find(ev.identifier) != callbacks[ev.input][ev.gesture].end()) {
                        callbacks[ev.input][ev.gesture][ev.identifier](ev);
                    }
                    /// Also fire off the catch-all handler
                    if (callbacks[ev.input][ev.gesture].find(0) != callbacks[ev.input][ev.gesture].end()) {
                        callbacks[ev.input][ev.gesture][0](ev);
                    }
                }
            }
        }

        virtual void emit() {}

        virtual std::string printable(int vkey) { return ""; }

    protected:
        std::map<type, std::map<action, std::map<int, callback_t>>> callbacks;

        int threshold_time = 1;
        float threshold_travel = 1.0;

        float last_distance;
        spatial::vector last_position;

        std::vector<pointer*> active_pointers;
        std::vector<key*> active_keys;
        std::vector<gamepad*> active_buttons;
    };

    inline platform::input::pointer pointers[8] = {
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x00, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x01, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x02, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x03, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x04, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x05, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x06, "" },
        { 0, { 0.0f, 0.0f, 0.0f }, "", 0x07, "" }
    };

    inline platform::input::gamepad buttons[256] = {
        { 0, "", "", 0x00, "Undefined" },
        { 0, "", "VK_PAD_A", 0x00, "" },
        { 0, "", "VK_PAD_B", 0x01, "" },
        { 0, "", "VK_PAD_X", 0x02, "" },
        { 0, "", "VK_PAD_Y", 0x03, "" },
        { 0, "", "VK_PAD_RSHOULDER", 0x04, "" },
        { 0, "", "VK_PAD_LSHOULDER", 0x05, "" },
        { 0, "", "VK_PAD_LTRIGGER", 0x06, "" },
        { 0, "", "VK_PAD_RTRIGGER", 0x07, "" },
        { 0, "", "", 0x08, "Undefined" },
        { 0, "", "", 0x0A, "Undefined" },
        { 0, "", "", 0x0B, "Undefined" },
        { 0, "", "", 0x0C, "Undefined" },
        { 0, "", "", 0x0D, "Undefined" },
        { 0, "", "", 0x0E, "Undefined" },
        { 0, "", "", 0x0F, "Undefined" },
        { 0, "", "VK_PAD_DPAD_UP", 0x10, "" },
        { 0, "", "VK_PAD_DPAD_DOWN", 0x11, "" },
        { 0, "", "VK_PAD_DPAD_LEFT", 0x12, "" },
        { 0, "", "VK_PAD_DPAD_RIGHT", 0x13, "" },
        { 0, "", "VK_PAD_START", 0x14, "" },
        { 0, "", "VK_PAD_BACK", 0x15, "" },
        { 0, "", "VK_PAD_LTHUMB_PRESS", 0x16, "" },
        { 0, "", "VK_PAD_RTHUMB_PRESS", 0x17, "" },
        { 0, "", "", 0x18, "Undefined" },
        { 0, "", "", 0x1A, "Undefined" },
        { 0, "", "", 0x1B, "Undefined" },
        { 0, "", "", 0x1C, "Undefined" },
        { 0, "", "", 0x1D, "Undefined" },
        { 0, "", "", 0x1E, "Undefined" },
        { 0, "", "", 0x1F, "Undefined" },
        { 0, "", "VK_PAD_LTHUMB_UP", 0x20, "" },
        { 0, "", "VK_PAD_LTHUMB_DOWN", 0x21, "" },
        { 0, "", "VK_PAD_LTHUMB_RIGHT", 0x22, "" },
        { 0, "", "VK_PAD_LTHUMB_LEFT", 0x23, "" },
        { 0, "", "VK_PAD_LTHUMB_UPLEFT", 0x24, "" },
        { 0, "", "VK_PAD_LTHUMB_UPRIGHT", 0x25, "" },
        { 0, "", "VK_PAD_LTHUMB_DOWNRIGHT", 0x26, "" },
        { 0, "", "VK_PAD_LTHUMB_DOWNLEFT", 0x27, "" },
        { 0, "", "", 0x28, "Undefined" },
        { 0, "", "", 0x2A, "Undefined" },
        { 0, "", "", 0x2B, "Undefined" },
        { 0, "", "", 0x2C, "Undefined" },
        { 0, "", "", 0x2D, "Undefined" },
        { 0, "", "", 0x2E, "Undefined" },
        { 0, "", "", 0x2F, "Undefined" },
        { 0, "", "VK_PAD_RTHUMB_UP", 0x30, "" },
        { 0, "", "VK_PAD_RTHUMB_DOWN", 0x31, "" },
        { 0, "", "VK_PAD_RTHUMB_RIGHT", 0x32, "" },
        { 0, "", "VK_PAD_RTHUMB_LEFT", 0x33, "" },
        { 0, "", "VK_PAD_RTHUMB_UPLEFT", 0x34, "" },
        { 0, "", "VK_PAD_RTHUMB_UPRIGHT", 0x35, "" },
        { 0, "", "VK_PAD_RTHUMB_DOWNRIGHT", 0x36, "" },
        { 0, "", "VK_PAD_RTHUMB_DOWNLEFT", 0x37, "" }
    };

    inline platform::input::key keys[256] = {
        { 0, "", "", "", 0x00, 0x00, 0x41, "Undefined" },
        { 0, "", "", "VK_LBUTTON", 0x1, 0x00, 0x53, "Left mouse button" },
        { 0, "", "", "VK_RBUTTON", 0x2, 0x00, 0x44, "Right mouse button" },
        { 0, "", "", "VK_CANCEL", 0x3, 0x00, 0x46, "Control-break processing" },
        { 0, "", "", "VK_MBUTTON", 0x4, 0x41, 0x48, "Middle mouse button on a three-button mouse" },
        { 0, "", "", "", 0x5, 0x42, 0x47, "Undefined" },
        { 0, "", "", "", 0x6, 0x43, 0x5A, "Undefined" },
        { 0, "", "", "", 0x7, 0x44, 0x58, "Undefined" },
        { 0, "", "", "VK_BACK", 0x8, 0x45, 0x43, "BACKSPACE key" },
        { 0, "", "", "VK_TAB", 0x9, 0x46, 0x56, "TAB key" },
        { 0, "", "", "", 0xA, 0x47, 0x00, "Undefined" },
        { 0, "", "", "", 0xB, 0x48, 0x42, "Undefined" },
        { 0, "", "", "VK_CLEAR", 0xC, 0x49, 0x51, "CLEAR key" },
        { 0, "", "", "VK_RETURN", 0xD, 0x4A, 0x57, "ENTER key" },
        { 0, "", "", "", 0xE, 0x4B, 0x45, "Undefined" },
        { 0, "", "", "", 0xF, 0x4C, 0x52, "Undefined" },
        { 0, "", "", "VK_SHIFT", 0x10, 0x4D, 0x59, "SHIFT key" },
        { 0, "", "", "VK_CONTROL", 0x11, 0x4E, 0x54, "CTRL key" },
        { 0, "", "", "VK_MENU", 0x12, 0x4F, 0x31, "ALT key" },
        { 0, "", "", "VK_PAUSE", 0x13, 0x50, 0x32, "PAUSE key" },
        { 0, "", "", "VK_CAPITAL", 0x14, 0x51, 0x33, "CAPS LOCK key" },
        { 0, "", "", "", 0x15, 0x52, 0x34, "Undefined" },
        { 0, "", "", "", 0x16, 0x53, 0x36, "Undefined" },
        { 0, "", "", "", 0x17, 0x54, 0x35, "Undefined" },
        { 0, "", "", "", 0x18, 0x55, 0xBB, "Undefined" },
        { 0, "", "", "", 0x19, 0x56, 0x39, "Reserved for Kanji systems" },
        { 0, "", "", "", 0x1A, 0x57, 0x37, "Undefined" },
        { 0, "", "", "VK_ESCAPE", 0x1B, 0x58, 0xBD, "ESC key" },
        { 0, "", "", "", 0x1C, 0x59, 0x38, "Undefined" },
        { 0, "", "", "", 0x1D, 0x5A, 0x30, "Undefined" },
        { 0, "", "", "", 0x1E, 0x31, 0xDD, "Undefined" },
        { 0, "", "", "", 0x1F, 0x32, 0x4F, "Reserved for Kanji systems" },
        { 0, " ", " ", "VK_SPACE", 0x20, 0x33, 0x55, "SPACEBAR" },
        { 0, "", "", "VK_PRIOR", 0x21, 0x34, 0xDB, "PAGE UP key" },
        { 0, "", "", "VK_NEXT", 0x22, 0x35, 0x49, "PAGE DOWN key" },
        { 0, "", "", "VK_END", 0x23, 0x36, 0x50, "END key" },
        { 0, "", "", "VK_HOME", 0x24, 0x37, 0xD, "HOME key" },
        { 0, "", "", "VK_LEFT", 0x25, 0x38, 0x4C, "LEFT ARROW key" },
        { 0, "", "", "VK_UP", 0x26, 0x39, 0x4A, "UP ARROW key" },
        { 0, "", "", "VK_RIGHT", 0x27, 0x30, 0xDE, "RIGHT ARROW key" },
        { 0, "", "", "VK_DOWN", 0x28, 0xD, 0x4B, "DOWN ARROW key" },
        { 0, "", "", "VK_SELECT", 0x29, 0x1B, 0xBA, "SELECT key" },
        { 0, "", "", "", 0x2A, 0x8, 0xDC, "Specific to original equipment manufacturer" },
        { 0, "", "", "VK_EXECUTE", 0x2B, 0x9, 0xBC, "EXECUTE key" },
        { 0, "", "", "VK_SNAPSHOT", 0x2C, 0x20, 0xBF, "PRINT SCREEN key" },
        { 0, "", "", "VK_INSERT", 0x2D, 0xBD, 0x4E, "INS key" },
        { 0, "", "", "VK_DELETE", 0x2E, 0xBB, 0x4D, "DEL key" },
        { 0, "", "", "VK_HELP", 0x2F, 0xDB, 0xBE, "HELP key" },
        { 0, "0", ")", "", 0x30, 0xDD, 0x9, "KEY_0" },
        { 0, "1", "!", "", 0x31, 0xDC, 0x20, "KEY_1" },
        { 0, "2", "@", "", 0x32, 0x00, 0xC0, "KEY_2" },
        { 0, "3", "#", "", 0x33, 0xBA, 0x8, "KEY_3" },
        { 0, "4", "$", "", 0x34, 0xDE, 0x00, "KEY_4" },
        { 0, "5", "%", "", 0x35, 0xC0, 0x1B, "KEY_5" },
        { 0, "6", "^", "", 0x36, 0xBC, 0x00, "KEY_6" },
        { 0, "7", "&", "", 0x37, 0xBE, 0x00, "KEY_7" },
        { 0, "8", "*", "", 0x38, 0xBF, 0x92, "KEY_8" },
        { 0, "9", "(", "", 0x39, 0x14, 0x14, "KEY_9" },
        { 0, "", "", "", 0x3A, 0x70, 0x96, "Undefined" },
        { 0, "", "", "", 0x3B, 0x71, 0x94, "Undefined" },
        { 0, "", "", "", 0x3C, 0x72, 0x00, "Undefined" },
        { 0, "", "", "", 0x3D, 0x73, 0x00, "Undefined" },
        { 0, "", "", "", 0x3E, 0x74, 0x00, "Undefined" },
        { 0, "", "", "", 0x3F, 0x75, 0x00, "Undefined" },
        { 0, "", "", "", 0x40, 0x76, 0x00, "Undefined" },
        { 0, "a", "A", "", 0x41, 0x77, 0x6E, "KEY_A" },
        { 0, "b", "B", "", 0x42, 0x78, 0x00, "KEY_B" },
        { 0, "c", "C", "", 0x43, 0x79, 0x6A, "KEY_C" },
        { 0, "d", "D", "", 0x44, 0x7A, 0x00, "KEY_D" },
        { 0, "e", "E", "", 0x45, 0x7B, 0x6B, "KEY_E" },
        { 0, "f", "F", "", 0x46, 0x00, 0x00, "KEY_F" },
        { 0, "g", "G", "", 0x47, 0x00, 0x90, "KEY_G" },
        { 0, "h", "H", "", 0x48, 0x00, 0x00, "KEY_H" },
        { 0, "i", "I", "", 0x49, 0x00, 0x00, "KEY_I" },
        { 0, "j", "J", "", 0x4A, 0x00, 0x00, "KEY_J" },
        { 0, "k", "K", "", 0x4B, 0x00, 0x6F, "KEY_K" },
        { 0, "l", "L", "", 0x4C, 0x2E, 0x00, "KEY_L" },
        { 0, "m", "M", "", 0x4D, 0x00, 0x00, "KEY_M" },
        { 0, "n", "N", "", 0x4E, 0x00, 0x6D, "KEY_N" },
        { 0, "o", "O", "", 0x4F, 0x27, 0x00, "KEY_O" },
        { 0, "p", "P", "", 0x50, 0x25, 0x00, "KEY_P" },
        { 0, "q", "Q", "", 0x51, 0x28, 0x00, "KEY_Q" },
        { 0, "r", "R", "", 0x52, 0x26, 0x60, "KEY_R" },
        { 0, "s", "S", "", 0x53, 0x00, 0x61, "KEY_S" },
        { 0, "t", "T", "", 0x54, 0x00, 0x62, "KEY_T" },
        { 0, "u", "U", "", 0x55, 0x00, 0x63, "KEY_U" },
        { 0, "v", "V", "", 0x56, 0x00, 0x64, "KEY_V" },
        { 0, "w", "W", "", 0x57, 0x00, 0x65, "KEY_W" },
        { 0, "x", "X", "", 0x58, 0x00, 0x66, "KEY_X" },
        { 0, "y", "Y", "", 0x59, 0x00, 0x67, "KEY_Y" },
        { 0, "z", "Z", "", 0x5A, 0x00, 0x00, "KEY_Z" },
        { 0, "", "", "VK_LWIN", 0x5B, 0x00, 0x68, "Left Windows key on a Microsoft Natural Keyboard" },
        { 0, "", "", "VK_RWIN", 0x5C, 0x00, 0x69, "Right Windows key on a Microsoft Natural Keyboard" },
        { 0, "", "", "VK_APPS", 0x5D, 0x00, 0x00, "Applications key on a Microsoft Natural Keyboard" },
        { 0, "", "", "", 0x5E, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x5F, 0x00, 0x00, "Undefined" },
        { 0, "", "", "VK_NUMPAD0", 0x60, 0x00, 0x74, "Numeric keypad 0 key" },
        { 0, "", "", "VK_NUMPAD1", 0x61, 0x00, 0x75, "Numeric keypad 1 key" },
        { 0, "", "", "VK_NUMPAD2", 0x62, 0x00, 0x76, "Numeric keypad 2 key" },
        { 0, "", "", "VK_NUMPAD3", 0x63, 0x00, 0x72, "Numeric keypad 3 key" },
        { 0, "", "", "VK_NUMPAD4", 0x64, 0x00, 0x77, "Numeric keypad 4 key" },
        { 0, "", "", "VK_NUMPAD5", 0x65, 0x00, 0x78, "Numeric keypad 5 key" },
        { 0, "", "", "VK_NUMPAD6", 0x66, 0x00, 0x00, "Numeric keypad 6 key" },
        { 0, "", "", "VK_NUMPAD7", 0x67, 0x00, 0x7A, "Numeric keypad 7 key" },
        { 0, "", "", "VK_NUMPAD8", 0x68, 0x00, 0x00, "Numeric keypad 8 key" },
        { 0, "", "", "VK_NUMPAD9", 0x69, 0x00, 0x7C, "Numeric keypad 9 key" },
        { 0, "", "", "VK_MULTIPLY", 0x6A, 0x00, 0x00, "Multiply key" },
        { 0, "", "", "VK_ADD", 0x6B, 0x00, 0x7D, "Add key" },
        { 0, "", "", "VK_SEPARATOR", 0x6C, 0x00, 0x00, "Separator key" },
        { 0, "", "", "VK_SUBTRACT", 0x6D, 0x00, 0x79, "Subtract key" },
        { 0, "", "", "VK_DECIMAL", 0x6E, 0x00, 0x00, "Decimal key" },
        { 0, "", "", "VK_DIVIDE", 0x6F, 0x00, 0x7B, "Divide key" },
        { 0, "", "", "VK_F1", 0x70, 0x00, 0x00, "F1 key" },
        { 0, "", "", "VK_F2", 0x71, 0x00, 0x7E, "F2 key" },
        { 0, "", "", "VK_F3", 0x72, 0x00, 0x00, "F3 key" },
        { 0, "", "", "VK_F4", 0x73, 0x00, 0x24, "F4 key" },
        { 0, "", "", "VK_F5", 0x74, 0x00, 0x21, "F5 key" },
        { 0, "", "", "VK_F6", 0x75, 0x00, 0x00, "F6 key" },
        { 0, "", "", "VK_F7", 0x76, 0x00, 0x73, "F7 key" },
        { 0, "", "", "VK_F8", 0x77, 0x00, 0x23, "F8 key" },
        { 0, "", "", "VK_F9", 0x78, 0x00, 0x71, "F9 key" },
        { 0, "", "", "VK_F10", 0x79, 0x00, 0x22, "F10 key" },
        { 0, "", "", "VK_F11", 0x7A, 0x00, 0x70, "F11 key" },
        { 0, "", "", "VK_F12", 0x7B, 0x00, 0x25, "F12 key" },
        { 0, "", "", "VK_F13", 0x7C, 0x00, 0x27, "F13 key" },
        { 0, "", "", "VK_F14", 0x7D, 0x00, 0x28, "F14 key" },
        { 0, "", "", "VK_F15", 0x7E, 0x00, 0x26, "F15 key" },
        { 0, "", "", "VK_F16", 0x7F, 0x00, 0x00, "F16 key" },
        { 0, "", "", "VK_F17", 0x80, 0x00, 0x00, "F17 key" },
        { 0, "", "", "VK_F18", 0x81, 0x00, 0x00, "F18 key" },
        { 0, "", "", "VK_F19", 0x82, 0x00, 0x00, "F19 key" },
        { 0, "", "", "VK_F20", 0x83, 0x00, 0x00, "F20 key" },
        { 0, "", "", "VK_F21", 0x84, 0x00, 0x00, "F21 key" },
        { 0, "", "", "VK_F22", 0x85, 0x00, 0x00, "F22 key (PPC only) Key used to lock device." },
        { 0, "", "", "VK_F23", 0x86, 0x00, 0x00, "F23 key" },
        { 0, "", "", "VK_F24", 0x87, 0x00, 0x00, "F24 key" },
        { 0, "", "", "", 0x88, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x89, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x8A, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x8B, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x8C, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x8D, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x8E, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x8F, 0x00, 0x00, "Undefined" },
        { 0, "", "", "VK_NUMLOCK", 0x90, 0x00, 0x00, "NUM LOCK key" },
        { 0, "", "", "VK_SCROLL", 0x91, 0x00, 0x00, "SCROLL LOCK key" },
        { 0, "", "", "VK_LSHIFT", 0x92, 0x00, 0x00, "Left SHIFT" },
        { 0, "", "", "VK_RSHIFT", 0x93, 0x00, 0x00, "Right SHIFT" },
        { 0, "", "", "VK_LCONTROL", 0x94, 0x00, 0x00, "Left CTRL" },
        { 0, "", "", "VK_RCONTROL", 0x95, 0x00, 0x00, "Right CTRL" },
        { 0, "", "", "VK_LMENU", 0x96, 0x00, 0x00, "Left ALT" },
        { 0, "", "", "VK_RMENU", 0x97, 0x00, 0x00, "Right ALT" },
        { 0, "", "", "", 0x98, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x99, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x9A, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x9B, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x9C, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x9D, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x9E, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0x9F, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA0, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA1, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA2, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA3, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA4, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA5, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA6, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA7, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA8, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xA9, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xAA, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xAB, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xAC, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xAD, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xAE, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xAF, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB0, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB1, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB2, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB3, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB4, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB5, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB6, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB7, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB8, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xB9, 0x00, 0x00, "Undefined" },
        { 0, ";", ":", "", 0xBA, 0x00, 0x00, "KEY_SEMICOLON" },
        { 0, "=", "+", "", 0xBB, 0x00, 0x00, "KEY_EQUAL" },
        { 0, ",", "<", "", 0xBC, 0x00, 0x00, "KEY_COMMA" },
        { 0, "-", "_", "", 0xBD, 0x00, 0x00, "KEY_DASH" },
        { 0, ".", ">", "", 0xBE, 0x00, 0x00, "KEY_DOT" },
        { 0, "/", "?", "", 0xBF, 0x00, 0x00, "KEY_SLASH" },
        { 0, "`", "~", "", 0xC0, 0x00, 0x00, "KEY_BACKTICK" },
        { 0, "", "", "", 0xC1, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC2, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC3, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC4, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC5, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC6, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC7, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC8, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xC9, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xCA, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xCB, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xCC, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xCD, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xCE, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xCF, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD0, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD1, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD2, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD3, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD4, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD5, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD6, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xD7, 0x00, 0x00, "Specific to original equipment manufacturer; reserved. See following tables." },
        { 0, "", "", "", 0xD8, 0x00, 0x00, "Specific to original equipment manufacturer; reserved. See following tables." },
        { 0, "", "", "", 0xD9, 0x00, 0x00, "Specific to original equipment manufacturer; reserved. See following tables." },
        { 0, "", "", "", 0xDA, 0x00, 0x00, "Specific to original equipment manufacturer; reserved. See following tables." },
        { 0, "[", "{", "", 0xDB, 0x00, 0x00, "Specific to original equipment manufacturer; reserved. See following tables." },
        { 0, "\\", " | ", "", 0xDC, 0x00, 0x00, "Specific to original equipment manufacturer; reserved.See following tables." },
        { 0, "]", "}", "", 0xDD, 0x92, 0x00, "Specific to original equipment manufacturer; reserved. See following tables." },
        { 0, "'", "\"", "", 0xDE, 0x00, 0x00, "Specific to original equipment manufacturer; reserved.See following tables." },
        { 0, "", "", "", 0xDF, 0x96, 0x00, "Specific to original equipment manufacturer" },
        { 0, "", "", "", 0xE0, 0x11, 0x00, "Specific to original equipment manufacturer" },
        { 0, "", "", "", 0xE1, 0x10, 0x00, "Undefined" },
        { 0, "", "", "", 0xE2, 0x5D, 0x00, "Specific to original equipment manufacturer" },
        { 0, "", "", "VK_PACKET", 0xE3, 0x12, 0x00, "Used to pass Unicode characters as if they were keystrokes. If VK_PACKET is used with SendInput, then the Unicode character to be delivered should be placed into the lower 16 bits of the scan code. If a keyboard message is removed from the message queue and the virtual key is VK_PACKET, then the Unicode character will be the upper 16 bits of the lparam." },
        { 0, "", "", "", 0xE4, 0x95, 0x00, "Undefined" },
        { 0, "", "", "", 0xE5, 0x93, 0x00, "Undefined" },
        { 0, "", "", "", 0xE6, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xE7, 0x97, 0x00, "Undefined" },
        { 0, "", "", "", 0xE8, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xE9, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xEA, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xEB, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xEC, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xED, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xEE, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xEF, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xF0, 0x00, 0x00, "Undefined" },
        { 0, "", "", "", 0xF1, 0x00, 0x00, "Specific to original equipment manufacturer" },
        { 0, "", "", "VK_ATTN", 0xF2, 0x00, 0x00, "ATTN key" },
        { 0, "", "", "VK_CRSEL", 0xF3, 0x00, 0x00, "CRSEL key" },
        { 0, "", "", "VK_EXSEL", 0xF4, 0x00, 0x00, "EXSEL key" },
        { 0, "", "", "VK_EREOF", 0xF5, 0x00, 0x00, "Erase EOF key" },
        { 0, "", "", "VK_PLAY", 0xF6, 0x00, 0x00, "PLAY key" },
        { 0, "", "", "VK_ZOOM", 0xF7, 0x00, 0x00, "ZOOM key" },
        { 0, "", "", "VK_NONAME", 0xF8, 0x00, 0x00, "Reserved for future use" },
        { 0, "", "", "VK_PA1", 0xF9, 0x00, 0x00, "PA1 key" },
        { 0, "", "", "VK_OEM_CLEAR", 0xFA, 0x00, 0x00, "CLEAR key" }
    };

}

/*

Unaccounted for

VK_KEYLOCK	F22	Key used to lock device
Original equipment manufacturers should make special note of the VK key ranges reserved for specific original equipment manufacturer use: 2A, DBE4, E6, and E9F5.
In addition to the VK key assignments in the previous table, Microsoft has assigned the following specific original equipment manufacturer VK keys.
Symbolic constant	Hexadecimal value	Mouse or keyboard equivalent

VK_OEM_SCROLL	0x91	None
VK_OEM_1	0xBA	";:" for US
VK_OEM_PLUS	0xBB	"+" any country/region
VK_OEM_COMMA	0xBC	"," any country/region
VK_OEM_MINUS	0xBD	"-" any country/region
VK_OEM_PERIOD	0xBE	"." any country/region
VK_OEM_2	0xBF	"/?" for US
VK_OEM_3	0xC0	"`~" for US
VK_OEM_4	0xDB	"[{" for US
VK_OEM_5	0xDC	"\|" for US
VK_OEM_6	0xDD	"]}" for US
VK_OEM_7	0xDE	"'"" for US
VK_OEM_8	0xDF	None
VK_OEM_AX	0xE1	AX key on Japanese AX keyboard
VK_OEM_102	0xE2	"<>" or "\|" on RT 102-key keyboard
For East Asian Input Method Editors (IMEs) the following additional virtual keyboard definitions must be observed.
Symbolic constant	Hexadecimal value	Description
VK_DBE_ALPHANUMERIC	0x0f0	Changes the mode to alphanumeric.
VK_DBE_KATAKANA	0x0f1	Changes the mode to Katakana.
VK_DBE_HIRAGANA	0x0f2	Changes the mode to Hiragana.
VK_DBE_SBCSCHAR	0x0f3	Changes the mode to single-byte characters.
VK_DBE_DBCSCHAR	0x0f4	Changes the mode to double-byte characters.
VK_DBE_ROMAN	0x0f5	Changes the mode to Roman characters.
VK_DBE_NOROMAN	0x0f6	Changes the mode to non-Roman characters.
VK_DBE_ENTERWORDREGISTERMODE	0x0f7	Activates the word registration dialog box.
VK_DBE_ENTERIMECONFIGMODE	0x0f8	Activates a dialog box for setting up an IME environment.
VK_DBE_FLUSHSTRING	0x0f9	Deletes the undetermined string without determining it.
VK_DBE_CODEINPUT	0x0fa	Changes the mode to code input.
VK_DBE_NOCODEINPUT	0x0fb	Changes the mode to no-code input.

*/
