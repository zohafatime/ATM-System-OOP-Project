#include "ATM.h"
#include "Admin.h"
#include "Account.h"
#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
ATM atm;

#ifndef strcpy_s
#define strcpy_s(dest, size, src)   \
    strncpy(dest, src, (size) - 1); \
    (dest)[(size) - 1] = '\0'
#endif
// --- Constants
const int SCREEN_W = 1100;
const int SCREEN_H = 750;
const int MAX_PIN = 4;
const int MAX_HISTORY = 20;
#define CX (SCREEN_W / 2)
#define BTN_W 320
#define BTN_H 50
#define BTN_X (CX - BTN_W / 2)
// colors
static const Color CLR_BG = {10, 25, 45, 255};
static const Color CLR_PANEL = {20, 40, 70, 255};
static const Color CLR_GREEN = {255, 255, 255, 255};
static const Color CLR_DIM = {140, 160, 185, 255};
static const Color CLR_DIMMER = {25, 45, 75, 255};
static const Color CLR_AMBER = {255, 220, 50, 255};
static const Color CLR_RED = {240, 70, 70, 255};
static const Color CLR_WHITE = {240, 245, 250, 255};
static const Color CLR_SCANLINE = {0, 0, 0, 25};
static const Color CLR_BORDER = {0, 150, 255, 255};
static const Color CLR_CARD = {30, 60, 100, 255};

// -- Screen States --
// BEFORE: enum had SCR_AUTH through SCR_ADMIN_RM_ACC
// AFTER: add signup and new admin screens
typedef enum
{
    SCR_AUTH,         // Login or Signup
    SCR_ROLE,         // select User or Admin
    SCR_SIGNUP_ROLE,  // signup as user only (admin cant self-signup)
    SCR_SIGNUP,       // user registration form
    SCR_SIGNUP_DONE,  // show generated ID to user
    SCR_ADMIN_SIGNUP, // admin adding new admin (shows generated ID)
    SCR_WELCOME,
    SCR_USER,
    SCR_ADMIN,
    SCR_PIN,
    SCR_MENU,
    SCR_BALANCE,
    SCR_WITHDRAW,
    SCR_DEPOSIT,
    SCR_TRANSFER,
    SCR_CHANGEPIN,
    SCR_HISTORY,
    SCR_CONFIRM,
    SCR_RECEIPT,
    SCR_ERROR,
    SCR_ADMIN_REFILL,
    SCR_ADMIN_RM_ACC,
    SCR_ADMIN_VIEW,      // view all accounts list
    SCR_ADMIN_RESET_PIN, // reset user PIN screen
    SCR_ADMIN_UNLOCK,    // unlock frozen account screen
    SCR_ADMIN_ADD_ACC    // admin adding a user account
} Screen;

// -- Transaction History --
typedef struct
{
    char desc[32];
    float amount;
    int isCredit;
} TxRecord;

// -- App State --
typedef struct
{
    Screen screen;
    Screen prevScreen;
    char pin[5];
    int pinLen;
    int pinAttempts;
    float balance;
    float pendingAmount;
    char inputBuf[16];
    int inputLen;
    TxRecord history[10];
    int historyCount;
    char message[64];
    float msgTimer;
    float time;
    int cursorBlink;
    float blinkTimer;
    char confirmAction[32];
    float scanlineOffset;
    int selectedMenu;
    char targetAccount[16];
    int transferStep;
    char loginID[32];
    int loginStep;
    int isLoggingInAsAdmin;
    int adminStep;
    char adminTemp[32];
    char signupName[32];
    char signupPin[5];
    char signupBal[16];
    int signupStep; // user (0=name,1=pin,2=bal)
    char generatedID[16];
    char loggedInAdminID[32];
    char loggedInName[32];
} AppState;

