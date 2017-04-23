#include "stdafx.h"

/*BUGS:
1.Неизвестная утечка памяти.
Предположения:
1.Неполное удаление из контекста устройства выбранных в него обьектов

2.Влияние(временное ускорение основной анимации) второго таймера на основную анимацию.//РЕШЕНО
Предположения:
1.Заменить таймер, на что-то другое.

3.Влияние счетчика end на анимацию и установку начальных точек для врагов.//РЕШЕНО

4.Невыпонение условий анимации врага в граничных точках секторов, при очень быстром перемещении игрока
Предположения
1.Пересмотрть условия анимации
5.Отображение одного взрыва при наличии двух.

2меню -
1меню +
повреждения игрока +
счет убитых врагов +
синхронизация после респавна +
босс -
взрывы +

*/
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <list>
#include "resource.h"

using namespace std;

//режимы разбиения прямоугольника на секторы
#define AI_BOTTOM 0  //not work
#define AI_TOP 1     //work
#define AI_LEFT 2    //not work
#define AI_RIGHT 3   //not work
//режимы разбиения прямоугольника на секторы

//Направление выстрелов пушки
#define GUN_UP 0     //work
#define GUN_DOWN 1   //work
//Направление выстрелов пушки

#define BSMOKE_WHITE 0
#define BSMOKE_RED 1
#define BSMOKE_GREEN 2
#define BSMOKE_BLUE 3

#define BOSS_PORTAL_TIMER 1000

unsigned mytimer;
unsigned kills;
unsigned m_kill;
unsigned pl_health;

RECT GetMainRect();

int GetPercentFromValue(int value, int percent);

RECT GetRectFromPoint(RECT, POINT, unsigned);

bool IsPointInRect(RECT, POINT);

POINT mainRect[2] = { { 100,50 },{ 900,650 } };

int bsize;

class BOSS {
    POINT pos;
    bool status;
    unsigned health;
public:
    bool active;
    void draw_in_point(HDC, POINT);
    void set_boss(POINT);
    POINT get_pos() const;
};

POINT BOSS::get_pos() const {
    return pos;
}

void BOSS::set_boss(POINT p) {
    status = 1;
    health = 1000;
    pos = p;
    active = 0;
}

//void BOSS :: draw_in_point(HDC hdc, POINT pos);



class EXPLOSION {
    POINT expl_pos;
    HBRUSH expl_color_out;
    HPEN expl_out;
    bool status;
    unsigned speed;

    HBRUSH expl_color_inner;
    HPEN expl_inner;

    unsigned expl_radius;
public:
    EXPLOSION() { /*status = 1;*/ status = 0; }
    EXPLOSION(POINT, unsigned);
    void set_explosion(POINT, unsigned, bool);
    void explosion_anim(HDC);
    bool get_status();
};

bool EXPLOSION::get_status() {
    return status;
}

EXPLOSION::EXPLOSION(POINT pos, unsigned rad) {
    status = 1;
    expl_pos = pos;
    expl_radius = rad;
}

void EXPLOSION::set_explosion(POINT pos, unsigned rad, bool active = 1) {
    status = active;
    expl_pos = pos;
    expl_radius = rad;
}

void EXPLOSION::explosion_anim(HDC hdc) {
    static float i = 0, i2 = 0;

    if (status) {
        expl_color_out = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(236, 0, 0)));
        expl_out = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 56, 6)));
        Rectangle(hdc, expl_pos.x - i, expl_pos.y + i, expl_pos.x + i, expl_pos.y - i);
        DeleteObject(SelectObject(hdc, expl_color_out));
        DeleteObject(SelectObject(hdc, expl_out));

        expl_color_inner = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(255, 100, 100)));
        expl_inner = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 56, 56)));
        Rectangle(hdc, expl_pos.x - i2, expl_pos.y + i2, expl_pos.x + i2, expl_pos.y - i2);
        DeleteObject(SelectObject(hdc, expl_color_inner));
        DeleteObject(SelectObject(hdc, expl_inner));

        if (i >= expl_radius) { i = 0; i2 = 0; status = 0; }
        i += 1.5;
        i2 += 0.45;
    }
}


class BULLET {
    POINT start_bullet_pos;
    POINT pos_of_bullet;
    HPEN Pen;
    void DrawModel(HDC, POINT) const;
    void DrawSmoke(HDC, POINT, int, unsigned);
public:
    BULLET(POINT);
    void DrawInPoint(HDC, POINT, unsigned, unsigned);
    POINT GetPos() const;
};

BULLET::BULLET(POINT start_pos) {
    start_bullet_pos = pos_of_bullet = start_pos;
}

POINT BULLET::GetPos() const { return pos_of_bullet; }

void BULLET::DrawInPoint(HDC hdc, POINT pos, unsigned smoke_lenght, unsigned mode = BSMOKE_WHITE) {
    pos_of_bullet = pos;
    int smoke_pixels = start_bullet_pos.y - pos.y;
    if (smoke_pixels > 0 && (smoke_pixels > smoke_lenght + 1)) smoke_pixels = smoke_lenght;
    else if (smoke_pixels < 0 && (abs(smoke_pixels) > smoke_lenght + 1)) smoke_pixels = 0 - smoke_lenght;

    DrawSmoke(hdc, pos, smoke_pixels, mode);

    Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(255, 0, 0)));
    DrawModel(hdc, pos);
    DeleteObject(SelectObject(hdc, Pen));
}

void BULLET::DrawModel(HDC hdc, POINT pos) const {
    MoveToEx(hdc, pos.x, pos.y, NULL);
    LineTo(hdc, pos.x, pos.y + 3);
}

void BULLET::DrawSmoke(HDC hdc, POINT pos, int smoke_pixels, unsigned mode) {
    POINT smoke;
    int smoke_direction = 1;
    float white_to_black = 255;
    float color_change_speed = white_to_black / abs(smoke_pixels);

    if (smoke_pixels < 0) {
        smoke_pixels = abs(smoke_pixels);
        smoke_direction = -1;
    }
    smoke.x = pos.x;
    smoke.y = pos.y + smoke_direction;

    for (unsigned i = 0; i < smoke_pixels; i++) {
        switch (mode) {
        case BSMOKE_RED:
            SetPixel(hdc, smoke.x, smoke.y, RGB(white_to_black, 0, 0));
            break;
        case BSMOKE_GREEN:
            SetPixel(hdc, smoke.x, smoke.y, RGB(0, white_to_black, 0));
            break;
        case BSMOKE_BLUE:
            SetPixel(hdc, smoke.x, smoke.y, RGB(0, 0, white_to_black));
            break;
        default:
            SetPixel(hdc, smoke.x, smoke.y, RGB(white_to_black, white_to_black, white_to_black));
            break;
        }
        smoke.y += smoke_direction;
        white_to_black -= color_change_speed;
    }
}


