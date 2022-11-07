#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "winlib_db.h"
#include "kbhit.h"
#include <time.h>
#include <math.h>
time_t time(time_t*);

struct enemy{
    int enemy_x;
    int enemy_y;
    int enemy_size;
    int enemy_ax; // enemyの加速度
    int enemy_ay;
    int enemy_vx; // enemyの速度
    int enemy_vy;
    int color;
};

struct enemy cpu[10];
int cpu_num;
int countc, countr, countb, countn, countm;

int xd, yd, btime, ex, level, player_v = 10, player_size = 10, player_HP;
int xflag, yflag, nflag, mflag, nattack, mattack, ncool, mcool, ntime, mtime;
int item_x, item_y, item_color;
int cursor1 = 0, cursor2 = 0, fini1, fini2 = 0, xback, yback, gtime, inia = 8, air = 1;


void *func();
void *rot();


void g_circle(int a, int b, int x, int y){ // 中心(x,y),横軸a,縦軸bの楕円を描写
    int i;

    for(i = 0; i < 300; i++){
        dot(a*cos(2*M_PI*i/300)+x, b*sin(2*M_PI*i/300)+y);
    }
}


void g_fillcircle(int r, int x, int y){ // 中心(x,y),半径rの円を描写し塗りつぶす
    int i;

    for(i = 0; i <= r; i++){
        g_circle(i, i, x, y);
    }
}


void g_word(int w, int x, int y){
    int i;

    if(w == 'B'){
        g_fill(x, y, 20, 200);
        g_fill(x, y, 50, 20);
        g_fill(x, y+90, 50, 20);
        g_fill(x, y+180, 50, 20);
        
        for(i = 0; i < 40; i++){
            g_fillcircle(10, 45*sin(M_PI*i/40)+x+50, 45*cos(M_PI*i/40)+y+55);
            g_fillcircle(10, 45*sin(M_PI*i/40)+x+50, 45*cos(M_PI*i/40)+y+145);
        }
    }
    else if(w == 'a'){
        g_fill(x, y, 20, 100);
        for(i = 0; i < 60; i++){
            g_fillcircle(10, 40*cos(2*M_PI*i/60)+x-30, 40*sin(2*M_PI*i/60)+y+50);
        }
    }
    else if(w == 'l'){
        g_fill(x, y, 20, 200);
    }
}


void reset(int c){
    if(c == 0){
        cpu_num = 2;
        countc = 0, countr = 0, countb = 0, countn = 0, countm = 0;

        btime = 0, ex = 0, level = 1, player_HP = 500;
        xflag = 0, yflag = 0, nflag = 0, mflag = 0, nattack = 0, mattack = 0, ncool = 0, mcool = 0, ntime = 0, mtime = 0;
        fini1 = 0, fini2 = 0, xback = 0, yback = 0, gtime = -1;
    }
    else if(c == 1){
        cpu_num = 4;
        countc = 0, countr = 0, countb = 0, countn = 0, countm = 0;

        btime = 0, ex = 0, level = 1, player_HP = 250;
        xflag = 0, yflag = 0, nflag = 0, mflag = 0, nattack = 0, mattack = 0, ncool = 0, mcool = 0, ntime = 0, mtime = 0;
        fini1 = 0, fini2 = 0, xback = 0, yback = 0, gtime = -1;
    }
}


