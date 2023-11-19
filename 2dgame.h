#include <SDL2/SDL.h>
#include <math.h>
#include <vector>
#include <algorithm>

const int TILE_SIZE = 20;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int STEPS = 10;

const int PLAYER_HITBOX_MARGIN = 5;

using namespace std;

enum collisionTypes {
    TOP,
    SIDE,
    BOTTOM,
    TOTAL
};


enum PlayerFacing {
    DOWN, LEFT, UP, RIGHT, TOTAL_FACING
};

SDL_Texture* IMG_toTexture(SDL_Renderer* renderer, char* name);

class SpriteSheet{
    public:
    char* name = "sprites.bmp";
    SDL_Texture* texture = NULL;
    int clipsize_x, clipsize_y;
    SDL_Rect Clips [TOTAL_FACING][STEPS];
    SDL_Rect idleClips[3];
    SpriteSheet(SDL_Renderer* renderer);
};

SpriteSheet::SpriteSheet(SDL_Renderer* renderer){
    texture = IMG_toTexture(renderer, name);
    int x = 0;
    int y = 417;
    clipsize_x = 95;
    clipsize_y = 106;

    

    for (int i = 0; i < TOTAL_FACING; i++){
        for(int j = 0; j < STEPS; j++){
            Clips[i][j] = {x, y, clipsize_x, clipsize_y};
            x += clipsize_x;
        }
        y += clipsize_y;
        x = 0;
    }

    x = 0; y = 0;

    for (int i = 0; i < 3; i++){
        idleClips[i] = {x, y, clipsize_x, clipsize_y};
        x += clipsize_x;
    }

}

int sign(float x){
    return (0<x) - (0>x);
}


const float G = 0.2;

SDL_Texture* IMG_toTexture(SDL_Renderer* renderer, char* name){
    SDL_Surface* imgsurface = NULL;
    imgsurface = SDL_LoadBMP(name);

    if (imgsurface == NULL){
        cout << "SHIT BRIXXXX" << endl;
        char* tmp;
        cin >> tmp;
        return NULL;
    }

    SDL_Texture* imgtexture = NULL;
    imgtexture = SDL_CreateTextureFromSurface(renderer, imgsurface);

    if (imgtexture == NULL){
        cout << "SHIT BRIXXXX" << endl;
        char* tmp;
        cin >> tmp;
        return NULL;
    }

    SDL_FreeSurface(imgsurface);
    return imgtexture;
}


class Camera{
    public:
    int x = 80;
    int y = 0;
    int w = SCREEN_WIDTH;
    int h = SCREEN_HEIGHT;

    void handleEvent(SDL_Event& e);
    
};

void Camera::handleEvent(SDL_Event& e){
    
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_RIGHT){
            //if(x+3 <= SCREEN_WIDTH) x+=3;
        }
        

        if (e.key.keysym.sym == SDLK_LEFT){
            //``if(x-3 >= 0) x-=3;
        }
        

        if (e.key.keysym.sym == SDLK_UP){
            
        }

        
    }

    /*
    if(e.type == SDL_KEYUP){
        if(e.key.keysym.sym == SDLK_RIGHT){
            
        }
        
        if (e.key.keysym.sym == SDLK_LEFT){
            
        }

        if (e.key.keysym.sym == SDLK_UP){
            
        }
        
        
    }
    */
    

    
    
}


class Bar{
    public:
    int x_base, y_base, Width, Height;
    int color; 
    Bar(int xpos, int ypos, int setcolor);
    void render(SDL_Renderer* renderer, float barModifier);
};

Bar::Bar(int xpos, int ypos, int setcolor){
    x_base = xpos;
    y_base = ypos;
    color = setcolor;
    Width = 10;
    Height = 10;
}