class SWITCH {
    bool *mas;
    unsigned size;
public:
    SWITCH(unsigned);
    void SetActive(unsigned);
    bool IsActive(unsigned) const;
};

SWITCH::SWITCH(unsigned s = 10) {
    size = s;
    mas = new bool[size];
    for (unsigned i = 0; i < size; i++) mas[i] = 0;
}

void SWITCH::SetActive(unsigned index) {
    if (index >= size) return;

    for (unsigned i = 0; i < size; i++) {
        if (i == index)mas[i] = 1;
        else mas[i] = 0;
    }
}

bool SWITCH::IsActive(unsigned index) const {
    if (index < size) return mas[index];
    else return 0;
}

class ENEMY {
    int a;
    int b;
    unsigned health;
    SWITCH direction;
    POINT model[2];
    //bool status;
    bool target_detect;
    //bool respawned;
    //bool need_explosion;

    HPEN pen;
    HBRUSH brush;

    /*EXPLOSION *ship_destroy;
    unsigned expl_num;*/

    unsigned portal_pulse;
    unsigned respawn_time;
public:
    bool respawned;
    bool need_explosion;
    bool unset;
    bool status;
    ENEMY();
    void draw_enemy(HDC hdc);
    void set_model_pos(unsigned x, unsigned y);
    void anim_test(unsigned x, unsigned lim1, unsigned lim2);
    POINT get_Xt_Xb_pos() const;
    POINT get_gun_pos();
    bool get_status() const;
    RECT get_enemy_rect() const;
    void damage(unsigned damage);
    void Draw_respawn_portal(HDC);
    void after_respawn_sync(RECT, bool);
    bool get_direction();
    void set_direction(unsigned);
    POINT get_xy();
    void respawn(POINT, unsigned);
};

void ENEMY::respawn(POINT pos, unsigned direct) {
    health = 100;
    respawned = 0;
    status = 1;
    set_model_pos(pos.x, pos.y);
    direction.SetActive(direct);
    need_explosion = 1;
}

POINT ENEMY::get_xy() {
    POINT xy;
    xy.x = (model[1].x + model[0].x) / 2;
    xy.y = (model[1].y + model[0].y) / 2;

    return xy;
}

void ENEMY::set_direction(unsigned  direc) {
    direction.SetActive(direc);
}

bool ENEMY::get_direction() {
    bool index = false;
    direction.IsActive(0);
    if (index) return index;
    else {
        direction.IsActive(1);
        return index;
    }
}

ENEMY::ENEMY() {
    respawned = 0;
    srand(time(NULL));
    a = 20;
    b = 20;
    portal_pulse = 0;
    target_detect = status = 1;
    direction.SetActive(0);
    health = 100;
    need_explosion = 1;
    unset = 0;
}

void ENEMY::Draw_respawn_portal(HDC hdc) {

    respawn_time = 5 + rand() % 35;

    if (!status && !unset) {
        pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(0, 23, 40)));
        brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 35, 62)));
        Ellipse(hdc, model[1].x, model[1].y - 7, model[0].x, model[0].y + 7);
        DeleteObject(SelectObject(hdc, pen));
        DeleteObject(SelectObject(hdc, brush));
        pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(0, 23, 40)));
        brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 49, 85)));
        Ellipse(hdc, model[1].x - portal_pulse, model[1].y - 7 - portal_pulse, model[0].x + portal_pulse, model[0].y + 7 + portal_pulse);
        DeleteObject(SelectObject(hdc, pen));
        DeleteObject(SelectObject(hdc, brush));


        if (mytimer > respawn_time /*&& i < 8*/) { portal_pulse++; mytimer = 0; }
        if (portal_pulse > 10) { portal_pulse = 0; respawned = 1; }
    }

}

void ENEMY::damage(unsigned damage) {
    POINT expl_pos;
    RECT enemy = get_enemy_rect();

    if (health) health -= damage;
    else if (health == 0) {
        /*for(int i = 0; i < expl_num; i++){
        expl_pos.x = enemy.top - rand()%15;
        expl_pos.y = enemy.left - rand()%15;
        ship_destroy[i].set_explosion(model[0],20);
        }*/
        status = 0;
        //kills++;
    }
}

RECT ENEMY::get_enemy_rect() const {
    RECT pos;

    pos.left = model[0].x;
    pos.top = model[0].y;
    pos.right = model[1].x;
    pos.bottom = model[1].y;

    return pos;
}

POINT ENEMY::get_gun_pos() {
    POINT pos;

    pos.y = model[1].y + 3;
    pos.x = (model[0].x + model[1].x) / 2;

    return pos;
}

bool ENEMY::get_status() const {
    return status;
}

POINT ENEMY::get_Xt_Xb_pos() const {
    POINT pos;
    pos.x = model[0].x;
    pos.y = model[1].x;

    return pos;
}

void ENEMY::anim_test(unsigned x, unsigned lim1, unsigned lim2) {
    if (status && !unset) {
        if (x > lim1 && x < lim2) {
            target_detect = 1;
            if ((x >(model[0].x + model[1].x) / 2) && (model[0].x > lim1) && (model[1].x < lim2)) { model[0].x += 4; model[1].x += 4; }
            else if ((x < (model[0].x + model[1].x) / 2) && (model[0].x > lim1) && (model[1].x < lim2)) { model[0].x -= 4; model[1].x -= 4; }
            else if ((x < (model[0].x + model[1].x) / 2) && (model[0].x > lim1) && (model[1].x <= lim2 + 6)) { model[0].x -= 4; model[1].x -= 4; }
            else if ((x >(model[0].x + model[1].x) / 2) && (model[0].x >= lim1 - 6) && (model[1].x < lim2)) { model[0].x += 4; model[1].x += 4; }
        }
        else if (x < lim1 || x > lim2) {
            target_detect = 0;
            if (direction.IsActive(0) && (model[0].x > lim1) && (model[1].x < lim2)) { model[0].x += 2; model[1].x += 2; }
            else if (direction.IsActive(0) && (model[0].x > lim1) && (model[1].x <= lim2 + 6)) { direction.SetActive(1); model[0].x -= 2; model[1].x -= 2; }
            else if (direction.IsActive(1) && (model[0].x > lim1) && (model[1].x < lim2)) { model[0].x -= 2; model[1].x -= 2; }
            else if (direction.IsActive(1) && (model[0].x >= lim1 - 6) && (model[1].x < lim2)) { direction.SetActive(0); model[0].x += 2; model[1].x += 2; }
        }
    }
}