void *rot(){
    int ball_x = 345, ball_y = 90, ball_v = 0, ball_a = 6;
    int i = 100;

    // タイトルの描写
    g_rgb(0, 0, 65535);
    g_word('B', 105, 80);
    g_word('a', 295, 180);
    g_word('l', 335, 80);
    g_word('l', 375, 80);
    g_rgb(0, 0, 0);

    // スタートコマンド
    g_rgb(0, 0, 65535);
    g_box(105, 300, 210, 40);
    g_line(202, 308, 202, 332);
    g_line(202, 308, 223, 320);
    g_line(223,320, 202, 332);
    g_rgb(0, 0, 0);

    // 終了コマンド
    g_box(330, 300, 65, 40);
    g_box(356, 310, 4, 20);
    g_box(364, 310, 4, 20);

    // Easyコマンド
    g_rgb(0, 0, 65535);
    g_box(105, 350, 137, 40);
    text(165, 374, "Easy");
    g_rgb(0, 0, 0);

    // Hardコマンド
    g_box(257, 350, 138, 40);
    text(316, 374, "Hard");

    while(1){
        // 動くballの描写
        if(ball_y < 180){
            ball_v += ball_a;
        }
        else if(ball_y > 180){
            ball_v -= ball_a;
        }
        ball_y += ball_v;
        g_rgb(65535, 65535, 65535);
        g_circle(9, 9, ball_x, ball_y);
        g_circle(9, 9, ball_x+40, 360-ball_y);
        g_rgb(0, 0, 0);
    
        display_update();

        // ballの消去
        g_rgb(0, 0, 65535);
        g_circle(9, 9, ball_x, ball_y);
        g_circle(9, 9, ball_x+40, 360-ball_y);
        g_rgb(0, 0, 0);

        usleep(100000);

        if(fini2 == 1){
            fini2 = 0;
            return 0;
        }
    }
}


int menu(){
    int c;
    cursor2 = 0;

    pthread_t pthread2;
    pthread_create(&pthread2, NULL, &rot, NULL);
    
    while(1){
        if(kbhit()){
            c = getchar();
            if((c == 'd') && (cursor1 == 0)){ // cursor1を右にずらす
                cursor1 = 1;
                g_rgb(0, 0, 65535);
                g_box(330, 300, 65, 40);
                g_box(356, 310, 4, 20);
                g_box(364, 310, 4, 20);
                g_rgb(0, 0, 0);
                g_box(105, 300, 210, 40);
                g_line(202, 308, 202, 332);
                g_line(202, 308, 223, 320);
                g_line(223,320, 202, 332);
            }
            else if((c == 'a') && (cursor1 == 1)){ // cursor1を左にずらす
                cursor1 = 0;
                g_rgb(0, 0, 65535);
                g_box(105, 300, 210, 40);
                g_line(202, 308, 202, 332);
                g_line(202, 308, 223, 320);
                g_line(223,320, 202, 332);
                g_rgb(0, 0, 0);
                g_box(330, 300, 65, 40);
                g_box(356, 310, 4, 20);
                g_box(364, 310, 4, 20);
            }
            else if((c == 'c') && (cursor2 == 0)){ // cursor2を右にずらす
                cursor2 = 1;
                g_rgb(0, 0, 65535);
                g_box(257, 350, 138, 40);
                text(316, 374, "Hard");
                g_rgb(0, 0, 0);
                g_box(105, 350, 137, 40);
                text(165, 374, "Easy");
            }
            else if((c == 'z') && (cursor2 == 1)){  // cursor2を左にずらす
                cursor2 = 0;
                g_rgb(0, 0, 65535);
                g_box(105, 350, 137, 40);
                text(165, 374, "Easy");
                g_rgb(0, 0, 0);
                g_box(257, 350, 138, 40);
                text(316, 374, "Hard");
            }
            else if(c == '\n'){
                if(cursor1 == 1){
                    return 1;
                }
                else if(cursor1 == 0){
                    if(cursor2 == 0){
                        reset(cursor2);
                    }
                    else if(cursor2 == 1){
                        reset(cursor2);
                    }
                    fini2 = 1;

                    return 0;
                }
            }
        }
    }
}


int controlPlayer(){
    int i, c;

    while (1){
        // playerの進行方向をリセット
        xd = 0;
        yd = 0;

        if(kbhit()){
            c = getchar();
            if(c == 'w'){ // 上移動
                yd = 1;
                xflag = 0;
                yflag = 1;
            }
            if(c == 'a'){ // 左移動
                xd = 1;
                xflag = 1;
                yflag = 0;
            }
            if(c == 's'){ // 下移動
                yd = -1;
                xflag = 0;
                yflag = -1;
            }
            if(c == 'd'){ // 右移動
                xd = -1;
                xflag = -1;
                yflag = 0;
            }
            if((c == 'n') && (ncool == 0)){ // nで攻撃｢ビーム｣
                if(nflag == 1){
                    nattack = 5;
                }
                else{
                    nattack = 3;
                }

                ncool = 55 - level; // クールタイム
            }
            if((c == 'm') && (mcool == 0)){ // mで攻撃｢ボム｣
                if(mflag == 1){
                    mattack = 6;
                }
                else{
                    mattack = 4;
                }

                mcool = 56 - level; // クールタイム
            }
        }

        if(fini1 == 1){
            return 0;
        }

        // 背景をずらす
        xback += xd*player_v;
        yback += yd*player_v;

        // enemyをずらす
        for(i = 0; i < cpu_num; i++){
            cpu[i].enemy_x += xd*player_v;
            cpu[i].enemy_y += yd*player_v;
        }

        // itemをずらす
        item_x += xd*player_v;
        item_y += yd*player_v;
    }
}