void Bar::render(SDL_Renderer* renderer, float barModifier){
    int x = x_base;
    int y = y_base;
    int iter_max = barModifier*10; 
    for (int i = 0; i < iter_max; i++) {
        SDL_Rect fillRect{(int)x, (int)y, Width, Height};
        if (color==0){
            SDL_SetRenderDrawColor( renderer, 0xFF, 0x00, 0x00, 0xFF);
        }
        if (color==1){
            SDL_SetRenderDrawColor( renderer, 0xEE, 0xEE, 0x00, 0xFF);
        }

        SDL_RenderFillRect( renderer, &fillRect );
        x+=Width;
    }

}



class Player{
    public:
    int currentFacing = RIGHT;
    int isMoving = 0;
    float xpos, xaccel, ypos, xvel, yvel, xvel_MAX, yvel_MAX,
        x_vel_difference;
    int playerWidth, playerHeight;

    int key_state, kright, kleft, key_up;

    float health, score;
    
    
    Player();
    void handleEvent(SDL_Event& e);
    void move(int* collisionList);
    void render(SDL_Renderer* renderer, Camera &camera, SpriteSheet spritesheet);
};

Player::Player(){
    xpos = 10;
    ypos = 0;
    xvel = 0;
    yvel = 0;
    score = 0.0;
    xaccel = 0.07;
    xvel_MAX = 3;
    yvel_MAX = 3;
    kright = 0;
    kleft = 0;
    key_state = 0;
    key_up = 0;    
    playerHeight=TILE_SIZE*4;
    playerWidth=TILE_SIZE*2.5;
    health = 2.0;
}

class Walls{
    public:
    SDL_Texture* texture = NULL;
    vector <SDL_Rect> wallMap;
    Walls(int** mapa, int numberOfTiles, SDL_Texture* brickTexture);
    void render(SDL_Renderer* renderer, Camera &camera);
};

bool compareByX(const SDL_Rect &a, const SDL_Rect &b){
    return a.x < b.x;
}

Walls::Walls(int** mapa, int numberOfTiles, SDL_Texture* brickTexture){
    texture = brickTexture;
    int iMax = SCREEN_HEIGHT/TILE_SIZE;
    int jMax = SCREEN_WIDTH/TILE_SIZE*numberOfTiles;   
    for (int i = 0; i < iMax; i++){
        for (int j = 0; j < jMax; j++){
            if (mapa[i][j] == 1){
                SDL_Rect newWallTile { j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE, TILE_SIZE };
                wallMap.push_back(newWallTile);
            }
        }
    }

    sort(wallMap.begin(), wallMap.end(), compareByX);
}

void Walls::render(SDL_Renderer* renderer, Camera &camera){
    cout << "---------------------------" << endl;
    cout << "seeking walls between " << camera.x-TILE_SIZE << "and " << camera.w+camera.x << endl;
    cout << "walls at: " << endl;
    for (int i = 0; i < wallMap.size(); i++){
        SDL_SetRenderDrawColor( renderer, 0x11, 0xaa, 0x00, 0xFF );
        if(wallMap[i].x >= camera.x-TILE_SIZE && wallMap[i].x < (camera.w+camera.x)){
            SDL_Rect fillRect = wallMap[i];
            fillRect.x -= camera.x;
            cout << "x: " << fillRect.x << ", y: " << fillRect.y << endl;
            SDL_RenderCopy(renderer, texture, NULL, &fillRect);
        }
    }
}



void Player::handleEvent(SDL_Event& e){
    
    if(e.type == SDL_KEYDOWN){
        if(e.key.keysym.sym == SDLK_RIGHT){
            kright = 1;
        }
        //else kright = 0;

        if (e.key.keysym.sym == SDLK_LEFT){
            kleft = 1;
        }
        //else kleft = 0;

        if (e.key.keysym.sym == SDLK_UP){
            key_up = 1;
        }

        key_state = kright - kleft;
    }

    
    if(e.type == SDL_KEYUP){
        if(e.key.keysym.sym == SDLK_RIGHT){
            kright = 0;
        }
        
        if (e.key.keysym.sym == SDLK_LEFT){
            kleft = 0;
        }

        if (e.key.keysym.sym == SDLK_UP){
            key_up = 0;
        }
        
        key_state = kright - kleft;
    }
    

    
    
}