void ENEMY::set_model_pos(unsigned x, unsigned y) {
    model[0].x = x - a;
    model[0].y = y - b;
    model[1].x = x + a;
    model[1].y = y + b;
}

void ENEMY::draw_enemy(HDC hdc) {
    if (status && !unset) {
        Rectangle(hdc, model[0].x, model[0].y, model[1].x, model[1].y);
        SetPixel(hdc, (model[0].x + model[1].x) / 2, (model[0].y + model[1].y) / 2, RGB(255, 0, 0));
    }
}

class PLAYER {
    POINT player_pos;
    unsigned health;
public:
    bool immortal;
    PLAYER();
    void set_pos(POINT);
    void set_posX(unsigned);
    POINT get_pos();
    void damage(unsigned);
    unsigned get_health();
};

PLAYER::PLAYER() {
    health = 100;
    immortal = 0;
}

unsigned PLAYER::get_health() {
    return health;
}

void PLAYER::damage(unsigned d) {
    if (health && !immortal)health -= d;
}

void PLAYER::set_posX(unsigned x) {
    player_pos.x = x;
}

void PLAYER::set_pos(POINT p) {
    player_pos = p;
}

POINT PLAYER::get_pos() {
    return player_pos;
}

class STATS {
    HPEN Pen;
    HBRUSH Brush;
    RECT rectGraph;
    void DrawShip(HDC hdc);
    unsigned r_s, g_s, b_s;
    unsigned r_c, g_c, b_c;
public:
    STATS();
    void DrawGraphStats(HDC hdc);
    void SetShieldColor(COLORREF);
    void SetCaseColor(COLORREF);
    void UpdateStat(HWND);
    void GetToDraw(HDC);
};

void STATS::UpdateStat(HWND hwnd) {
    InvalidateRect(hwnd, &rectGraph, NULL);
}

void STATS::SetShieldColor(COLORREF color) {
    r_s = GetRValue(color);
    g_s = GetGValue(color);
    b_s = GetBValue(color);
}

void STATS::SetCaseColor(COLORREF color) {
    r_c = GetRValue(color);
    g_c = GetGValue(color);
    b_c = GetBValue(color);
}

STATS::STATS() {
    rectGraph.left = mainRect[0].x / 8;
    rectGraph.top = mainRect[1].y / 2;
    rectGraph.right = mainRect[0].x - 1;
    rectGraph.bottom = mainRect[1].y - 200;
    g_s = 255;
    r_s = b_s = 0;
    g_c = 200;
    r_c = b_c = 0;
}

void STATS::DrawGraphStats(HDC hdc) {
    Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(0, 0, 100)));
    Brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 0, 0)));
    Rectangle(hdc, rectGraph.left, rectGraph.top, rectGraph.right, rectGraph.bottom);
    DeleteObject(SelectObject(hdc, Pen));
    DeleteObject(SelectObject(hdc, Brush));
    DrawShip(hdc);
}

void STATS::DrawShip(HDC hdc) {
    POINT middle;
    RECT model = { 0 };
    COLORREF oldBkColor;
    int oldBkMode;
    middle.x = (rectGraph.left + rectGraph.right) / 2;
    middle.y = (rectGraph.top + rectGraph.bottom) / 2;

    model = GetRectFromPoint(model, middle, 50);

    Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(r_s, g_s, b_s)));
    Brush = (HBRUSH)SelectObject(hdc, CreateHatchBrush(HS_DIAGCROSS, RGB(r_s, g_s, b_s)));
    oldBkMode = SetBkMode(hdc, OPAQUE);
    oldBkColor = SetBkColor(hdc, RGB(r_c, g_c, b_c));
    Rectangle(hdc, model.left, model.top, model.right, model.bottom);
    SetBkColor(hdc, oldBkColor);
    SetBkMode(hdc, oldBkMode);
    DeleteObject(SelectObject(hdc, Pen));
    DeleteObject(SelectObject(hdc, Brush));
}

STATS gameStat;

class AI {
    unsigned sectors_num;
    unsigned sector_lenght;
    unsigned sector_height;
    unsigned sector_middle;
    RECT *sectors;
    unsigned mode;
    unsigned enemy_num;
    POINT *respawn_point;
    ENEMY *en_ship;


    list<BULLET> *gun;

    PLAYER player;
    COLORREF player_color;
    RECT player_ship;
    unsigned player_ship_size;
    void draw_player(HDC);
    void draw_player_fire(HDC);
    unsigned pl_start_health;
    unsigned kills;
    unsigned max_kills;

    unsigned active_guns;
    list<EXPLOSION> explosions;

    HPEN Pen;
    HBRUSH Brush;

    void division(unsigned max_coord, unsigned min_coord, unsigned top, unsigned bottom);
    void set_respawn_points();
    unsigned set_sector_middle(unsigned a, unsigned b);
    POINT set_size(HDC hdc, POINT pos);
    void hit(list<BULLET> &g);
    void draw_bullets(HDC, list<BULLET> &, unsigned, unsigned, unsigned, unsigned);
    void enemy_control();
    void draw_explosions(HDC);
    void calc_explosions();
    void get_expl(EXPLOSION &, RECT, unsigned);
    void stat_control();

    BOSS bs;
    POINT boss_model[6];
    void boss_setup();
    void boss_portal_draw(HDC);
public:
    bool boss;
    bool init_boss;
    bool boss_timer;
    void start_game(HDC hdc, unsigned sect, unsigned m);
    void enemy_attack();

    void player_pos_set(unsigned posX);
    void player_gun_shoot();

    void main_calc();
    void main_draw(HDC);

    int expl_size();
    void enemy_respawn_sync();
    void enemy_respawn_in_sector();
    void player_immortal();
    unsigned get_kills();
    void set_player_color(COLORREF);
    void start_boss_demo();
};