int result(){
    int c;
    char t_str[12], e_str[12], l_str[12], c_str[6], r_str[6], b_str[6], n_str[6], m_str[6];

    sprintf(t_str, "Time: %.1f", (float)gtime/10);
    sprintf(e_str, "Exp: %d", ex);
    sprintf(l_str, "Level: %d", level);
    sprintf(c_str, "x %d", countc);
    sprintf(r_str, "x %d", countr);
    sprintf(b_str, "x %d", countb);
    sprintf(n_str, "x %d", countn);
    sprintf(m_str, "x %d", countm);

    g_box(100, 50, 300, 400);
    text(234, 78, "Result");
    g_line(110, 74, 225, 74);
    g_line(275, 74, 390, 74);

    if(cursor2 == 0){
        text(240, 125, "Easy");
    }
    else if(cursor2 == 1){
        text(240, 125, "Hard");
    }

    text(142, 180, t_str);
    text(232, 180, e_str);
    text(306, 180, l_str);

    g_box(155, 235, 10, 10);
    text(173, 245, c_str);
    g_rgb(65535, 0, 0);
    g_fill(233, 235, 10, 10);
    g_rgb(0, 0, 0);
    text(249, 245, r_str);
    g_rgb(0, 0, 65535);
    g_box(309, 235, 10, 10);
    g_rgb(0, 0, 0);
    text(325, 245, b_str);

    g_rgb(65535, 65535, 0);
    g_fillcircle(5, 197, 300);
    g_fillcircle(5, 281, 300);
    g_rgb(0, 0, 0);
    g_circle(5, 5, 197, 300);
    g_circle(5, 5, 281, 300);
    text(195, 303, "n");
    text(279, 303, "m");
    text(208, 304, n_str);
    text(292, 304, m_str);

    text(218, 375, "Press Enter");

    display_update();

    while(1){
        if(kbhit()){
            c = getchar();
            if(c == '\n'){
                g_clear();
                return 0;
            }
        }
    }
}


void manageTime(){
    gtime++;

    if(btime > 0){
        btime--;
    }
    if(ncool > 0){
        ncool--;
    }
    if(mcool > 0){
        mcool--;
    }
    if(ntime > 0){
        ntime--;
        if(ntime == 0){
            nflag = 0;
        }
    }
    if(mtime > 0){
        mtime--;
        if(mtime == 0){
            mflag = 0;
        }
    }

    // 0.1秒ごとにHPを1回復
    if(cursor2 == 0){
        if(player_HP < 490+10*level){
            player_HP++;
        }
    }
    else if(cursor2 == 1){
        if(player_HP < 240+10*level){
            player_HP++;
        }
    }
}


void addEnemy(){
    if(cpu_num < 10){
        if(gtime%200 == 0){
            cpu_num++;
            if(rand()%2){
                cpu[cpu_num-1].enemy_x = rand()%490;
                cpu[cpu_num-1].enemy_y = -20+rand()%2*560;
            }
            else{
                cpu[cpu_num-1].enemy_x = -20+rand()%2*560;
                cpu[cpu_num-1].enemy_y = rand()%490;
            }
            cpu[cpu_num-1].enemy_ax = 0;
            cpu[cpu_num-1].enemy_ay = 0;
            cpu[cpu_num-1].enemy_vx = 0;
            cpu[cpu_num-1].enemy_vy = 0;
            cpu[cpu_num-1].color = rand()%10;
            if(cpu[cpu_num-1].color == 1){
                cpu[cpu_num-1].enemy_size = 20;
            }
            else if(cpu[cpu_num-1].color == 2){
                cpu[cpu_num-1].enemy_size = 10;
            }
            else{
                cpu[cpu_num-1].enemy_size = 15;
            }
        }
    }
}


