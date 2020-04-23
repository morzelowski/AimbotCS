#include <iostream>
#define _WIN32_WINNT 0x0500 //RIGHT
#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <tlhelp32.h>
#include <vector>
#include <string.h>
#include <thread>
#include "haze.h"
#include "Point.h"
#include "Player.h"
#include "Process.h"

DWORD GetModuleBaseAddress(LPSTR ModuleName, DWORD pid)
{
    MODULEENTRY32 module32;

    std::string helper;

    module32.dwSize = sizeof(MODULEENTRY32);
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    Module32First(hSnap, &module32);
    if (hSnap == INVALID_HANDLE_VALUE)
        return 0;
    do
    {
        for (int i = 0; i < 256; i++)
        {
            if ((CHAR)module32.szModule[i] != 0)
            {
                helper = helper + (CHAR)module32.szModule[i];
            }
        }

        if (strcmp(helper.c_str(), ModuleName) == 0)
        {
            break;
        }
        helper = "";
    } while (Module32Next(hSnap, &module32));
    CloseHandle(hSnap);

    return (DWORD)module32.modBaseAddr;
}

using namespace std;
using namespace hazedumper;

int GetClosest(Player entity[64], Player myplayer, int maxplayer)
{
    float clo_dist = 100000;
    int enemy_return = 1;
    float dist;

    for (int i = 1; i <= maxplayer; i++)
    {
        dist = myplayer.pos.distance(entity[i].pos);
        if (dist < clo_dist && entity[i].hp > 0 && entity[i].team != myplayer.team)
        {
            clo_dist = dist;
            enemy_return = i;
        }
        dist = 0;
    }
    return enemy_return;
}

vector<float> Aim(Player entity[64], Player myplayer, int closest)
{
    Point angles;
    float delta[3] = { (myplayer.pos.x - entity[closest].pos.x), (myplayer.pos.y - entity[closest].pos.y), (myplayer.pos.z - entity[closest].pos.z) };
    float hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]);
    angles.x = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
    angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
    angles.z = 0.0f;
    if (delta[0] >= 0.0) { angles.y += 180.0f; }
    if (angles.y > 180) { angles.y = angles.y - 360; }

    vector<float> angles_vec;
    angles_vec.push_back(angles.x);
    angles_vec.push_back(angles.y);
    if (angles.y > -180 && angles.y <= 180 && angles.x <= 89 && angles.x > -89)
        return angles_vec;
    else
    {
        return { 10,0 };
    }
}

void Attack()
{
    keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 0, 0);
    this_thread::sleep_for(25ms);
    keybd_event(VK_F1, MapVirtualKey(VK_F1, 0), 2, 0);
    this_thread::sleep_for(25ms);
}