void AI::draw_player_fire(HDC hdc) {
    //RECT fire = player_ship;

    //fire.left = fire.left  +15/*GetPercentFromValue(fire.left,30)*/;
    //fire.top = fire.top + 20;//+ /*GetPercentFromValue(fire.top,30)*/;
    //fire.right = fire.right - 15/*GetPercentFromValue(fire.right,30)*/;
    //fire.bottom = fire.bottom + 20 /*GetPercentFromValue(fire.bottom,30)*/;

    Pen = (HPEN)SelectObject(hdc, GetStockObject(DC_PEN));

    static int red = 255;
    int size = 30;
    for (int i = 0; i < size; i++) {
        SetDCPenColor(hdc, RGB(red, 0, 0));
        red--;
        Arc(hdc, player_ship.left + i + 15, player_ship.top + i + 20, player_ship.right - i - 15, player_ship.bottom - i + 20, 500, 500, 500, 500);
    }
    DeleteObject(SelectObject(hdc, Pen));
}

void AI::start_boss_demo() {
    kills = 10000;
}

void AI::set_player_color(COLORREF c) {
    player_color = c;
}

void AI::boss_portal_draw(HDC hdc) {
    POINT portal_center = bs.get_pos();
    static unsigned portal_shield_rad = 1;
    RECT shield;
    static unsigned main_portal = 1;
    static float inner_portal = 1;
    static int boss_from_portal = GetPercentFromValue(sector_height, 50);
    POINT inner_lines;
    static RECT main_portal_form;
    static RECT inner_portal_form;

    shield.top = portal_center.y - GetPercentFromValue(sector_height, 80);
    shield.bottom = portal_center.y + GetPercentFromValue(sector_height, 80);
    shield.left = portal_center.x - GetPercentFromValue(sector_height, 80);
    shield.right = portal_center.x + GetPercentFromValue(sector_height, 80);

    main_portal_form.top = portal_center.y - GetPercentFromValue(sector_height, 10);
    main_portal_form.bottom = portal_center.y + GetPercentFromValue(sector_height, 10);
    main_portal_form.left = portal_center.x - GetPercentFromValue(sector_height, 10);
    main_portal_form.right = portal_center.x + GetPercentFromValue(sector_height, 10);

    inner_portal_form = main_portal_form;

    Pen = (HPEN)SelectObject(hdc, GetStockObject(DC_PEN));
    for (int i = 0; i < portal_shield_rad; i++) {
        SetDCPenColor(hdc, RGB(0, i, 0));
        Arc(hdc, shield.left - i, shield.top - i, shield.right + i, shield.bottom + i, 500, 500, 500, 500);
    }
    DeleteObject(SelectObject(hdc, Pen));
    if (portal_shield_rad < 100) portal_shield_rad++;

    if (portal_shield_rad > 50) {
        Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(255, 67, 20)));
        Brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(255, 67, 20)));
        Ellipse(hdc, main_portal_form.left - main_portal, main_portal_form.top - main_portal, main_portal_form.right + main_portal, main_portal_form.bottom + main_portal);
        if (main_portal < 80)main_portal++;
        DeleteObject(SelectObject(hdc, Pen));
        DeleteObject(SelectObject(hdc, Brush));
    }

    if (main_portal > 50) {
        Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(175, 46, 14)));
        Brush = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(175, 46, 14)));
        Ellipse(hdc, main_portal_form.left - inner_portal, main_portal_form.top - inner_portal, main_portal_form.right + inner_portal, main_portal_form.bottom + inner_portal);
        for (int i = 0; i < 6; i++) {
            inner_lines = en_ship[i].get_xy();
            MoveToEx(hdc, inner_lines.x, inner_lines.y, NULL);
            LineTo(hdc, portal_center.x, portal_center.y);
        }
        DeleteObject(SelectObject(hdc, Pen));
        DeleteObject(SelectObject(hdc, Brush));

        Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 1, RGB(142, 28, 204)));
        //Brush = SelectObject(hdc,CreateSolidBrush(RGB(12,0,50)));

        MoveToEx(hdc, boss_model[0].x, boss_model[0].y + boss_from_portal, NULL);
        LineTo(hdc, boss_model[1].x + boss_from_portal, boss_model[1].y);
        LineTo(hdc, boss_model[2].x + boss_from_portal, boss_model[2].y - boss_from_portal);
        LineTo(hdc, boss_model[3].x, boss_model[3].y - boss_from_portal);
        LineTo(hdc, boss_model[4].x - boss_from_portal, boss_model[4].y - boss_from_portal);
        LineTo(hdc, boss_model[5].x - boss_from_portal, boss_model[5].y);
        LineTo(hdc, boss_model[0].x, boss_model[0].y + boss_from_portal);

        if (boss_from_portal > 0)boss_from_portal--;
        if (inner_portal < 60) { inner_portal += 0.5; }
        DeleteObject(SelectObject(hdc, Pen));
        //DeleteObject(SelectObject(hdc,Brush));
    }
    if (inner_portal > 60) { bs.active = 1; boss_timer = 0; init_boss = 0; }
}

void AI::boss_setup() {
    POINT main_rect_middle;
    //POINT boss_model[6];
    sector_height = sectors[0].bottom - sectors[0].top;
    main_rect_middle.x = (mainRect[0].x + mainRect[1].x) / 2;
    main_rect_middle.y = (sectors[0].top + sectors[0].bottom) / 2 + 25;

    /*boss_model[0].x = boss_model[1].x = main_rect_middle.x;
    boss_model[0].y = main_rect_middle.y - GetPercentFromValue(sector_height,60);
    boss_model[1].y = main_rect_middle.y + GetPercentFromValue(sector_height,60);
    boss_model[2].x = boss_model[4].x = main_rect_middle.x - GetPercentFromValue(sector_lenght,50);
    boss_model[3].x = boss_model[5].x = main_rect_middle.x + GetPercentFromValue(sector_lenght,50);
    boss_model[2].y = boss_model[3].y = main_rect_middle.y - GetPercentFromValue(sector_height,25);
    boss_model[4].y = boss_model[5].y = main_rect_middle.y + GetPercentFromValue(sector_height,25);
    */
    boss_model[0].x = boss_model[3].x = main_rect_middle.x;
    boss_model[0].y = main_rect_middle.y - GetPercentFromValue(sector_height, 60);
    boss_model[3].y = main_rect_middle.y + GetPercentFromValue(sector_height, 60);
    boss_model[1].x = boss_model[2].x = main_rect_middle.x - GetPercentFromValue(sector_lenght, 50);
    boss_model[4].x = boss_model[5].x = main_rect_middle.x + GetPercentFromValue(sector_lenght, 50);
    boss_model[1].y = boss_model[5].y = main_rect_middle.y - GetPercentFromValue(sector_height, 25);
    boss_model[2].y = boss_model[4].y = main_rect_middle.y + GetPercentFromValue(sector_height, 25);



    if (kills > max_kills) {
        boss = 1; for (int i = 0; i < 6; i++) {
            en_ship[i].status = 0;
            en_ship[i].set_model_pos(boss_model[i].x, boss_model[i].y);
        }
        for (int i = 6; i < enemy_num; i++) {
            en_ship[i].unset = 1;
        }
    }
    bs.set_boss(main_rect_middle);
    boss_timer = 1;
}