void getItem(){
    if((item_x>240 && item_x<260) && (item_y>240 && item_y<260)){
        if(item_color == 0){ // ビームを+5秒強化
            nflag = 1;
            ntime += 50;
            countn++;
            if(ntime > 200){
                ntime = 200; // ntimeの上限は20秒
            }
        }
        else{ // ボムを+5秒強化
            mflag = 1;
            mtime += 50;
            countm++;
            if(mtime > 200){
                mtime = 200; // mtimeの上限は20秒
            }
        }

        item_x = rand()%1000-500;
        item_y = rand()%1000-500;
        item_color = rand()%2;
    }
}


void manageEx(){
    if((ex%10 == 0) && (level < 50)){ // exを管理
        level++;
    }
}


void addEx(int i){
    // enemyを攻撃するとexを取得
    if(cpu[i].color == 1){
        ex++; // 赤のenemyはexが2倍
        manageEx();
        ex++;
        manageEx();

        countr++;
    }
    else if(cpu[i].color == 2){
        level++; // 青のenemyはlevelがあがる

        countb++;
    }
    else{
        ex++;
        manageEx();

        countc++;
    }
}


int judgeDamage(int i){
    // enemyがplayerに触れたら1を返す
    if((cpu[i].enemy_x>240-cpu[i].enemy_size && cpu[i].enemy_x<260) 
            && (cpu[i].enemy_y>240-cpu[i].enemy_size && cpu[i].enemy_y<260)){
        if(btime == 0){
            player_HP -= 100; // enemyにふれたらHPを100減らす
            if(cpu[i].color == 1){
                player_HP -= 100; // 赤のenemyは攻撃力が2倍
            }
            btime = 5; // 連続ヒットを回避するために、数秒間バリアを展開
        }

        return 1;
    }

    // 青のenemyは逃げることがある
    if(cpu[i].color == 2){
        if(rand()%10 == 1){
            return 1;
        } 
    }

    // enemyがplayerの攻撃にヒットしたら1を返す
    // ビームの当たり判定
    if(nattack > 0){
        if(nflag == 1){
            if((cpu[i].enemy_x>240-cpu[i].enemy_size && cpu[i].enemy_x<260)
                    || (cpu[i].enemy_y>240-cpu[i].enemy_size && cpu[i].enemy_y<260)){
                addEx(i);

                return 1;
            }
        }
        else{
            if(yflag == 1){
                if((cpu[i].enemy_y<240-cpu[i].enemy_size) && (cpu[i].enemy_x>245-cpu[i].enemy_size && cpu[i].enemy_x<255)){
                    addEx(i);

                    return 1;
                }
            }    
            else if(xflag == 1){
                if((cpu[i].enemy_x<240-cpu[i].enemy_size) && (cpu[i].enemy_y>245-cpu[i].enemy_size && cpu[i].enemy_y<255)){
                    addEx(i);

                    return 1;
                }
            }
            else if(yflag == -1){
                if((cpu[i].enemy_y>260) && (cpu[i].enemy_x>245-cpu[i].enemy_size && cpu[i].enemy_x<255)){
                    addEx(i);

                    return 1;
                }
            }
            else{
                if((cpu[i].enemy_x>260) && (cpu[i].enemy_y>245-cpu[i].enemy_size && cpu[i].enemy_y<255)){
                    addEx(i);

                    return 1;
                }
            }
        }
    }
    // ボムのあたり判定
    if(mattack > 0){
        if((mflag == 1) && ((mattack == 4) || (mattack == 3) || (mattack == 2) || (mattack == 1))){
            if((cpu[i].enemy_x>180-cpu[i].enemy_size && cpu[i].enemy_x<320) 
                    && (cpu[i].enemy_y>180-cpu[i].enemy_size && cpu[i].enemy_y<320)){
                addEx(i);

                return 1;
            }
        }
        else if((mflag == 0) && ((mattack == 1) || (mattack == 2) || (mattack == 3))){
            if((cpu[i].enemy_x>210-cpu[i].enemy_size && cpu[i].enemy_x<290) 
                    && (cpu[i].enemy_y>210-cpu[i].enemy_size && cpu[i].enemy_y<290)){
                addEx(i);

                return 1;
            }
        }
    }

    // 上記以外なら0を返す
    return 0;
}