void Player::move(int* collisionList){
    x_vel_difference = xvel_MAX * key_state - xvel;
    xvel += sign(x_vel_difference) * min(xaccel, abs(x_vel_difference));
    xpos += xvel;

    if (key_up){

        if(yvel == 0 && collisionList[BOTTOM]){
            yvel = -yvel_MAX-3;
        }

        else{
            yvel = min(G + yvel, yvel_MAX);
        }
    }
    else{
        yvel = min(G+yvel, yvel_MAX);
    }

    ypos += yvel;

}

void Player::render(SDL_Renderer* renderer, Camera &camera, SpriteSheet spritesheet){
    SDL_Rect fillRect{(int)xpos-camera.x+100, (int)ypos, playerWidth, playerHeight};
    //SDL_SetRenderDrawColor( renderer, 0x11, 0x11, 0x66, 0xAA );        
    //SDL_RenderFillRect( renderer, &fillRect );
    if (xvel == 0){
        isMoving = 0; 
    }
    else isMoving = 1;
    if (xvel > 0){
        currentFacing = RIGHT;
    }
    if (xvel < 0){
        currentFacing = LEFT;
    }

    if(isMoving) SDL_RenderCopy(renderer, spritesheet.texture, &spritesheet.Clips[currentFacing][SDL_GetTicks()%500/50], &fillRect);
    else SDL_RenderCopy(renderer, spritesheet.texture, &spritesheet.idleClips[SDL_GetTicks()%600/200], &fillRect);

}


bool detectCollision(Player &player, Walls walls, int* collisionList, Camera camera){
    bool result = false;
    for(int i = 0; i < TOTAL; i++){
        collisionList[i] = 0;
    }
    for(int i = 0; i < walls.wallMap.size(); i++){
        SDL_Rect playerHitbox {(int)player.xpos-camera.x+100, (int)player.ypos, player.playerWidth, player.playerHeight};
        playerHitbox.x += PLAYER_HITBOX_MARGIN;
        playerHitbox.w -= PLAYER_HITBOX_MARGIN*2;
        SDL_Rect wallHitboxCameraShift = walls.wallMap[i];
        wallHitboxCameraShift.x -= camera.x;

        if(abs(playerHitbox.x - wallHitboxCameraShift.x) < playerHitbox.w &&
            abs(playerHitbox.y - wallHitboxCameraShift.y) < player.playerHeight ){
                int ydiff = wallHitboxCameraShift.y - playerHitbox.y;
                if (ydiff < player.playerHeight && ydiff > player.playerHeight-4){
                    collisionList[BOTTOM] = 1;
                    player.ypos -= player.yvel;
                    player.yvel = 0.0;
                }
            result = true;
            }

    }

    for(int i = 0; i < walls.wallMap.size(); i++){
        SDL_Rect wallHitboxCameraShift = walls.wallMap[i];
        wallHitboxCameraShift.x -= camera.x;    
        SDL_Rect playerHitbox {(int)player.xpos-camera.x+100, (int)player.ypos, player.playerWidth-1, player.playerHeight};
        playerHitbox.x += PLAYER_HITBOX_MARGIN;
        playerHitbox.w -= PLAYER_HITBOX_MARGIN*2;
        if (SDL_HasIntersection(&playerHitbox, &wallHitboxCameraShift)){
            int xdiff = wallHitboxCameraShift.x - playerHitbox.x;
            if (xdiff < playerHitbox.w){
                collisionList[SIDE] = 1;
                player.xpos -= xdiff/2;
                player.xvel = 0;

            }
            
            result = true;                
        }
            

    }

    return result;
}





class Enemy{
    public:
    float xpos, ypos, xaccel, xvel, xvel_MAX, x_vel_difference;
    int x_left_max, x_right_max;
    int direction;
    int Width, Height;
    Enemy(float x, float y);
    void render(SDL_Renderer* renderer, Camera camera);
    void move(Walls walls);
};