unsigned AI::get_kills() {
    return kills;
}

void AI::player_immortal() {
    static bool n = 1;
    if (n) { player.immortal = 1; n = 0; }
    else { player.immortal = 0; n = 1; }
}

void AI::stat_control() {
    pl_health = player.get_health();
    if (player.immortal) { gameStat.SetCaseColor(RGB(181, 203, 255)); gameStat.SetShieldColor(RGB(53, 90, 255)); }
    else if (player.get_health() > GetPercentFromValue(pl_start_health, 85) && !player.immortal) { gameStat.SetCaseColor(RGB(0, 200, 0)); gameStat.SetShieldColor(RGB(0, 255, 0)); }
    else if (player.get_health() > GetPercentFromValue(pl_start_health, 70) && !player.immortal) { gameStat.SetShieldColor(RGB(255, 194, 63)); gameStat.SetCaseColor(RGB(0, 200, 0)); }
    else if (player.get_health() > GetPercentFromValue(pl_start_health, 40) && !player.immortal) { gameStat.SetShieldColor(RGB(255, 40, 2)); gameStat.SetCaseColor(RGB(0, 200, 0)); }
    else if (player.get_health() > GetPercentFromValue(pl_start_health, 25) && !player.immortal) { gameStat.SetCaseColor(RGB(255, 194, 63)); gameStat.SetShieldColor(RGB(255, 40, 2)); }
    else if (player.get_health() > GetPercentFromValue(pl_start_health, 0) && !player.immortal) { gameStat.SetCaseColor(RGB(198, 31, 1)); gameStat.SetShieldColor(RGB(255, 40, 2)); }
    else if (player.get_health() <= 0) { gameStat.SetCaseColor(RGB(0, 0, 0)); gameStat.SetShieldColor(RGB(128, 128, 128)); }
}

void AI::get_expl(EXPLOSION &n, RECT zone, unsigned power) {
    //EXPLOSION n;
    POINT rand_pos;
    srand(time(NULL));

    rand_pos.x = zone.left + rand() % (zone.right - zone.left);
    rand_pos.y = zone.top + rand() % (zone.bottom - zone.top);
    n.set_explosion(rand_pos, power, 1);

    //return n;
}

void AI::enemy_respawn_in_sector() {

    if (kills < max_kills) {
        static bool *ready_to_respawn = new bool[enemy_num];
        //unsigned *ready = new unsigned[sectors_num];
        unsigned start = 0, end = 3;
        unsigned direction;
        POINT pos;

        for (int i = 0; i < sectors_num; i++) {
            for (int start = 0; start < end; start++) {
                if (en_ship[start].respawned)ready_to_respawn[start] = 1;
                else ready_to_respawn[start] = 0;
            }
            //for(int k = start; k < end; k++) if(ready_to_respawn[k]) ready[i]++;
            if (ready_to_respawn[start] && ready_to_respawn[start + 1] && ready_to_respawn[start + 2]) {
                srand(time(NULL));
                direction = rand() % 1;
                for (int j = start; j < end; j++) {
                    pos = en_ship[j].get_xy();
                    pos.x = (sectors[i].left + sectors[i].right) / 2;
                    en_ship[j].respawn(pos, direction);
                }
            }
            start = end;
            end += 3;
        }
    }
}

void AI::enemy_respawn_sync() {
    POINT pos_2;
    POINT *pos_1 = new POINT[sectors_num];
    //bool active = 0;
    bool *active = new bool[sectors_num];
    unsigned *direction = new unsigned[sectors_num];
    unsigned start = 0, end = 3;

    for (int i = 0; i < sectors_num; i++) {
        for (int start = 0; start < end; start++) {
            /*if(!en_ship[start].get_status() /*&& en_ship[start].respawned*///) {
                                                                             /* for(int j = start; j < end; j++){
                                                                             if(en_ship[j].get_status()) { pos_1[i] = en_ship[j].get_xy(); direction[i] = en_ship[j].get_direction(); active[i] = 1;}
                                                                             if(!active[i]){
                                                                             pos_1[i].x = (sectors[i].left + sectors[i].right) / 2;
                                                                             direction[i] = 0;
                                                                             //en_ship[j].set_model_pos(pos_1.x, pos_1.y);
                                                                             }

                                                                             }
                                                                             if(en_ship[start].respawned) {
                                                                             pos_2 = en_ship[start].get_xy();
                                                                             pos_2.x = pos_1[i].x;
                                                                             en_ship[start].respawn(pos_2,direction[i]);}
                                                                             }                                                   */
        }
        start = end;
        end += 3;
    }
}

int AI::expl_size() {
    return explosions.size();
}

void AI::calc_explosions() {
    if (explosions.size()) {
        list<EXPLOSION> ::iterator p = explosions.begin();
        //p++;
        //while(p != explosions.end()/*explosions.size()*/) {
        if (!p->get_status()) explosions.pop_back();
        //p++;
        //}
    }
}

void AI::draw_explosions(HDC hdc) {
    if (explosions.size()) {
        list<EXPLOSION> ::iterator p = explosions.begin();
        //p++;
        while (p != explosions.end()/*explosions.size()*/) {
            p->explosion_anim(hdc);
            // explosions.pop_front();
            // p = explosions.begin();
            p++;
        }
    }
}