void chasePlayer(){
    int i;

    for(i = 0; i < cpu_num; i++){
        // (250,250)にいるplayerを追尾
        if(cpu[i].enemy_x+cpu[i].enemy_size/2 < 250){
            cpu[i].enemy_vx += cpu[i].enemy_ax;
            cpu[i].enemy_ax = inia - air*cpu[i].enemy_vx;
        }
        else if(cpu[i].enemy_x+cpu[i].enemy_size/2 > 250){
            cpu[i].enemy_vx += cpu[i].enemy_ax;
            cpu[i].enemy_ax = -inia - air*cpu[i].enemy_vx;
        }

        if(cpu[i].enemy_y+cpu[i].enemy_size/2 < 250){
            cpu[i].enemy_vy += cpu[i].enemy_ay;
            cpu[i].enemy_ay = inia - air*cpu[i].enemy_vy;
        }
        else if(cpu[i].enemy_y+cpu[i].enemy_size/2 > 250){
            cpu[i].enemy_vy += cpu[i].enemy_ay;
            cpu[i].enemy_ay = -inia - air*cpu[i].enemy_vy;
        }

        cpu[i].enemy_x += cpu[i].enemy_vx;
        cpu[i].enemy_y += cpu[i].enemy_vy;
    }
}


void drawBack(){
    int i;

    g_rgb(0, 65535, 0);
    for(i = 0; i < 11; i++){
        g_line(xback%50 + i*50, 0, xback%50 + i*50, 500);
        g_line(0, yback%50 + i*50, 500, yback%50 + i*50);
    }
    g_rgb(0, 0, 0);
}


void drawAttack(){
    int i;

    if(nattack > 0){
        if(nflag == 1){ // 強化ビーム
            g_rgb(65535, 0, 65535);
            g_fill(249, 0, 2, 236);
            g_fill(0, 249, 236, 2);
            g_fill(249, 264, 2, 236);
            g_fill(264, 249, 236, 2);
            if((nattack == 5) || (nattack == 2)){
                g_fill(245, 0, 10, 236);
                g_fill(0, 245, 236, 10);
                g_fill(245, 264, 10, 236);
                g_fill(264, 245, 236, 10);
            }
            if((nattack == 4) || (nattack == 3)){
                g_fill(240, 0, 20, 236);
                g_fill(0, 240, 236, 20);
                g_fill(240, 264, 20, 236);
                g_fill(264, 240, 236, 20);
            }
        }
        else{ // 通常ビーム
            g_rgb(0, 65535, 65535);
            if(yflag == 1){
                g_fill(249, 0, 2, 236);
            }    
            else if(xflag == 1){
                g_fill(0, 249, 236, 2);
            }
            else if(yflag == -1){
                g_fill(249, 264, 2, 236);
            }
            else{
                g_fill(264, 249, 236, 2);
            }
            if((nattack == 3) || (nattack == 2)){
                if(yflag == 1){
                    g_fill(245, 0, 10, 236);
                }
                else if(xflag == 1){
                    g_fill(0, 245, 236, 10);
                }
                else if(yflag == -1){
                    g_fill(245, 264, 10, 236);
                }
                else{
                    g_fill(264, 245, 236, 10);
                }
            }
        }

        nattack--;
    }

    if(mattack > 0){
        if(mflag == 1){ // 強化ボム
            g_rgb(65535, 65535, 0);
            g_fillcircle(15, 250, 250);
            if((mattack == 4) || (mattack == 3) || (mattack == 2) || (mattack == 1)){
                g_rgb(65535, 0, 65535);
                g_fillcircle(70, 250, 250);
            }
        }
        else{ // 通常ボム
            g_rgb(65535, 65535, 0);
            g_fillcircle(15, 250, 250);
            if((mattack == 1) || (mattack == 2) || (mattack == 3)){
                g_rgb(0, 65535, 65535);
                g_fillcircle(40, 250, 250);
            }
        }

        mattack--;
    }

    g_rgb(0, 0, 0);
}