// Forward declarations
void DrawAdmin(AppState *s, Font font);
void DrawTransfer(AppState *s, Font font);
void DrawChangePin(AppState *s, Font font);
void DrawAuth(AppState *s, Font font);
void DrawRole(AppState *s, Font font);
void DrawCRT(AppState *s);
void DrawWelcome(AppState *s, Font font);
void DrawPin(AppState *s, Font font);
void DrawMenu(AppState *s, Font font);
void DrawBalance(AppState *s, Font font);
void DrawWithdraw(AppState *s, Font font);
void DrawDeposit(AppState *s, Font font);
void DrawHistory(AppState *s, Font font);
void DrawConfirm(AppState *s, Font font);
void DrawReceipt(AppState *s, Font font);
void DrawError(AppState *s, Font font);
void HandleInput(AppState *s);
void AddHistory(AppState *s, const char *desc, float amount, int isCredit);
void DrawPanel(int x, int y, int w, int h);
void DrawGreenButton(int x, int y, int w, int h, const char *label, Font font, int hover);
int IsButtonPressed(int x, int y, int w, int h);
void DrawCenteredText(Font font, const char *text, int y, int size, Color col);
void DrawStatusBar(AppState *s, Font font);
void ShowMessage(AppState *s, const char *msg);
void DrawAdminRefill(AppState *s, Font font);
void DrawAdminRmAcc(AppState *s, Font font);
void DrawSignup(AppState *s, Font font);
void DrawSignupDone(AppState *s, Font font);
void DrawAdminViewAccounts(AppState *s, Font font);
void DrawAdminResetPin(AppState *s, Font font);
void DrawAdminUnlock(AppState *s, Font font);
void DrawAdminAddAcc(AppState *s, Font font);
void DrawAdminSignup(AppState *s, Font font);
//........................................................................ main ftn
int main(void)
{
    atm.loadAccounts();
    atm.loadAdmins();
    atm.checkFirstRun();
    InitWindow(SCREEN_W, SCREEN_H, "ATM ");
    SetTargetFPS(60);

    Font font = LoadFontEx("resources/monospace.ttf", 20, 0, 256);
    if (font.texture.id == 0)
        font = GetFontDefault();

    AppState s = {};
    s.screen = SCR_AUTH;

    s.pinAttempts = 3;

    while (!WindowShouldClose())
    {
        s.time += GetFrameTime();
        s.blinkTimer += GetFrameTime();
        s.scanlineOffset = (float)fmod(s.time * 30.0f, (float)SCREEN_H);
        if (s.blinkTimer > 0.5f)
        {
            s.cursorBlink ^= 1;
            s.blinkTimer = 0;
        }
        if (s.msgTimer > 0)
            s.msgTimer -= GetFrameTime();

        HandleInput(&s);

        BeginDrawing();
        ClearBackground(CLR_BG);

        for (int x = 0; x < SCREEN_W; x += 40)
            DrawLine(x, 0, x, SCREEN_H, (Color){18, 40, 20, 80});
        for (int y = 0; y < SCREEN_H; y += 40)
            DrawLine(0, y, SCREEN_W, y, (Color){18, 40, 20, 80});

        DrawPanel(30, 30, SCREEN_W - 60, SCREEN_H - 60);

        DrawRectangle(30, 30, SCREEN_W - 60, 50, CLR_DIMMER);
        DrawRectangleLinesEx((Rectangle){30, 30, (float)SCREEN_W - 60, 50}, 1, CLR_BORDER);
        DrawTextEx(font, "GreenBank ATM  v2.4.1", (Vector2){50, 45}, 16, 1, CLR_DIM);

        char tstr[32];
        int sec = (int)s.time % 60, mn = ((int)s.time / 60) % 60;
        snprintf(tstr, sizeof(tstr), "SESSION %02d:%02d", mn, sec);
        DrawTextEx(font, tstr, (Vector2){(float)SCREEN_W - 200, 45}, 16, 1, CLR_DIM);

        switch (s.screen)
        {
        case SCR_AUTH:
            DrawAuth(&s, font);
            break;
        case SCR_ROLE:
            DrawRole(&s, font);
            break;
        case SCR_WELCOME:
            DrawWelcome(&s, font);
            break;
        case SCR_ADMIN:
            DrawAdmin(&s, font);
            break;
        case SCR_PIN:
            DrawPin(&s, font);
            break;
        case SCR_MENU:
            DrawMenu(&s, font);
            break;
        case SCR_BALANCE:
            DrawBalance(&s, font);
            break;
        case SCR_WITHDRAW:
            DrawWithdraw(&s, font);
            break;
        case SCR_DEPOSIT:
            DrawDeposit(&s, font);
            break;
        case SCR_TRANSFER:
            DrawTransfer(&s, font);
            break;
        case SCR_CHANGEPIN:
            DrawChangePin(&s, font);
            break;
        case SCR_HISTORY:
            DrawHistory(&s, font);
            break;
        case SCR_CONFIRM:
            DrawConfirm(&s, font);
            break;
        case SCR_RECEIPT:
            DrawReceipt(&s, font);
            break;
        case SCR_ERROR:
            DrawError(&s, font);
            break;
        case SCR_ADMIN_REFILL:
            DrawAdminRefill(&s, font);
            break;
        case SCR_ADMIN_RM_ACC:
            DrawAdminRmAcc(&s, font);
            break;
        case SCR_SIGNUP:
            DrawSignup(&s, font);
            break;
        case SCR_SIGNUP_DONE:
            DrawSignupDone(&s, font);
            break;
        case SCR_ADMIN_VIEW:
            DrawAdminViewAccounts(&s, font);
            break;
        case SCR_ADMIN_RESET_PIN:
            DrawAdminResetPin(&s, font);
            break;
        case SCR_ADMIN_UNLOCK:
            DrawAdminUnlock(&s, font);
            break;
        case SCR_ADMIN_ADD_ACC:
            DrawAdminAddAcc(&s, font);
            break;
        case SCR_ADMIN_SIGNUP:
            DrawAdminSignup(&s, font);
            break;
        case SCR_USER:
            break;
        }

        DrawStatusBar(&s, font);
        DrawCRT(&s);
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}

// -- CRT overlay --
void DrawCRT(AppState *s)
{
    for (int y = 0; y < SCREEN_H; y += 4)
        DrawRectangle(0, y, SCREEN_W, 2, CLR_SCANLINE);
    DrawRectangleGradientH(0, 0, 120, SCREEN_H, (Color){0, 0, 0, 120}, (Color){0, 0, 0, 0});
    DrawRectangleGradientH(SCREEN_W - 120, 0, 120, SCREEN_H, (Color){0, 0, 0, 0}, (Color){0, 0, 0, 120});
    DrawRectangleGradientV(0, 0, SCREEN_W, 80, (Color){0, 0, 0, 80}, (Color){0, 0, 0, 0});
    DrawRectangleGradientV(0, SCREEN_H - 80, SCREEN_W, 80, (Color){0, 0, 0, 0}, (Color){0, 0, 0, 80});
}
// -- Helpers --
void DrawPanel(int x, int y, int w, int h)
{
    DrawRectangle(x, y, w, h, CLR_PANEL);
    DrawRectangleLinesEx((Rectangle){(float)x, (float)y, (float)w, (float)h}, 2, CLR_BORDER);
    DrawRectangleLinesEx((Rectangle){(float)x + 4, (float)y + 4, (float)w - 8, (float)h - 8}, 1, CLR_DIMMER);
}

void DrawCenteredText(Font font, const char *text, int y, int size, Color col)
{
    Vector2 sz = MeasureTextEx(font, text, (float)size, 1);
    Vector2 pos = {((float)SCREEN_W - sz.x) / 2, (float)y};

    DrawTextEx(font, text, (Vector2){pos.x + 1, pos.y + 1}, (float)size, 1, (Color){0, 0, 0, 150});
    DrawTextEx(font, text, pos, (float)size, 1, col);
}

int IsButtonPressed(int x, int y, int w, int h)
{
    if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        return 0;
    Vector2 m = GetMousePosition();
    return m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h;
}

void DrawGreenButton(int x, int y, int w, int h, const char *label, Font font, int hover)
{
    Vector2 m = GetMousePosition();
    int hov = (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
    Color bg = hov ? CLR_GREEN : CLR_CARD;
    Color txt = hov ? CLR_BG : CLR_GREEN;
    Color brd = hov ? CLR_GREEN : CLR_BORDER;
    DrawRectangle(x, y, w, h, bg);
    DrawRectangleLinesEx((Rectangle){(float)x, (float)y, (float)w, (float)h}, 2, brd);
    Vector2 tsz = MeasureTextEx(font, label, 18, 1);
    DrawTextEx(font, label, (Vector2){x + (w - tsz.x) / 2, y + (h - tsz.y) / 2}, 18, 1, txt);
}

void AddHistory(AppState *s, const char *desc, float amount, int isCredit)
{
    if (s->historyCount < MAX_HISTORY)
    {
        int i = s->historyCount++;
        strncpy_s(s->history[i].desc, sizeof(s->history[i].desc), desc, 31);
        s->history[i].amount = amount;
        s->history[i].isCredit = isCredit;
    }
}

void ShowMessage(AppState *s, const char *msg)
{
    strncpy_s(s->message, sizeof(s->message), msg, 63);
    s->msgTimer = 2.5f;
}

void DrawStatusBar(AppState *s, Font font)
{
    int y = SCREEN_H - 55;
    DrawRectangle(30, y, SCREEN_W - 60, 25, CLR_DIMMER);
    DrawRectangleLinesEx((Rectangle){30, (float)y, (float)SCREEN_W - 60, 25}, 1, CLR_BORDER);

    if (s->msgTimer > 0)
    {
        float alpha = s->msgTimer > 0.5f ? 1.0f : s->msgTimer / 0.5f;
        Color mc = CLR_AMBER;
        mc.a = (unsigned char)(alpha * 255);
        DrawCenteredText(font, s->message, y + 2, 22, mc);
    }
    else
    {
        DrawCenteredText(font, "INSERT CARD OR PRESS [ENTER] TO BEGIN", y + 4, 20, CLR_DIMMER);
    }
}

void DrawWelcome(AppState *s, Font font)
{
    DrawCenteredText(font, "BANK", 110, 38, CLR_GREEN);
    DrawCenteredText(font, "AUTOMATED TELLER MACHINE", 158, 18, CLR_DIM);
    DrawRectangle(150, 188, SCREEN_W - 300, 2, CLR_BORDER);
    int cx = SCREEN_W / 2 - 60, cy = 215;
    DrawRectangleRounded((Rectangle){(float)cx, (float)cy, 120, 75}, 0.1f, 4, CLR_CARD);
    DrawRectangleLinesEx((Rectangle){(float)cx, (float)cy, 120, 75}, 2, CLR_BORDER);
    if (s->cursorBlink)
        DrawCenteredText(font, ">> PLEASE INSERT YOUR CARD <<", 318, 20, CLR_GREEN);
    if (IsKeyPressed(KEY_ENTER) || IsButtonPressed(30, 30, SCREEN_W - 60, SCREEN_H - 60))
        s->screen = SCR_PIN;
}
// 1. Sign In / Sign Up Screen
void DrawAuth(AppState *s, Font font)
{
    DrawCenteredText(font, "WELCOME TO ATM", 110, 32, CLR_GREEN);
    DrawCenteredText(font, "PLEASE CHOOSE AN OPTION", 158, 18, CLR_DIM);

    if (IsButtonPressed(BTN_X, 240, BTN_W, BTN_H))
        s->screen = SCR_ROLE;
    DrawGreenButton(BTN_X, 240, BTN_W, BTN_H, "SIGN IN", font, 0);

    if (IsButtonPressed(BTN_X, 310, BTN_W, BTN_H))
    {
        s->signupStep = 0;
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->signupName[0] = '\0';
        s->signupPin[0] = '\0';
        s->signupBal[0] = '\0';
        s->screen = SCR_SIGNUP;
    }
    DrawGreenButton(BTN_X, 310, BTN_W, BTN_H, "SIGN UP", font, 0);
}

// 2. User / Admin Screen
void DrawRole(AppState *s, Font font)
{
    DrawCenteredText(font, "SELECT ACCOUNT TYPE", 110, 32, CLR_GREEN);
    DrawCenteredText(font, "SIGN IN AS:", 158, 18, CLR_DIM);

    if (IsButtonPressed(BTN_X, 220, BTN_W, BTN_H))
    {
        s->isLoggingInAsAdmin = 0;
        s->loginStep = 0;
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->pinAttempts = 3;
        s->screen = SCR_PIN;
    }
    DrawGreenButton(BTN_X, 220, BTN_W, BTN_H, "USER", font, 0);

    if (IsButtonPressed(BTN_X, 290, BTN_W, BTN_H))
    {
        s->isLoggingInAsAdmin = 1;
        s->loginStep = 0;
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->pinAttempts = 3;
        s->screen = SCR_PIN;
    }
    DrawGreenButton(BTN_X, 290, BTN_W, BTN_H, "ADMIN", font, 0);

    if (IsButtonPressed(BTN_X, 370, BTN_W, BTN_H))
        s->screen = SCR_AUTH;
    DrawGreenButton(BTN_X, 370, BTN_W, BTN_H, "BACK", font, 0);
}

void DrawPin(AppState *s, Font font)
{
    if (s->loginStep == 0)
    {
        // --- STEP 0: ID ENTER KARNA ---
        DrawCenteredText(font, "ENTER ACCOUNT NUMBER / ID", 110, 26, CLR_GREEN);

        char disp[32];
        snprintf(disp, sizeof(disp), "%s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 215, 24, CLR_GREEN);

        // NEXT Button: Yeh ID confirm karke PIN par le jayega
        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen > 0)
        {
            strcpy_s(s->loginID, sizeof(s->loginID), s->inputBuf);
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->loginStep = 1;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else
    {
        // --- STEP 1: PIN ENTER KARNA ---
        DrawCenteredText(font, "ENTER YOUR 4-DIGIT PIN", 110, 26, CLR_GREEN);

        int bx = SCREEN_W / 2 - 98, by = 220;
        for (int i = 0; i < MAX_PIN; i++)
        {
            DrawRectangle(bx + i * 44, by, 36, 50, CLR_CARD);
            DrawRectangleLinesEx((Rectangle){(float)(bx + i * 44), (float)by, 36, 50}, 2, CLR_BORDER);
            if (i < s->inputLen)
                DrawCircle(bx + i * 44 + 18, by + 25, 10, CLR_GREEN);
        }

        // LOGIN Button: Yeh PIN check karke Menu par le jayega
        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen == MAX_PIN)
        {
            // Hum HandleInput wali logic yahan repeat kar rahe hain taake button se bhi kaam kare
            if (s->isLoggingInAsAdmin == 0)
            {
                bool success = atm.authenticate(s->loginID, s->inputBuf);
                if (success)
                {
                    s->balance = (float)atm.currentAccount->getBalance();
                    strcpy_s(s->loggedInName, sizeof(s->loggedInName), atm.currentAccount->getHolderName().c_str());
                    s->historyCount = 0; // clear GUI history, reload below
                    s->pinAttempts = 3;
                    s->inputLen = 0;
                    s->pinLen = 0;
                    s->screen = SCR_MENU;
                }
                else
                {
                    s->pinAttempts--;
                    s->inputLen = 0;
                    s->pinLen = 0;
                    if (s->pinAttempts <= 0)
                    {
                        strcpy_s(s->message, sizeof(s->message), "CARD BLOCKED");
                        s->screen = SCR_ERROR;
                    }
                    else
                        ShowMessage(s, "INCORRECT PIN");
                }
            }
            else // admin login
            {
                // find admin manually since ATM doesn't expose admin auth directly
                bool found = false;
                for (int i = 0; i < atm.getAdminCount(); i++)
                {
                    if (atm.getAdmin(i)->getAccountNumber() == string(s->loginID) &&
                        atm.getAdmin(i)->verifyPin(string(s->inputBuf)))
                    {
                        strcpy_s(s->loggedInName, sizeof(s->loggedInName), atm.getAdmin(i)->getName().c_str());
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    strcpy_s(s->loggedInAdminID, sizeof(s->loggedInAdminID), s->loginID);
                    s->pinAttempts = 3;
                    s->inputLen = 0;
                    s->pinLen = 0;
                    s->screen = SCR_ADMIN;
                }
                else
                {
                    s->pinAttempts--;
                    s->inputLen = 0;
                    s->pinLen = 0;
                    if (s->pinAttempts <= 0)
                    {
                        strcpy_s(s->message, sizeof(s->message), "CARD BLOCKED");
                        s->screen = SCR_ERROR;
                    }
                    else
                        ShowMessage(s, "INCORRECT PIN");
                }
            }
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "LOGIN", font, 0);
    }

    // BACK Button: Yeh hamesha neechay rahega taake user peeche ja sakay
    if (IsButtonPressed(BTN_X, 400, BTN_W, BTN_H))
    {
        if (s->loginStep == 1)
        {
            s->loginStep = 0; // PIN se wapis ID par
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
        }
        else
        {
            s->screen = SCR_ROLE; // ID se wapis Role selection par
        }
    }
    DrawGreenButton(BTN_X, 400, BTN_W, BTN_H, "BACK", font, 0);
}

void DrawMenu(AppState *s, Font font)
{
    DrawCenteredText(font, "--- ACCOUNT MENU ---", 80, 26, CLR_GREEN);
    char welcome[48];
    snprintf(welcome, sizeof(welcome), "WELCOME, %s", s->loggedInName);
    DrawCenteredText(font, welcome, 108, 18, CLR_AMBER);
    const char *labels[] = {
        "1. Check Balance", "2. Withdraw Cash", "3. Deposit Funds",
        "4. Funds Transfer", "5. Mini Statement", "6. Change PIN", "0. Logout"};

    int my = 140;
    for (int i = 0; i < 7; i++)
    {
        if (IsButtonPressed(CX - 300, my, 600, 38))
        {
            s->inputLen = 0;
            s->inputBuf[0] = '\0'; // Safety reset for next screen

            if (i == 0)
                s->screen = SCR_BALANCE;
            else if (i == 1)
                s->screen = SCR_WITHDRAW;
            else if (i == 2)
                s->screen = SCR_DEPOSIT;
            else if (i == 3)
            {
                s->screen = SCR_TRANSFER;
                s->transferStep = 0;
            }
            else if (i == 4)
                s->screen = SCR_HISTORY;
            else if (i == 5)
                s->screen = SCR_CHANGEPIN;
            else if (i == 6)
                s->screen = SCR_AUTH; // Logout
        }
        DrawGreenButton(CX - 300, my, 600, 38, labels[i], font, 0);
        my += 45;
    }
}

void DrawBalance(AppState *s, Font font)
{
    DrawCenteredText(font, "ACCOUNT BALANCE", 110, 26, CLR_GREEN);

    char bal[32];
    snprintf(bal, sizeof(bal), "PKR  %.2f", s->balance);
    DrawCenteredText(font, bal, 215, 34, CLR_GREEN);

    // Back Button logic
    if (IsButtonPressed(BTN_X, 380, BTN_W, BTN_H) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE))
    {
        s->inputLen = 0;       // Safety reset
        s->inputBuf[0] = '\0'; // Safety reset
        s->screen = SCR_MENU;
    }

    DrawGreenButton(BTN_X, 380, BTN_W, BTN_H, "BACK TO MENU", font, 0);
}

void DrawWithdraw(AppState *s, Font font)
{
    DrawCenteredText(font, "WITHDRAW CASH", 90, 26, CLR_GREEN);

    // Nayi Line: Limit display karna
    DrawCenteredText(font, "MAX LIMIT: PKR 25000", 130, 18, CLR_AMBER);

    // 1. Quick Cash Buttons (Pehle se bane hue options)
    float qamt[] = {500, 1000, 2000, 5000, 10000, 20000};
    for (int i = 0; i < 6; i++)
    {
        int bx = CX - 310 + (i % 3) * 215, by = 200 + (i / 3) * 60;
        if (IsButtonPressed(bx, by, 190, 44))
        {
            s->pendingAmount = qamt[i];
            strcpy_s(s->confirmAction, sizeof(s->confirmAction), "WITHDRAW");
            s->screen = SCR_CONFIRM;
        }
        DrawGreenButton(bx, by, 190, 44, TextFormat("PKR %d", (int)qamt[i]), font, 0);
    }

    // 2. Custom Amount Enter Karne ki field (Naya Hissa)
    DrawCenteredText(font, "OR ENTER CUSTOM AMOUNT:", 320, 18, CLR_DIM);
    char disp[32];
    snprintf(disp, sizeof(disp), "PKR %s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
    DrawCenteredText(font, disp, 360, 24, CLR_GREEN);

    // Confirm Button
    if (IsButtonPressed(CX - 200, 460, 180, BTN_H) && s->inputLen > 0)
    {
        float amt = (float)atof(s->inputBuf);

        // Agar limit se zyada amount enter ki toh Error message
        if (amt > 25000.0f)
        {
            ShowMessage(s, "LIMIT EXCEEDED! MAX IS 25000");
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
        }
        else
        {
            s->pendingAmount = amt;
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            strcpy_s(s->confirmAction, sizeof(s->confirmAction), "WITHDRAW");
            s->screen = SCR_CONFIRM;
        }
    }
    DrawGreenButton(CX - 200, 460, 180, BTN_H, "CONFIRM", font, 0);

    // Cancel Button
    if (IsButtonPressed(CX + 20, 460, 180, BTN_H))
    {
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_MENU;
    }
    DrawGreenButton(CX + 20, 460, 180, BTN_H, "CANCEL", font, 0);
}
void DrawDeposit(AppState *s, Font font)
{
    DrawCenteredText(font, "DEPOSIT FUNDS", 110, 26, CLR_GREEN);
    char disp[32];
    snprintf(disp, sizeof(disp), "PKR %s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
    DrawCenteredText(font, disp, 215, 24, CLR_GREEN);

    // Confirm Button
    if (IsButtonPressed(CX - 200, 330, 180, BTN_H) && s->inputLen > 0)
    {
        s->pendingAmount = (float)atof(s->inputBuf);
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        strcpy(s->confirmAction, "DEPOSIT"); // Confirm screen logic handle karegi
        s->screen = SCR_CONFIRM;
    }
    DrawGreenButton(CX - 200, 330, 180, BTN_H, "CONFIRM", font, 0);

    // Back Button
    if (IsButtonPressed(CX + 20, 330, 180, BTN_H))
    {
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_MENU;
    }
    DrawGreenButton(CX + 20, 330, 180, BTN_H, "BACK", font, 0);
}

void DrawHistory(AppState *s, Font font)
{
    DrawCenteredText(font, "--- MINI STATEMENT ---", 80, 26, CLR_GREEN);
    int startY = 130;
    // Agar koi transaction nahi hui
    if (s->historyCount == 0)
    {
        DrawCenteredText(font, "NO TRANSACTIONS YET", startY + 50, 20, CLR_DIM);
    }
    else
    {
        // Sirf aakhri 6 ya 7 transactions dikhane ke liye loop
        int startIdx = (s->historyCount > 7) ? s->historyCount - 7 : 0;
        int displayRow = 0;

        for (int i = startIdx; i < s->historyCount; i++)
        {
            char record[64];
            snprintf(record, sizeof(record), "%s: PKR %.2f", s->history[i].desc, s->history[i].amount);

            // Deposit (1) green mein, Withdraw/Transfer (0) red mein dikhayega
            Color textColor = s->history[i].isCredit ? CLR_GREEN : RED;

            DrawTextEx(font, record, (Vector2){150, (float)(startY + (displayRow * 35))}, 20, 1, textColor);
            displayRow++;
        }
    }

    if (IsButtonPressed(BTN_X, 480, BTN_W, BTN_H))
        s->screen = SCR_MENU;
    DrawGreenButton(BTN_X, 480, BTN_W, BTN_H, "BACK TO MENU", font, 0);
}

void DrawConfirm(AppState *s, Font font)
{
    DrawCenteredText(font, "CONFIRM TRANSACTION", 110, 26, CLR_GREEN);
    if (IsButtonPressed(CX - 220, 380, 200, BTN_H))
    {
        if (strcmp(s->confirmAction, "WITHDRAW") == 0)
        {
            bool ok = atm.currentAccount->withdraw((double)s->pendingAmount);
            if (ok)
            {
                s->balance = (float)atm.currentAccount->getBalance();
                AddHistory(s, "ATM Withdrawal", s->pendingAmount, 0);
                s->screen = SCR_RECEIPT;
            }
            else
            {
                ShowMessage(s, "INSUFFICIENT FUNDS");
                s->screen = SCR_MENU;
            }
        }
        else if (strcmp(s->confirmAction, "DEPOSIT") == 0)
        {
            atm.currentAccount->deposit((double)s->pendingAmount);
            s->balance = (float)atm.currentAccount->getBalance();
            AddHistory(s, "Cash Deposit", s->pendingAmount, 1);
            s->screen = SCR_RECEIPT;
        }
        // TRANSFER KI NAYI LOGIC:
        else if (strcmp(s->confirmAction, "TRANSFER") == 0)
        {
            if (s->pendingAmount > s->balance)
            {
                ShowMessage(s, "INSUFFICIENT FUNDS");
                s->screen = SCR_MENU;
            }
            else
            {
                Account *dest = nullptr;
                for (int i = 0; i < atm.getCount(); i++)
                {
                    if (atm.getAccount(i)->getAccountNumber() == string(s->targetAccount))
                    {
                        dest = atm.getAccount(i);
                        break;
                    }
                }
                if (dest == nullptr)
                {
                    ShowMessage(s, "ACCOUNT NOT FOUND");
                    s->screen = SCR_MENU;
                }
                else
                {
                    bool ok = atm.currentAccount->transfer(dest, (double)s->pendingAmount);
                    if (ok)
                    {
                        s->balance = (float)atm.currentAccount->getBalance();
                        char desc[32];
                        snprintf(desc, sizeof(desc), "Trf to %s", s->targetAccount);
                        AddHistory(s, desc, s->pendingAmount, 0);
                        s->screen = SCR_RECEIPT;
                    }
                    else
                    {
                        ShowMessage(s, "TRANSFER FAILED");
                        s->screen = SCR_MENU;
                    }
                }
            }
        }
    }
    char confirmLine[64];
    snprintf(confirmLine, sizeof(confirmLine), "AMOUNT: PKR %.2f", s->pendingAmount);
    DrawCenteredText(font, confirmLine, 200, 22, CLR_AMBER);
    DrawGreenButton(CX - 220, 380, 200, BTN_H, "CONFIRM", font, 0);

    if (IsButtonPressed(CX + 20, 380, 200, BTN_H))
        s->screen = SCR_MENU;
    DrawGreenButton(CX + 20, 380, 200, BTN_H, "CANCEL", font, 0);
}

void DrawReceipt(AppState *s, Font font)
{
    DrawCenteredText(font, "TRANSACTION COMPLETE", 110, 26, CLR_GREEN);

    // show what was done
    char line1[64], line2[64], line3[64];
    snprintf(line1, sizeof(line1), "TYPE:    %s", s->confirmAction);
    snprintf(line2, sizeof(line2), "AMOUNT:  PKR %.2f", s->pendingAmount);
    snprintf(line3, sizeof(line3), "BALANCE: PKR %.2f", s->balance);

    DrawTextEx(font, line1, (Vector2){200, 200}, 20, 1, CLR_DIM);
    DrawTextEx(font, line2, (Vector2){200, 240}, 20, 1, CLR_GREEN);
    DrawTextEx(font, line3, (Vector2){200, 280}, 20, 1, CLR_AMBER);

    if (IsButtonPressed(BTN_X, 480, BTN_W, BTN_H))
        s->screen = SCR_MENU;
    DrawGreenButton(BTN_X, 480, BTN_W, BTN_H, "BACK TO MENU", font, 0);
}

void DrawError(AppState *s, Font font)
{
    DrawCenteredText(font, s->message, 250, 18, CLR_AMBER);
    if (IsButtonPressed(BTN_X, 360, BTN_W, BTN_H))
        s->screen = SCR_AUTH;
    DrawGreenButton(BTN_X, 360, BTN_W, BTN_H, "RETURN TO START", font, 0);
}

void HandleInput(AppState *s)
{
    // 1. PIN AUR LOGIN KI LOGIC (NAYI)
    if (s->screen == SCR_PIN)
    {
        // ID Type karne ki logic
        if (s->loginStep == 0)
        {
            for (int k = KEY_ZERO; k <= KEY_NINE; k++)
            {
                if (IsKeyPressed(k) && s->inputLen < 15)
                {
                    s->inputBuf[s->inputLen++] = '0' + (k - KEY_ZERO);
                    s->inputBuf[s->inputLen] = '\0';
                }
            }
        }
        // PIN Type karne ki logic
        else if (s->loginStep == 1)
        {
            for (int k = KEY_ZERO; k <= KEY_NINE; k++)
            {
                if (IsKeyPressed(k) && s->inputLen < MAX_PIN)
                {
                    s->inputBuf[s->inputLen++] = '0' + (k - KEY_ZERO);
                    s->inputBuf[s->inputLen] = '\0';
                    s->pinLen = s->inputLen;
                }
            }
            if (IsKeyPressed(KEY_ENTER) && s->inputLen == MAX_PIN)
            {
                if (s->isLoggingInAsAdmin == 0)
                {
                    bool success = atm.authenticate(s->loginID, s->inputBuf);
                    if (success)
                    {
                        s->balance = (float)atm.currentAccount->getBalance();
                        strcpy_s(s->loggedInName, sizeof(s->loggedInName), atm.currentAccount->getHolderName().c_str());
                        s->historyCount = 0; // clear GUI history, reload below
                        s->pinAttempts = 3;
                        s->inputLen = 0;
                        s->pinLen = 0;
                        s->screen = SCR_MENU;
                    }
                    else
                    {
                        s->pinAttempts--;
                        s->inputLen = 0;
                        s->pinLen = 0;
                        if (s->pinAttempts <= 0)
                        {
                            strcpy_s(s->message, sizeof(s->message), "CARD BLOCKED");
                            s->screen = SCR_ERROR;
                        }
                        else
                            ShowMessage(s, "INCORRECT PIN");
                    }
                }
                else // admin login
                {
                    // find admin manually since ATM doesn't expose admin auth directly
                    bool found = false;
                    for (int i = 0; i < atm.getAdminCount(); i++)
                    {
                        if (atm.getAdmin(i)->getAccountNumber() == string(s->loginID) &&
                            atm.getAdmin(i)->verifyPin(string(s->inputBuf)))
                        {
                            strcpy_s(s->loggedInName, sizeof(s->loggedInName), atm.getAdmin(i)->getName().c_str());
                            found = true;
                            break;
                        }
                    }
                    if (found)
                    {
                        strcpy_s(s->loggedInAdminID, sizeof(s->loggedInAdminID), s->loginID);
                        s->pinAttempts = 3;
                        s->inputLen = 0;
                        s->pinLen = 0;
                        s->screen = SCR_ADMIN;
                    }
                    else
                    {
                        s->pinAttempts--;
                        s->inputLen = 0;
                        s->pinLen = 0;
                        if (s->pinAttempts <= 0)
                        {
                            strcpy_s(s->message, sizeof(s->message), "CARD BLOCKED");
                            s->screen = SCR_ERROR;
                        }
                        else
                            ShowMessage(s, "INCORRECT PIN");
                    }
                }
            }
        }
    }

    // 2. WITHDRAW AUR DEPOSIT KI LOGIC
    if (s->screen == SCR_WITHDRAW || s->screen == SCR_DEPOSIT)
    {
        for (int k = KEY_ZERO; k <= KEY_NINE; k++)
        {
            if (IsKeyPressed(k) && s->inputLen < 10)
            {
                s->inputBuf[s->inputLen++] = '0' + (k - KEY_ZERO);
                s->inputBuf[s->inputLen] = '\0';
            }
        }
    }

    // 3. BACKSPACE KI LOGIC (Delete karne ke liye)
    if (IsKeyPressed(KEY_BACKSPACE) && s->inputLen > 0)
    {
        s->inputBuf[--s->inputLen] = '\0';
        if (s->screen == SCR_PIN && s->loginStep == 1)
            s->pinLen = s->inputLen;
    }
    // 4. TRANSFER ACCOUNT AUR AMOUNT ENTER KARNA
    if (s->screen == SCR_TRANSFER || s->screen == SCR_ADMIN_REFILL ||
        s->screen == SCR_ADMIN_RM_ACC || s->screen == SCR_ADMIN_UNLOCK ||
        s->screen == SCR_ADMIN_RESET_PIN)
    {
        for (int k = KEY_ZERO; k <= KEY_NINE; k++)
        {
            if (IsKeyPressed(k) && s->inputLen < 15)
            {
                s->inputBuf[s->inputLen++] = '0' + (k - KEY_ZERO);
                s->inputBuf[s->inputLen] = '\0';
            }
        }
    }
    // 5. CHANGE PIN KI LOGIC
    if (s->screen == SCR_CHANGEPIN)
    {
        for (int k = KEY_ZERO; k <= KEY_NINE; k++)
        {
            if (IsKeyPressed(k) && s->inputLen < MAX_PIN)
            {
                s->inputBuf[s->inputLen++] = '0' + (k - KEY_ZERO);
                s->inputBuf[s->inputLen] = '\0';
            }
        }
        if (IsKeyPressed(KEY_ENTER) && s->inputLen == MAX_PIN)
        {
            atm.currentAccount->setPin(string(s->inputBuf));
            atm.currentAccount->saveToFile();
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            ShowMessage(s, "PIN CHANGED SUCCESSFULLY");
            s->screen = SCR_MENU;
        }
    }
    // Name input for signup screens (allows letters and spaces)
    if (s->screen == SCR_SIGNUP || s->screen == SCR_ADMIN_ADD_ACC || s->screen == SCR_ADMIN_SIGNUP)
    {
        // only capture name in step 0 of signup
        bool nameStep = (s->screen == SCR_SIGNUP && s->signupStep == 0) ||
                        ((s->screen == SCR_ADMIN_ADD_ACC || s->screen == SCR_ADMIN_SIGNUP) && s->adminStep == 0);

        if (nameStep)
        {
            int nameLen = strlen(s->signupName);
            // letters A-Z
            for (int k = KEY_A; k <= KEY_Z; k++)
            {
                if (IsKeyPressed(k) && nameLen < 31)
                {
                    bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
                    s->signupName[nameLen] = shift ? ('A' + k - KEY_A) : ('a' + k - KEY_A);
                    s->signupName[nameLen + 1] = '\0';
                }
            }
            // space
            if (IsKeyPressed(KEY_SPACE) && nameLen < 31 && nameLen > 0)
            {
                s->signupName[nameLen] = ' ';
                s->signupName[nameLen + 1] = '\0';
            }
            // backspace for name
            if (IsKeyPressed(KEY_BACKSPACE) && nameLen > 0)
                s->signupName[nameLen - 1] = '\0';
        }
        else
        {
            // PIN and balance steps use number input
            for (int k = KEY_ZERO; k <= KEY_NINE; k++)
            {
                if (IsKeyPressed(k) && s->inputLen < 10)
                {
                    s->inputBuf[s->inputLen++] = '0' + (k - KEY_ZERO);
                    s->inputBuf[s->inputLen] = '\0';
                }
            }
        }
    }
}
// transfer and change screen k function
void DrawTransfer(AppState *s, Font font)
{
    DrawCenteredText(font, "TRANSFER FUNDS", 110, 26, CLR_GREEN);

    if (s->transferStep == 0)
    {
        DrawCenteredText(font, "ENTER TARGET ACCOUNT NO:", 160, 18, CLR_DIM);
        char disp[32];
        snprintf(disp, sizeof(disp), "%s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 215, 24, CLR_GREEN);

        if (IsButtonPressed(CX - 200, 330, 180, BTN_H) && s->inputLen > 0)
        {
            strcpy_s(s->targetAccount, sizeof(s->targetAccount), s->inputBuf);
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->transferStep = 1;
        }
        DrawGreenButton(CX - 200, 330, 180, BTN_H, "NEXT", font, 0);
    }
    else
    {
        DrawCenteredText(font, "ENTER AMOUNT TO TRANSFER:", 160, 18, CLR_DIM);
        char disp[32];
        snprintf(disp, sizeof(disp), "PKR %s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 215, 24, CLR_GREEN);

        if (IsButtonPressed(CX - 200, 330, 180, BTN_H) && s->inputLen > 0)
        {
            s->pendingAmount = (float)atof(s->inputBuf);
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->transferStep = 0;
            strcpy_s(s->confirmAction, sizeof(s->confirmAction), "TRANSFER");
            s->screen = SCR_CONFIRM; // Confirm screen par bhejein
        }
        DrawGreenButton(CX - 200, 330, 180, BTN_H, "CONFIRM", font, 0);
    }

    if (IsButtonPressed(CX + 20, 330, 180, BTN_H))
    {
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->transferStep = 0;
        s->screen = SCR_MENU;
    }
    DrawGreenButton(CX + 20, 330, 180, BTN_H, "CANCEL", font, 0);
}

void DrawChangePin(AppState *s, Font font)
{
    DrawCenteredText(font, "ENTER NEW 4-DIGIT PIN", 110, 26, CLR_GREEN);
    int bx = SCREEN_W / 2 - 98, by = 220;

    for (int i = 0; i < MAX_PIN; i++)
    {
        DrawRectangle(bx + i * 44, by, 36, 50, CLR_CARD);
        DrawRectangleLinesEx((Rectangle){(float)(bx + i * 44), (float)by, 36, 50}, 2, CLR_BORDER);
        if (i < s->inputLen)
            DrawCircle(bx + i * 44 + 18, by + 25, 10, CLR_GREEN);
    }

    if (IsButtonPressed(BTN_X, 390, BTN_W, BTN_H))
    {
        atm.currentAccount->setPin(string(s->inputBuf));
        atm.currentAccount->saveToFile();
        ShowMessage(s, "PIN CHANGED SUCCESSFULLY");
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_MENU;
    }
    DrawGreenButton(BTN_X, 390, BTN_W, BTN_H, "CONFIRM", font, 0);
}
// BEFORE: only refill and remove worked, rest said COMING SOON
// AFTER: all options functional, uses loggedInAdminID to find correct admin
void DrawAdmin(AppState *s, Font font)
{
    DrawCenteredText(font, "--- ADMIN MENU ---", 80, 26, CLR_GREEN);
    char welcome[48];
    snprintf(welcome, sizeof(welcome), "WELCOME, %s", s->loggedInName);
    DrawCenteredText(font, welcome, 108, 18, CLR_AMBER);
    const char *labels[] = {
        "1. View All Accounts",
        "2. Add Account",
        "3. Remove Account",
        "4. Reset User PIN",
        "5. Unlock Frozen Account",
        "6. Add New Admin",
        "7. Refill ATM Cash",
        "0. Logout"};

    int my = 110;
    for (int i = 0; i < 8; i++)
    {
        if (IsButtonPressed(CX - 300, my, 600, 38))
        {
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->adminStep = 0;

            if (i == 0)
                s->screen = SCR_ADMIN_VIEW;
            else if (i == 1)
                s->screen = SCR_ADMIN_ADD_ACC;
            else if (i == 2)
                s->screen = SCR_ADMIN_RM_ACC;
            else if (i == 3)
                s->screen = SCR_ADMIN_RESET_PIN;
            else if (i == 4)
                s->screen = SCR_ADMIN_UNLOCK;
            else if (i == 5)
                s->screen = SCR_ADMIN_SIGNUP;
            else if (i == 6)
                s->screen = SCR_ADMIN_REFILL;
            else if (i == 7)
                s->screen = SCR_AUTH; // logout
        }
        DrawGreenButton(CX - 300, my, 600, 38, labels[i], font, 0);
        my += 38;
    }
}
// --- ADMIN SCREENS ---

void DrawAdminRefill(AppState *s, Font font)
{
    DrawCenteredText(font, "REFILL ATM CASH", 110, 26, CLR_GREEN);
    DrawCenteredText(font, "ENTER AMOUNT TO REFILL:", 160, 18, CLR_DIM);

    char disp[32];
    snprintf(disp, sizeof(disp), "PKR %s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
    DrawCenteredText(font, disp, 215, 24, CLR_GREEN);

    if (IsButtonPressed(CX - 200, 330, 180, BTN_H) && s->inputLen > 0)
    {
        atm.refillCash((double)atof(s->inputBuf));
        ShowMessage(s, "ATM CASH REFILLED SUCCESSFULLY");
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(CX - 200, 330, 180, BTN_H, "REFILL", font, 0);

    if (IsButtonPressed(CX + 20, 330, 180, BTN_H))
    {
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(CX + 20, 330, 180, BTN_H, "CANCEL", font, 0);
}

void DrawAdminRmAcc(AppState *s, Font font)
{
    DrawCenteredText(font, "REMOVE ACCOUNT", 110, 26, CLR_GREEN);
    DrawCenteredText(font, "ENTER ACCOUNT NO. TO REMOVE:", 160, 18, CLR_DIM);

    char disp[32];
    snprintf(disp, sizeof(disp), "%s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
    DrawCenteredText(font, disp, 215, 24, CLR_GREEN);

    // finds correct admin by loggedInAdminID
    if (IsButtonPressed(CX - 200, 330, 180, BTN_H) && s->inputLen > 0)
    {
        // find which admin is logged in, use that one
        atm.removeAccount(string(s->inputBuf));
        ShowMessage(s, "ACCOUNT REMOVED");

        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(CX - 200, 330, 180, BTN_H, "REMOVE", font, 0);

    if (IsButtonPressed(CX + 20, 330, 180, BTN_H))
    {
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(CX + 20, 330, 180, BTN_H, "CANCEL", font, 0);
}

// User signup — 3 steps: name, pin, initial balance
// Auto generates ID, shows it at the end
void DrawSignup(AppState *s, Font font)
{
    DrawCenteredText(font, "NEW ACCOUNT REGISTRATION", 80, 24, CLR_GREEN);

    if (s->signupStep == 0)
    {
        // Step 0: enter name (allow letters via keyboard)
        DrawCenteredText(font, "ENTER YOUR FULL NAME:", 140, 18, CLR_DIM);
        char disp[64];
        snprintf(disp, sizeof(disp), "%s%s", s->signupName, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->signupName[0] != '\0')
        {
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->signupStep = 1;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else if (s->signupStep == 1)
    {
        // Step 1: enter 4-digit PIN
        DrawCenteredText(font, "SET YOUR 4-DIGIT PIN:", 140, 18, CLR_DIM);
        int bx = SCREEN_W / 2 - 98, by = 220;
        for (int i = 0; i < MAX_PIN; i++)
        {
            DrawRectangle(bx + i * 44, by, 36, 50, CLR_CARD);
            DrawRectangleLinesEx((Rectangle){(float)(bx + i * 44), (float)by, 36, 50}, 2, CLR_BORDER);
            if (i < s->inputLen)
                DrawCircle(bx + i * 44 + 18, by + 25, 10, CLR_GREEN);
        }
        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen == MAX_PIN)
        {
            strcpy_s(s->signupPin, sizeof(s->signupPin), s->inputBuf);
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->signupStep = 2;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else if (s->signupStep == 2)
    {
        // Step 2: enter initial deposit
        DrawCenteredText(font, "ENTER INITIAL DEPOSIT (PKR):", 140, 18, CLR_DIM);
        char disp[32];
        snprintf(disp, sizeof(disp), "PKR %s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen > 0)
        {
            // Generate ID, create account, save
            string newID = atm.generateUserID();
            strcpy_s(s->generatedID, sizeof(s->generatedID), newID.c_str());

            double bal = atof(s->inputBuf); // string to double
            Account *newAcc = new Account(newID, string(s->signupName), bal);
            newAcc->setPin(string(s->signupPin));
            newAcc->saveToFile();

            atm.addAccountToArray(newAcc); // save

            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->screen = SCR_SIGNUP_DONE;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "CREATE ACCOUNT", font, 0);
    }

    // back button
    if (IsButtonPressed(BTN_X, 410, BTN_W, BTN_H))
    {
        if (s->signupStep > 0)
        {
            s->signupStep--;
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
        }
        else
            s->screen = SCR_AUTH;
    }
    DrawGreenButton(BTN_X, 410, BTN_W, BTN_H, "BACK", font, 0);
}

// Show generated ID after successful signup
void DrawSignupDone(AppState *s, Font font)
{
    DrawCenteredText(font, "ACCOUNT CREATED!", 110, 28, CLR_GREEN);
    DrawCenteredText(font, "YOUR ACCOUNT ID IS:", 170, 18, CLR_DIM);
    DrawCenteredText(font, s->generatedID, 210, 40, CLR_AMBER);
    DrawCenteredText(font, "PLEASE SAVE THIS ID TO LOGIN", 270, 16, CLR_DIM);

    if (IsButtonPressed(BTN_X, 340, BTN_W, BTN_H))
        s->screen = SCR_AUTH;
    DrawGreenButton(BTN_X, 340, BTN_W, BTN_H, "PROCEED TO LOGIN", font, 0);
}

// Admin views all accounts in a scrollable list
void DrawAdminViewAccounts(AppState *s, Font font)
{
    DrawCenteredText(font, "ALL ACCOUNTS", 80, 24, CLR_GREEN);

    int startY = 130;
    if (atm.getCount() == 0)
    {
        DrawCenteredText(font, "NO ACCOUNTS FOUND", startY + 30, 18, CLR_DIM);
    }
    else
    {
        // show up to 6 accounts
        int show = atm.getCount() < 6 ? atm.getCount() : 6;
        for (int i = 0; i < show; i++)
        {
            char line[64];
            // show ID, name, balance, and locked status
            snprintf(line, sizeof(line), "ID:%s  %s  PKR:%.0f  %s",
                     atm.getAccount(i)->getAccountNumber().c_str(),
                     atm.getAccount(i)->getHolderName().c_str(),
                     atm.getAccount(i)->getBalance(),
                     atm.getAccount(i)->getIsActive() ? "ACTIVE" : "LOCKED");
            Color c = atm.getAccount(i)->getIsActive() ? CLR_GREEN : CLR_RED;
            DrawTextEx(font, line, (Vector2){60, (float)(startY + i * 30)}, 16, 1, c);
        }
        if (atm.getCount() > 6)
        {
            char more[32];
            snprintf(more, sizeof(more), "...and %d more accounts", atm.getCount() - 6);
            DrawTextEx(font, more, (Vector2){60, (float)(startY + 6 * 30)}, 14, 1, CLR_DIM);
        }
    }

    if (IsButtonPressed(BTN_X, 530, BTN_W, BTN_H))
        s->screen = SCR_ADMIN;
    DrawGreenButton(BTN_X, 530, BTN_W, BTN_H, "BACK", font, 0);
}

// Admin resets a user's PIN
void DrawAdminResetPin(AppState *s, Font font)
{
    DrawCenteredText(font, "RESET USER PIN", 80, 24, CLR_GREEN);

    if (s->adminStep == 0)
    {
        DrawCenteredText(font, "ENTER USER ACCOUNT ID:", 140, 18, CLR_DIM);
        char disp[32];
        snprintf(disp, sizeof(disp), "%s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen > 0)
        {
            // verify account exists first
            bool exists = false;
            for (int i = 0; i < atm.getCount(); i++)
            {
                if (atm.getAccount(i)->getAccountNumber() == string(s->inputBuf))
                {
                    exists = true;
                    break;
                }
            }
            if (exists)
            {
                strcpy_s(s->adminTemp, sizeof(s->adminTemp), s->inputBuf);
                s->inputLen = 0;
                s->inputBuf[0] = '\0';
                s->adminStep = 1;
            }
            else
            {
                ShowMessage(s, "ACCOUNT NOT FOUND");
                s->inputLen = 0;
                s->inputBuf[0] = '\0';
            }
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else
    {
        DrawCenteredText(font, "ENTER NEW 4-DIGIT PIN:", 140, 18, CLR_DIM);
        int bx = SCREEN_W / 2 - 98, by = 220;
        for (int i = 0; i < MAX_PIN; i++)
        {
            DrawRectangle(bx + i * 44, by, 36, 50, CLR_CARD);
            DrawRectangleLinesEx((Rectangle){(float)(bx + i * 44), (float)by, 36, 50}, 2, CLR_BORDER);
            if (i < s->inputLen)
                DrawCircle(bx + i * 44 + 18, by + 25, 10, CLR_GREEN);
        }

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen == MAX_PIN)
        {
            // find correct admin and reset pin
            atm.resetUserPin(string(s->adminTemp), string(s->inputBuf));

            ShowMessage(s, "PIN RESET SUCCESSFULLY");
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->adminStep = 0;
            s->screen = SCR_ADMIN;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "CONFIRM", font, 0);
    }

    if (IsButtonPressed(BTN_X, 410, BTN_W, BTN_H))
    {
        s->adminStep = 0;
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(BTN_X, 410, BTN_W, BTN_H, "CANCEL", font, 0);
}

// Admin unlocks a frozen/locked account
void DrawAdminUnlock(AppState *s, Font font)
{
    DrawCenteredText(font, "UNLOCK FROZEN ACCOUNT", 80, 24, CLR_GREEN);
    DrawCenteredText(font, "ENTER ACCOUNT ID TO UNLOCK:", 140, 18, CLR_DIM);

    char disp[32];
    snprintf(disp, sizeof(disp), "%s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
    DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

    if (IsButtonPressed(CX - 200, 330, 180, BTN_H) && s->inputLen > 0)
    {
        atm.unlockAccount(string(s->inputBuf));
        ShowMessage(s, "ACCOUNT UNLOCKED SUCCESSFULLY");
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(CX - 200, 330, 180, BTN_H, "UNLOCK", font, 0);

    if (IsButtonPressed(CX + 20, 330, 180, BTN_H))
    {
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(CX + 20, 330, 180, BTN_H, "CANCEL", font, 0);
}

// Admin adds a user account (same as signup but from admin side)
void DrawAdminAddAcc(AppState *s, Font font)
{
    DrawCenteredText(font, "ADD NEW ACCOUNT", 80, 24, CLR_GREEN);

    if (s->adminStep == 0)
    {
        DrawCenteredText(font, "ENTER HOLDER NAME:", 140, 18, CLR_DIM);
        char disp[64];
        snprintf(disp, sizeof(disp), "%s%s", s->signupName, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->signupName[0] != '\0')
        {
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->adminStep = 1;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else if (s->adminStep == 1)
    {
        DrawCenteredText(font, "SET 4-DIGIT PIN:", 140, 18, CLR_DIM);
        int bx = SCREEN_W / 2 - 98, by = 220;
        for (int i = 0; i < MAX_PIN; i++)
        {
            DrawRectangle(bx + i * 44, by, 36, 50, CLR_CARD);
            DrawRectangleLinesEx((Rectangle){(float)(bx + i * 44), (float)by, 36, 50}, 2, CLR_BORDER);
            if (i < s->inputLen)
                DrawCircle(bx + i * 44 + 18, by + 25, 10, CLR_GREEN);
        }
        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen == MAX_PIN)
        {
            strcpy_s(s->signupPin, sizeof(s->signupPin), s->inputBuf);
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->adminStep = 2;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else
    {
        DrawCenteredText(font, "INITIAL BALANCE (PKR):", 140, 18, CLR_DIM);
        char disp[32];
        snprintf(disp, sizeof(disp), "PKR %s%s", s->inputBuf, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen > 0)
        {
            string newID = atm.generateUserID();
            strcpy_s(s->generatedID, sizeof(s->generatedID), newID.c_str());

            double bal = atof(s->inputBuf);
            Account *newAcc = new Account(newID, string(s->signupName), bal);
            newAcc->setPin(string(s->signupPin));
            newAcc->saveToFile();

            atm.addAccountToArray(newAcc); // save

            // show generated ID
            char msg[32];
            snprintf(msg, sizeof(msg), "CREATED! ID: %s", newID.c_str());
            ShowMessage(s, msg);

            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->signupName[0] = '\0';
            s->adminStep = 0;
            s->screen = SCR_ADMIN;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "CREATE", font, 0);
    }

    if (IsButtonPressed(BTN_X, 410, BTN_W, BTN_H))
    {
        s->adminStep = 0;
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->signupName[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(BTN_X, 410, BTN_W, BTN_H, "CANCEL", font, 0);
}

// Admin creates a new admin,auto-generates admin ID, shows it
void DrawAdminSignup(AppState *s, Font font)
{
    DrawCenteredText(font, "ADD NEW ADMIN", 80, 24, CLR_GREEN);

    if (s->adminStep == 0)
    {
        DrawCenteredText(font, "ENTER ADMIN NAME:", 140, 18, CLR_DIM);
        char disp[64];
        snprintf(disp, sizeof(disp), "%s%s", s->signupName, s->cursorBlink ? "_" : " ");
        DrawCenteredText(font, disp, 200, 22, CLR_GREEN);

        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->signupName[0] != '\0')
        {
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->adminStep = 1;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "NEXT", font, 0);
    }
    else
    {
        DrawCenteredText(font, "SET 4-DIGIT PIN:", 140, 18, CLR_DIM);
        int bx = SCREEN_W / 2 - 98, by = 220;
        for (int i = 0; i < MAX_PIN; i++)
        {
            DrawRectangle(bx + i * 44, by, 36, 50, CLR_CARD);
            DrawRectangleLinesEx((Rectangle){(float)(bx + i * 44), (float)by, 36, 50}, 2, CLR_BORDER);
            if (i < s->inputLen)
                DrawCircle(bx + i * 44 + 18, by + 25, 10, CLR_GREEN);
        }
        if (IsButtonPressed(BTN_X, 330, BTN_W, BTN_H) && s->inputLen == MAX_PIN)
        {
            string newID = atm.generateAdminID();
            strcpy_s(s->generatedID, sizeof(s->generatedID), newID.c_str());

            // save to file
            ofstream outfile("admin_record.txt", ios::app);
            outfile << newID << " " << s->signupName << " " << s->inputBuf << endl;
            outfile.close();

            // add to memory
            Admin *newAdm = new Admin(string(s->signupName), newID, string(s->inputBuf));
            atm.addAdminToArray(newAdm);

            // show generated ID on screen
            s->inputLen = 0;
            s->inputBuf[0] = '\0';
            s->signupName[0] = '\0';
            s->adminStep = 0;

            // display generated ID via message
            char msg[32];
            snprintf(msg, sizeof(msg), "ADMIN CREATED! ID: %s", newID.c_str());
            ShowMessage(s, msg);
            s->screen = SCR_ADMIN;
        }
        DrawGreenButton(BTN_X, 330, BTN_W, BTN_H, "CREATE ADMIN", font, 0);
    }

    if (IsButtonPressed(BTN_X, 410, BTN_W, BTN_H))
    {
        s->adminStep = 0;
        s->inputLen = 0;
        s->inputBuf[0] = '\0';
        s->signupName[0] = '\0';
        s->screen = SCR_ADMIN;
    }
    DrawGreenButton(BTN_X, 410, BTN_W, BTN_H, "BACK", font, 0);
}