void AI::draw_bullets(HDC hdc, list<BULLET> &g, unsigned smoke_lenght, unsigned speed = 25, unsigned bullet_direction = GUN_UP, unsigned smoke_color = BSMOKE_WHITE) {
    POINT pos_change;

    if (g.size()) {
        list<BULLET> ::iterator p = g.begin();

        while (p != g.end()) {
            pos_change = p->GetPos();

            if (bullet_direction == GUN_UP) pos_change.y -= speed;
            if (bullet_direction == GUN_DOWN) pos_change.y += speed;

            p->DrawInPoint(hdc, pos_change, smoke_lenght, smoke_color);

            p++;
        }
    }
}

void AI::hit(list<BULLET> &g) {
    bsize = g.size();
    POINT check_pos = { 0 };
    RECT enemy_check;
    RECT mainR = GetMainRect();
    EXPLOSION bullet_hit(check_pos, NULL);

    list<BULLET> ::iterator p = g.begin();

    while (p != g.end()) {
        check_pos = p->GetPos();


        if ((check_pos.y < mainRect[0].y || check_pos.y > mainRect[1].y) && g.size() > 1) { g.erase(p--); }
        if (!boss) {
            if ((check_pos.x < player_ship.right && check_pos.x > player_ship.left && check_pos.y < player_ship.bottom && check_pos.y > player_ship.top) && g.size()) { if (!player.immortal) { bullet_hit.set_explosion(check_pos, 5); explosions.push_front(bullet_hit); player.damage(5); g.erase(p--); } }
            else for (int i = 0; i < enemy_num; i++) {
                enemy_check = en_ship[i].get_enemy_rect();
                if ((check_pos.x < enemy_check.right && check_pos.x > enemy_check.left && check_pos.y < enemy_check.bottom && check_pos.y > enemy_check.top) && g.size()) { bullet_hit.set_explosion(check_pos, 5); explosions.push_front(bullet_hit); en_ship[i].damage(20); g.erase(p--); }
            }
        }
        p++;
    }
}

void AI::draw_player(HDC hdc) {
    player_ship = GetRectFromPoint(player_ship, player.get_pos(), player_ship_size);
    Rectangle(hdc, player_ship.left, player_ship.top, player_ship.right, player_ship.bottom);
    if (player.immortal) {
        Pen = (HPEN)SelectObject(hdc, GetStockObject(DC_PEN));
        for (int i = 0; i < 100; i += 2) {
            SetDCPenColor(hdc, RGB(0, 0, i));
            Arc(hdc, player_ship.left - 20 - i, player_ship.top - 20 - i, player_ship.right + 20 + i, player_ship.bottom + 20 + i, 500, 500, 500, 500);
        }
        DeleteObject(SelectObject(hdc, Pen));
    }
}

void AI::player_gun_shoot() {
    POINT pos = player.get_pos();

    pos.y = pos.y - player_ship_size / 2;
    gun[0].push_front(pos);
}

void AI::main_calc() {

    stat_control();
    enemy_control();

    for (int i = 0; i < sectors_num + 1; i++) {
        hit(gun[i]);
    }
    calc_explosions();
    //enemy_respawn_sync();
    enemy_respawn_in_sector();
    if (kills > max_kills) init_boss = 1;
    if (/*kills > max_kills && !bs.active*/init_boss) boss_setup();
}

void AI::main_draw(HDC hdc) {

    //draw_player_fire(hdc);

    Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 3, player_color));
    draw_player(hdc);
    DeleteObject(SelectObject(hdc, Pen));

    Pen = (HPEN)SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(255, 255, 255)));
    for (int i = 0; i < enemy_num; i++) { en_ship[i].draw_enemy(hdc); if (!en_ship[i].get_status()) en_ship[i].Draw_respawn_portal(hdc); }
    if (!boss)for (int i = 0; i < 2; i++) {
        MoveToEx(hdc, sectors[i].left, sectors[i].top, NULL);
        LineTo(hdc, sectors[i].left, sectors[i].bottom);
    }
    DeleteObject(SelectObject(hdc, Pen));

    draw_bullets(hdc, gun[0], 150, 25, GUN_UP);

    for (int i = 1; i < 4; i++) {
        draw_bullets(hdc, gun[i], 150, 25, GUN_DOWN, BSMOKE_RED);
    }

    if (explosions.size())draw_explosions(hdc);
    if (/*!bs.active*/init_boss)boss_portal_draw(hdc);
}

void AI::player_pos_set(unsigned posX) {
    if ((posX >(mainRect[0].x + player_ship_size / 2)) && (posX < (mainRect[1].x - player_ship_size / 2))) player.set_posX(posX);
}

POINT AI::set_size(HDC hdc, POINT pos) {
    int x;
    int y;

    x = GetDeviceCaps(hdc, HORZRES) - 100;
    y = (GetDeviceCaps(hdc, VERTRES) / 100) * 90;
    pos.x = x;
    pos.y = y;

    return pos;
}

void AI::enemy_attack() {
    int temp = 1;

    for (int i = 0; i < enemy_num; i += 3) {
        for (int k = i; k < 3 + i; k++) {
            if (en_ship[k].get_status() && !en_ship[k].unset) { gun[temp].push_front(en_ship[k].get_gun_pos()); temp++; break; }
        }
    }
    active_guns = temp - 1;
}

void AI::enemy_control() {

    if (!boss) {
        unsigned start = 0, end = 3;
        POINT posX = player.get_pos();
        RECT pos;
        bool direction;
        EXPLOSION expl;
        unsigned expl_num;

        for (int i = 0; i < sectors_num; i++) {
            for (int start = 0; start < end; start++) {
                en_ship[start].anim_test(posX.x, sectors[i].left, sectors[i].right);
                pos = en_ship[start].get_enemy_rect();
                direction = en_ship[start].get_direction();
                if (!en_ship[start].get_status() && en_ship[start].need_explosion) {
                    kills++;
                    for (int i = 0; i < 5; i++) {
                        get_expl(expl, en_ship[start].get_enemy_rect(), 40);
                        explosions.push_front(expl);
                    }
                    en_ship[start].need_explosion = 0;
                }
            }
            start = end;
            end += 3;
        }
    }
}