void drawPlayer(){
    g_rgb(65535, 0, 0);
    if(btime > 0){
        g_rgb(0, 0, 65535); // バリア展開中は青に染める
    }
    g_circle(player_size, player_size, 250, 250);
    g_rgb(0, 0, 0);
}


void drawEnemy(){
    int i;

    for(i = 0; i < cpu_num; i++){
        if(cpu[i].color == 1){
            g_rgb(65535, 0, 0);
            g_fill(cpu[i].enemy_x, cpu[i].enemy_y, cpu[i].enemy_size, cpu[i].enemy_size);
            g_rgb(0, 0, 0);
        }
        else if(cpu[i].color == 2){
            g_rgb(0, 0, 65535);
            g_box(cpu[i].enemy_x, cpu[i].enemy_y, cpu[i].enemy_size, cpu[i].enemy_size);
            g_rgb(0, 0, 0);
        }
        else{
            g_box(cpu[i].enemy_x, cpu[i].enemy_y, cpu[i].enemy_size, cpu[i].enemy_size);
        }
    }
}


void drawItem(){
    // 3秒ことにitemの位置をリセット
    if(gtime%30 == 0){
        item_x = rand()%1000-500;
        item_y = rand()%1000-500;
        item_color = rand()%2;
    }

    g_rgb(65535, 65535, 0);
    g_fillcircle(5, item_x, item_y);
    g_rgb(0, 0, 0);
    g_circle(5, 5, item_x, item_y);
    if(item_color == 0){
        text(item_x-2, item_y+3, "n");
    }
    else{
        text(item_x-2, item_y+3, "m");
    }
}


void drawUI(){
    char gtime_str[12], HP_str[15], level_str[12];

    // 経過時間の描写
    sprintf(gtime_str, " Time: %.1f", (float)gtime/10);
    text(10, 20, gtime_str);

    // 残りHPの描写
    g_rgb(65535, 0, 0);
    if(cursor2 == 0){
        g_fill(150, 7, 340*player_HP/(490+10*level), 13);
    }
    else if(cursor2 == 1){
        g_fill(150, 7, 340*player_HP/(240+10*level), 13);
    }
    g_rgb(0, 0, 0);
    g_box(150, 7, 340, 13);
    if(cursor2 == 0){
        sprintf(HP_str, "HP: %d / %d", player_HP, 490+10*level);
    }
    else if(cursor2 == 1){
        sprintf(HP_str, "HP: %d / %d", player_HP, 240+10*level);
    }
    text(400, 19, HP_str);

    // ex, levelの描写
    g_rgb(0, 65535, 65535);
    g_fill(150, 25, 34*(ex%10), 13);
    g_rgb(0, 0, 0);
    g_box(150, 25, 340, 13);
    sprintf(level_str, "level: %d", level);
    text(425, 37, level_str);

    // ビームの発射方向の描写
    if(nflag == 1){
        g_line(250, 230, 245, 235);
        g_line(250, 230, 255, 235);
        g_line(230, 250, 235, 255);
        g_line(230, 250, 235, 245);
        g_line(250, 270, 255, 265);
        g_line(250, 270, 245, 265);
        g_line(270, 250, 265, 245);
        g_line(270, 250, 265, 255);
    }
    else{
        if(yflag == 1){
            g_line(250, 230, 245, 235);
            g_line(250, 230, 255, 235);
        }    
        else if(xflag == 1){
            g_line(230, 250, 235, 255);
            g_line(230, 250, 235, 245);
        }
        else if(yflag == -1){
            g_line(250, 270, 255, 265);
            g_line(250, 270, 245, 265);
        }
        else{
            g_line(270, 250, 265, 245);
            g_line(270, 250, 265, 255);
        }
    }

    // 武器の情報の描写
    g_rgb(0, 0, 65535);
    g_fill(300, 477, 80*ncool/(55-level), 8);
    g_fill(400, 477, 80*mcool/(56-level), 8);
    if(nflag == 1){
        g_rgb(0, 65535, 65535);
        g_fill(300, 470-2*ntime/5, 80, 2*ntime/5);
        g_rgb(65535, 0, 65535);
        g_fill(300, 420, 80, 20);
        g_fill(330, 390, 20, 80);
    }
    else{
        g_rgb(0, 65535, 65535);
        g_fill(300, 425, 80, 10);
    }
    if(mflag == 1){
        g_rgb(0, 65535, 65535);
        g_fill(400, 470-2*mtime/5, 80, 2*mtime/5);
        g_rgb(65535, 0, 65535);
        g_fillcircle(35, 440, 430);
    }
    else{
        g_rgb(0, 65535, 65535);
        g_fillcircle(15, 440, 430);
    }
    g_rgb(0, 0, 0);
    g_box(300, 390, 80, 80);
    g_box(400, 390, 80, 80);
    g_box(300, 477, 80, 8);
    g_box(400, 477, 80, 8);
    text(304, 400, "n");
    text(404, 400, "m");
}