void Enemy::move(Walls walls){
    if (xpos < x_left_max || xpos > x_right_max){
        direction = -direction;
        
    }
    if(x_right_max-x_left_max>=Width) xpos += xvel_MAX*direction;   
}

Enemy::Enemy(float x, float y){
    xpos = x;
    ypos = y;
    Width = TILE_SIZE;
    Height = TILE_SIZE;
    xaccel = 0.1;
    xvel = 0;
    xvel_MAX = 1;
    x_vel_difference = 0;
    direction = 1;
    
}

void Enemy::render(SDL_Renderer* renderer, Camera camera){
    SDL_Rect fillRect{(int)xpos, (int)ypos, Width, Height};
    SDL_SetRenderDrawColor( renderer, 0xEE, 0x11, 0x44, 0xAA );

    if(xpos >= camera.x-TILE_SIZE && xpos < (camera.w+camera.x)){
        fillRect.x -= camera.x;
        SDL_RenderFillRect( renderer, &fillRect );
    }
}

vector<Enemy> generateEnemies(Walls walls){
    vector <Enemy> enemies;

    for (int i = 1; i < walls.wallMap.size(); i++){

        if ( (i%3) && walls.wallMap[i].y==walls.wallMap[i+2].y  && walls.wallMap[i].y!=walls.wallMap[i-1].y){
            Enemy enemy(walls.wallMap[i].x, walls.wallMap[i].y-TILE_SIZE);
            enemy.x_left_max = walls.wallMap[i].x-1;
            for (int j = i; j < walls.wallMap.size()-1; j++){
                if(walls.wallMap[j].y != walls.wallMap[j+1].y){
                    enemy.x_right_max = walls.wallMap[j].x;
                    break;
                }
            }
            enemies.push_back(enemy);
        }
    }

    return enemies;
}



bool detectCollision(Player &player, vector<Enemy> &enemies, int* smashList, Camera camera){
    bool result = false;
    for(int i = 0; i < TOTAL; i++){
        smashList[i] = 0;
    }
    for(int i = 0; i < enemies.size(); i++){
        SDL_Rect playerHitbox {(int)player.xpos-camera.x+100, (int)player.ypos, player.playerWidth, player.playerHeight};
        playerHitbox.x += PLAYER_HITBOX_MARGIN;
        playerHitbox.w -= PLAYER_HITBOX_MARGIN*2;
        SDL_Rect enemyHitbox = {(int)enemies[i].xpos, (int)enemies[i].ypos, enemies[i].Width, enemies[i].Height};
        enemyHitbox.x -= camera.x;
        if(abs(playerHitbox.x - enemyHitbox.x) < playerHitbox.w &&
            abs(playerHitbox.y - enemyHitbox.y) < player.playerHeight ){
            int ydiff = enemies[i].ypos - playerHitbox.y;
            if (ydiff < player.playerHeight && ydiff > player.playerHeight-4){
                smashList[BOTTOM] = 1;
                enemies.erase(enemies.begin()+i);
                player.score += 0.1;
            }
            
            result = true;                
        }
    }

    for(int i = 0; i < enemies.size(); i++){
        SDL_Rect playerHitbox {(int)player.xpos-camera.x+100, (int)player.ypos, player.playerWidth-1, player.playerHeight};
        playerHitbox.x += PLAYER_HITBOX_MARGIN;
        playerHitbox.w -= PLAYER_HITBOX_MARGIN*2;
        SDL_Rect enemyHitbox = {(int)enemies[i].xpos, (int)enemies[i].ypos, enemies[i].Width, enemies[i].Height};
        enemyHitbox.x -= camera.x;
        if (SDL_HasIntersection(&playerHitbox, &enemyHitbox)){
            int xdiff = enemyHitbox.x - playerHitbox.x;
            if (xdiff < playerHitbox.w){
                smashList[SIDE] = 1;
                player.xvel -= player.xvel_MAX/2 * sign(enemyHitbox.x- player.xpos);
                if(player.health - 0.1 > 0){
                    player.health -=0.1;
                }
                else player.health = 0;
                
            }
            
            result = true;                
        }
            

    }

    return result;
}