void AI::start_game(HDC hdc, unsigned sect, unsigned m) {
    enemy_num = sect * 3;

    player_color = RGB(0, 72, 145);
    player_ship_size = 50;
    boss = 0;
    srand(time(NULL));
    max_kills = 20 + rand() % 30;
    init_boss = 0;
    m_kill = max_kills;

    sectors_num = sect;
    gun = new list<BULLET>[sectors_num + 1];
    en_ship = new ENEMY[enemy_num];
    sectors = new RECT[sectors_num];
    respawn_point = new POINT[enemy_num];
    POINT player_start;

    boss_timer = 0;
    pl_start_health = player.get_health();

    POINT void_point = { 0 };
    for (int i = 0; i < sectors_num + 1; i++) {
        gun[i].push_front(void_point);
    }

    EXPLOSION expl(void_point, 5);
    //explosions.push_back(expl);

    mainRect[1] = set_size(hdc, mainRect[1]);

    switch (m) {
    case AI_BOTTOM:

        division(mainRect[1].x, mainRect[0].x, mainRect[1].y, (mainRect[1].y - mainRect[0].y) - ((mainRect[1].y - mainRect[0].y) / 3));//height = 450
        sector_middle = set_sector_middle(sectors[sectors_num - 1].left, sectors[sectors_num - 1].right);
        set_respawn_points();
        player_start.x = (mainRect[0].x + mainRect[1].x) / 2;
        player_start.y = mainRect[0].y + 40;
        player.set_pos(player_start);
        break;
    case AI_TOP:
        division(mainRect[1].x, mainRect[0].x, mainRect[0].y, (mainRect[1].y - mainRect[0].y) / 2); //height = 150
        sector_middle = set_sector_middle(sectors[0].left, sectors[0].right);
        set_respawn_points();
        player_start.x = (mainRect[0].x + mainRect[1].x) / 2;
        player_start.y = mainRect[1].y - 40;
        player.set_pos(player_start);
        break;
    case AI_LEFT:
        division(mainRect[1].y, mainRect[0].y, mainRect[0].x, (mainRect[1].x - mainRect[0].x) / 3); //height = 200
        sector_middle = set_sector_middle(sectors[sectors_num - 1].right, sectors[sectors_num - 1].left);
        set_respawn_points();
        break;
    case AI_RIGHT:
        division(mainRect[1].y, mainRect[0].y, mainRect[1].x, (mainRect[1].x - mainRect[0].x) - ((mainRect[1].x - mainRect[0].x) / 3)); //height = 600
        sector_middle = set_sector_middle(sectors[sectors_num - 1].right, sectors[sectors_num - 1].left);
        set_respawn_points();
        break;
    default:
        division(mainRect[1].x, mainRect[0].x, mainRect[0].y, (mainRect[1].y - mainRect[0].y) / 3); //height = 150
        sector_middle = set_sector_middle(sectors[sectors_num - 1].left, sectors[sectors_num - 1].right);
        set_respawn_points();
        break;
    }
}

void AI::division(unsigned max_coord, unsigned min_coord, unsigned top, unsigned bottom) {
    sector_lenght = (max_coord - min_coord) / sectors_num;
    unsigned temp_lenght = sector_lenght;
    unsigned temp_lenght2 = 0;
    for (int i = 0; i < sectors_num; i++) {
        sectors[i].left = max_coord - temp_lenght;
        sectors[i].top = top;
        sectors[i].right = max_coord - temp_lenght2;
        sectors[i].bottom = bottom;
        temp_lenght += sector_lenght;
        temp_lenght2 += sector_lenght;
    }
}

void AI::set_respawn_points() {
    unsigned start = 0, end = enemy_num / sectors_num;
    unsigned offsetY = (sectors[0].bottom - sectors[0].top) / 4;
    unsigned offsetX = sector_middle;
    for (int i = 0; i < sectors_num; i++, offsetX -= sector_lenght) {
        for (int j = start; j < end; j++, offsetY += (sectors[0].bottom - sectors[0].top) / 4) {
            respawn_point[j].x = offsetX;
            respawn_point[j].y = sectors[i].bottom - offsetY;
            en_ship[j].set_model_pos(respawn_point[j].x, respawn_point[j].y);
        }
        offsetY = (sectors[0].bottom - sectors[0].top) / 4;
        start = end;
        end += 3;
    }
}

unsigned AI::set_sector_middle(unsigned a, unsigned b) {
    return (a + b) / 2;
}



class STARS {
    int starsNum;
    int starSpeed;
    POINT *mas;
public:
    STARS(unsigned s, unsigned num);
    void starsSet();
    void starsDraw(HDC hdc) const;
};

STARS::STARS(unsigned s, unsigned num) {
    starSpeed = s;
    starsNum = num;
    mas = new POINT[starsNum];

    srand(time(NULL));
    for (int i = 0; i < starsNum; i++) {
        mas[i].x = (rand() % 800) + 100;
        mas[i].y = ((600 / starsNum)*i) + 50;
    }
}

void STARS::starsSet() {
    for (int i = 0; i < starsNum; i++) {
        mas[i].y += starSpeed;
        if (mas[i].y > mainRect[1].y) {
            mas[i].y = 50;
            mas[i].x = (rand() % 800) + 100;
        }
    }
}

void STARS::starsDraw(HDC hdc) const {
    for (int i = 0; i < starsNum; i++) SetPixel(hdc, mas[i].x, mas[i].y, RGB(255, 255, 255));
}