int main()
{
    SYSTEMTIME st;
    int moment = NULL;

    bool ifplay = false;
    int maxplayer = 0;
    Player entity[64];

    int tryb = 2;

    DWORD pid;
    HWND window;
    window = FindWindow(0, L"Counter-Strike: Global Offensive");
    if (window != NULL)
    {
        GetWindowThreadProcessId(window, &pid);
        Process process(pid);
        if (process.get() != NULL)
        {
            DWORD clientmodule = GetModuleBaseAddress((LPSTR)"client_panorama.dll", pid);
            DWORD engine = GetModuleBaseAddress((LPSTR)"engine.dll", pid);

            while (true)
            {
                if (GetAsyncKeyState(VK_UP))
                    ifplay = true;

                if (GetAsyncKeyState(VK_F2))
                {
                    tryb = 2;//ak
                }
                if (GetAsyncKeyState(VK_F4))
                {
                    tryb = 1;//sniper
                }

                if (ifplay == true)
                {
                    DWORD dwclient;

                    DWORD player;
                    ReadProcessMemory(process.get(), (LPCVOID)(clientmodule + signatures::dwLocalPlayer), (PVOID)&player, sizeof(player), 0);

                    Player myplayer;
                    ReadProcessMemory(process.get(), (LPCVOID)(engine + signatures::dwClientState), (PVOID)&dwclient, sizeof(dwclient), 0);

                    DWORD basebone;
                    ReadProcessMemory(process.get(), (LPCVOID)(player + netvars::m_dwBoneMatrix), (PVOID)&basebone, sizeof(basebone), 0);

                    while (true)
                    {

                        GetSystemTime(&st);
                        if (st.wSecond % 3 == 0 && moment != st.wSecond)
                        {
                            moment = st.wSecond;
                            for (int i = 1; i < 64; i++)
                            {
                                entity[i].shooted = false;
                            }
                        }

                        if (GetAsyncKeyState(VK_DOWN))
                        {
                            ifplay = false;
                            break;
                        }

                        if (GetAsyncKeyState(VK_F2))
                        {
                            tryb = 2;//ak
                        }
                        if (GetAsyncKeyState(VK_F4))
                        {
                            tryb = 1;//sniper
                        }

                        float pos_add[3];

                        ReadProcessMemory(process.get(), (LPCVOID)(player + netvars::m_iHealth), (LPVOID)&myplayer.hp, sizeof(myplayer.hp), 0);
                        ReadProcessMemory(process.get(), (LPCVOID)(player + netvars::m_iTeamNum), (PVOID)&myplayer.team, sizeof(myplayer.team), 0);
                        ReadProcessMemory(process.get(), (LPCVOID)(player + netvars::m_vecOrigin), (PVOID)&myplayer.pos, sizeof(myplayer.pos), 0);
                        ReadProcessMemory(process.get(), (LPCVOID)(player + netvars::m_vecViewOffset), (PVOID)&pos_add, sizeof(pos_add), 0);

                        myplayer.pos.x += pos_add[0];
                        myplayer.pos.y += pos_add[1];
                        myplayer.pos.z += pos_add[2];

                        ReadProcessMemory(process.get(), (LPCVOID)(dwclient + signatures::dwClientState_ViewAngles), (PVOID)&pos_add, sizeof(pos_add), 0);

                        myplayer.ang.x = pos_add[0];
                        myplayer.ang.y = pos_add[1];
                        myplayer.ang.z = pos_add[2];

                        maxplayer = 0;

                        for (int i = 1; i < 64; i++)
                        {
                            DWORD ent;
                            ReadProcessMemory(process.get(), (LPCVOID)(clientmodule + signatures::dwEntityList + (0x10 * i)), (PVOID)&ent, sizeof(ent), 0);
                            if (ent != NULL)
                            {
                                maxplayer++;

                                ReadProcessMemory(process.get(), (LPCVOID)(ent + netvars::m_iTeamNum), (PVOID)&entity[i].team, sizeof(entity[i].team), 0);
                                ReadProcessMemory(process.get(), (LPCVOID)(ent + netvars::m_iHealth), (PVOID)&entity[i].hp, sizeof(entity[i].hp), 0);
                                ReadProcessMemory(process.get(), (LPCVOID)(ent + netvars::m_bSpotted), (PVOID)&entity[i].isSpotted, sizeof(entity[i].isSpotted), 0);

                                ReadProcessMemory(process.get(), (LPCVOID)(ent + netvars::m_vecOrigin), (PVOID)&entity[i].pos, sizeof(entity[i].pos), 0);
                                DWORD basebone;
                                ReadProcessMemory(process.get(), (LPCVOID)(ent + netvars::m_dwBoneMatrix), (PVOID)&basebone, sizeof(basebone), 0);
                                ReadProcessMemory(process.get(), (LPCVOID)(basebone + (tryb * 4) * 0x30 + 0x0C), (PVOID)&entity[i].pos.x, sizeof(entity[i].pos.x), 0);
                                ReadProcessMemory(process.get(), (LPCVOID)(basebone + (tryb * 4) * 0x30 + 0x1C), (PVOID)&entity[i].pos.y, sizeof(entity[i].pos.y), 0);
                                ReadProcessMemory(process.get(), (LPCVOID)(basebone + (tryb * 4) * 0x30 + 0x2C), (PVOID)&entity[i].pos.z, sizeof(entity[i].pos.z), 0);
                            }
                        }

                        float minchange = 100;
                        int closestentity = 0;

                        for (int i = 1; i <= maxplayer; i++)
                        {
                            vector<float> angle = Aim(entity, myplayer, i);
                            float angles[3];
                            angles[0] = angle[0];
                            angles[1] = angle[1];
                            angles[2] = 0;
                            float changex = (myplayer.ang.x - angles[0]);
                            float changey = (myplayer.ang.y - angles[1]);
                            if ((abs(changex) + abs(changey)) / 2 < minchange && entity[i].team != myplayer.team && entity[i].hp > 0 && entity[i].isSpotted == true && entity[i].shooted == false)
                            {
                                minchange = (abs(changex) + abs(changey)) / 2;
                                closestentity = i;
                            }

                        }

                        if (closestentity != 0)
                        {
                            vector<float> angle = Aim(entity, myplayer, closestentity);
                            float angles[3];
                            angles[0] = angle[0];
                            angles[1] = angle[1];
                            angles[2] = 0;
                            float changex = (myplayer.ang.x - angles[0]);
                            float changey = (myplayer.ang.y - angles[1]);

                            if (GetAsyncKeyState(VK_LBUTTON))
                            {
                                if (abs(changex) < 3 * (3 - tryb) && abs(changey) < 3 * (3 - tryb) && entity[closestentity].team != myplayer.team && entity[closestentity].hp > 0 && entity[closestentity].isSpotted == true && entity[closestentity].shooted == false)
                                {
                                    for (int i = 0; i < 5; i++)
                                    {
                                        myplayer.ang.x -= changex / 5;
                                        myplayer.ang.y -= changey / 5;

                                        float ang[3];
                                        ang[0] = myplayer.ang.x;
                                        ang[1] = myplayer.ang.y;
                                        ang[2] = myplayer.ang.z;

                                        Process process_write(pid, true);
                                        WriteProcessMemory(process_write.get(), (LPVOID)(dwclient + signatures::dwClientState_ViewAngles), ang, sizeof(ang), 0);

                                        this_thread::sleep_for(2ms);
                                    }
                                    entity[closestentity].shooted = true;
                                }
                                Attack();
                            }
                        }
                        else
                        {
                            if (GetAsyncKeyState(VK_LBUTTON))
                            {
                                Attack();
                            }
                        }
                        this_thread::sleep_for(1ms);
                    }
                }
                else
                {
                    if (GetAsyncKeyState(VK_LBUTTON))
                    {
                        Attack();
                    }
                }
            }
        }
        else
        {
            cout << "Nie znaleziono procesu cs:go";
        }
    }
    else
    {
        cout << "Nie znaleziono okna cs:go";
    }
    return 0;
}