void *func(){
    srand((unsigned int)time(NULL));
    int i;

    for(i = 0; i < cpu_num; i++){
        if(rand()%2){
            cpu[i].enemy_x = rand()%490;
            cpu[i].enemy_y = -20*(i+1)+rand()%2*560;
        }
        else{
            cpu[i].enemy_x = -20*(i+1)+rand()%2*560;
            cpu[i].enemy_y = rand()%490;
        }
        cpu[i].enemy_ax = 0;
        cpu[i].enemy_ay = 0;
        cpu[i].enemy_vx = 0;
        cpu[i].enemy_vy = 0;
        cpu[i].color = rand()%10;
        if(cpu[i].color == 1){
            cpu[i].enemy_size = 20;
        }
        else if(cpu[i].color == 2){
            cpu[i].enemy_size = 10;
        }
        else{
            cpu[i].enemy_size = 15;
        }
    }

    while(1){
        manageTime(); // ゲーム経過時間、クールタイムなどを計測
        addEnemy(); // cpu_numが10未満なら、20秒ごとにenemyの数を1つ増やす
        getItem(); // itemを取得した時の処理

        // あたり判定
        for(i = 0; i < cpu_num; i++){
            if(judgeDamage(i)){
                // enemyを新しく生成する
                if(rand()%2){
                    cpu[i].enemy_x = rand()%490;
                    cpu[i].enemy_y = -20*(i+1)+rand()%2*560;
                }
                else{
                    cpu[i].enemy_x = -20*(i+1)+rand()%2*560;
                    cpu[i].enemy_y = rand()%490;
                }
                cpu[i].enemy_ax = 0;
                cpu[i].enemy_ay = 0;
                cpu[i].enemy_vx = 0;
                cpu[i].enemy_vy = 0;
                cpu[i].color = rand()%10;
                if(cpu[i].color == 1){
                    cpu[i].enemy_size = 20;
                }
                 else if(cpu[i].color == 2){
                    cpu[i].enemy_size = 10;
                }
                else{
                    cpu[i].enemy_size = 15;
                }
            }

            // HPが0になったらゲームオーバー
            if(player_HP <= 0){
                fini1 = 1;
            }
        }
        if(fini1 == 1){
            return 0;
        }

        chasePlayer(); // enemyがplayerを追いかける

        g_clear();
        drawBack(); // 背景を描写
        drawAttack(); // playerの攻撃モーションを描写
        drawPlayer(); // playerを描写
        drawEnemy(); // enemyを描写
        drawItem(); // itemを描写
        drawUI(); // UIを描写
        display_update();
        usleep(100000);
    }
}



int main(void){
    int f = 0;
    XInitThreads();

    initwin();

    pthread_t pthread1;

    while(1){
        f = menu(); //menuを描写
        if(f){
            return 0;
        }

        pthread_create(&pthread1, NULL, &func, NULL);

        controlPlayer(); // playerの操作
        g_clear();

        result(); // 結果発表
    }

    return 0;
}