AI control;
STARS star(5, 15);
//STATS gameStat;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hWndMain;
    HWND hButton1;
    wchar_t szClassName[] = L"MainWndClass";
    MSG msg;
    WNDCLASSEX wc;

    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreatePatternBrush((HBITMAP)LoadImageA(NULL, "back.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);//NULL;
    wc.lpszClassName = szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_WARNING);

    if (!RegisterClassEx(&wc)) {
        MessageBoxA(NULL, "Cannot register class", "Error", MB_OK);
        return 0;
    }

    hWndMain = CreateWindowW(szClassName, L"Game. pre-alpha buil.v040.Last update : 18.12.08", WS_MAXIMIZE | WS_CAPTION | WS_VISIBLE | WS_TABSTOP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, (HWND)NULL, (HMENU)NULL, (HINSTANCE)hInstance, NULL);
    //hButton1 = CreateWindow("BUTTON", "exit", WS_CHILD, 0, 0, 100, 20, hWndMain, (HMENU)NULL, (HINSTANCE) hInstance, NULL);
    if (!hWndMain) {
        MessageBoxA(NULL, "Cannot create main window", "Error", MB_OK);
        return 0;
    }

    ShowWindow(hWndMain, SW_MAXIMIZE);
    //ShowWindow(hButton1, nCmdShow);
    //UpdateWindow(hWndMain);


    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    HDC hdc;// = GetDC(hwnd);
    static HINSTANCE hInst;

    PAINTSTRUCT ps;
    RECT rect;
    RECT MyButton;

    //
    MyButton.left = 5;
    MyButton.top = 50;
    MyButton.right = 90;
    MyButton.bottom = 70;
    //

    //rect update
    RECT updateRect;
    updateRect.left = mainRect[0].x;
    updateRect.top = mainRect[0].y;
    updateRect.right = mainRect[1].x;
    updateRect.bottom = mainRect[1].y;
    //rect update

    //rect color
    static HPEN mainRectLine, temp;
    static HBRUSH mainRectBack, old;
    LOGBRUSH logMRL;
    logMRL.lbStyle = BS_SOLID;
    logMRL.lbColor = RGB(0, 0, 100);
    //rect color

    //ship
    char coord[1024];
    //ship


    int hor;
    int vert;


    switch (uMsg) {
    case WM_CREATE:
        hInst = GetModuleHandle(NULL);
        srand(time(NULL));
        hdc = GetDC(hwnd);
        control.start_game(hdc, 3, AI_TOP);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetTimer(hwnd, 1, 40, NULL);
        SetTimer(hwnd, 2, 800, NULL);
        break;
    case WM_KEYDOWN:
        switch ((int)wParam) {
        case VK_SPACE:
            KillTimer(hwnd, 1);
            KillTimer(hwnd, 2);
            ReleaseDC(hwnd, hdc);
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            break;
        case 73: //I
            control.player_immortal();
            break;
        case 66: //B
            control.start_boss_demo();
            break;
        case 71:
            InvalidateRect(hwnd, &updateRect, 1);
            break;
        case 65: //A
            break;
        }
        break;
    case WM_TIMER:
        switch (wParam) {
        case 2:
            control.enemy_attack();
            break;
        case 1:
            mytimer++;
            star.starsSet();
            control.main_calc();
            InvalidateRect(hwnd, &updateRect, 0);
            gameStat.UpdateStat(hwnd);
            break;
        }


        break;//345
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_DDD_DDD:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, (DLGPROC)DlgProc);
            break;
        case ID_SHIPCOLOR_RED:
            control.set_player_color(RGB(142, 0, 0));
            break;
        case ID_SHIPCOLOR_GREEN:
            control.set_player_color(RGB(11, 142, 0));
            break;
        case ID_SHIPCOLOR_BLUE:
            control.set_player_color(RGB(0, 72, 145));
            break;
        }
    case WM_MOUSEMOVE:
        control.player_pos_set(LOWORD(lParam));
        break;
    case WM_LBUTTONDOWN:
        control.player_gun_shoot();
        if ((LOWORD(lParam) > MyButton.left) && (LOWORD(lParam) < MyButton.right) && (HIWORD(lParam) > MyButton.top) && (HIWORD(lParam) < MyButton.bottom)) DestroyWindow(hwnd);
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);

        //main RECT
        mainRectLine = (HPEN)SelectObject(hdc, ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, 3, &logMRL, NULL, NULL));
        mainRectBack = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 0, 0)));
        Rectangle(hdc, mainRect[0].x, mainRect[0].y, mainRect[1].x, mainRect[1].y);
        DeleteObject(SelectObject(hdc, mainRectLine));
        //main RECT

        star.starsDraw(hdc);

        //ship navigation
        control.main_draw(hdc);
        //ship navigation

        //bullet
        SelectObject(hdc, old);
        //bullet



        gameStat.DrawGraphStats(hdc);

        mainRectLine = (HPEN)SelectObject(hdc, ExtCreatePen(PS_GEOMETRIC | PS_SOLID | PS_ENDCAP_SQUARE, 3, &logMRL, NULL, NULL));
        mainRectBack = (HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(0, 0, 0)));
        RoundRect(hdc, MyButton.left, MyButton.top, MyButton.right, MyButton.bottom, 8, 8);
        TextOutA(hdc, 33, 50, "exit", 4);
        DeleteObject(SelectObject(hdc, mainRectLine));
        DeleteObject(SelectObject(hdc, mainRectBack));


        //sys inf
        hor = GetDeviceCaps(hdc, HORZRES);
        vert = GetDeviceCaps(hdc, VERTRES);
        sprintf(coord, "sys inf:                            \nsize = %d\nResolution: %d x %d\nexpl: %d\nkills: %d of %d\nhealth: %d", bsize, hor, vert, control.expl_size(), control.get_kills(), m_kill, pl_health);
        DrawTextA(hdc, coord, -1, &updateRect, DT_RIGHT);
        //sys inf
        EndPaint(hwnd, &ps);
        break;
    case WM_CLOSE:
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    switch (umsg) {
    case WM_INITDIALOG:
        control.player_immortal();
        return 1;
    case WM_COMMAND:
        switch (LOWORD(wparam)) {
        case IDOK:
            unsigned buffer_red;
            unsigned buffer_green;
            unsigned buffer_blue;
            buffer_red = GetDlgItemInt(hwnd, IDC_EDIT1, NULL, 0);
            buffer_green = GetDlgItemInt(hwnd, IDC_EDIT1, NULL, 0);
            buffer_blue = GetDlgItemInt(hwnd, IDC_EDIT1, NULL, 0);
            control.set_player_color(RGB(buffer_red, buffer_green, buffer_blue));
            control.player_immortal();
            EndDialog(hwnd, 0);
            return 1;
        case IDCANCEL:
            control.player_immortal();
            EndDialog(hwnd, 0);
            return 1;
            /*case IDM_CLOSE:
            control.player_immortal();
            EndDialog(hwnd, 0);
            return 1;*/
        }
        break;
    }
    return 0;
}

RECT GetMainRect() {
    RECT mainR;
    mainR.left = mainRect[0].x;
    mainR.top = mainRect[0].y;
    mainR.right = mainRect[1].x;
    mainR.bottom = mainRect[1].y;

    return mainR;
}

bool IsPointInRect(RECT rect, POINT pt) {
    if ((pt.x > rect.left) && (pt.x < rect.right) && (pt.y > rect.bottom) && (pt.y < rect.top)) return 1;
    else return 0;
}

RECT GetRectFromPoint(RECT r, POINT p, unsigned size) {
    r.left = p.x - size / 2;
    r.top = p.y - size / 2;
    r.right = p.x + size / 2;
    r.bottom = p.y + size / 2;

    return r;
}

int GetPercentFromValue(int value, int percent) {
    return (value / 100) * percent;
}




