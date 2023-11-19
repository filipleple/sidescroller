#include <iostream>
#include <fstream>
#include "2dgame.h"

using namespace std;

struct pixel {
    int red, green, blue;
};

struct BMPinfo {
    char sygnatura[2];
    short int reserved1;
    short int reserved2;
    int rozmiar;
    short int offset;

    short int infHeadSize;
    int szer;
    int wys;
    short int planes;
    short int bitcount;
    short int comp;
    int rozmiarRysunku;
    short int XPelsPM;
    short int YPelsPM;
    short int clrUses;
    short int clrImportant;
    int l_bajt_0;

};

void wczytaj_info(BMPinfo& info, ifstream& plik) {
    plik.seekg(ios::beg);
    plik.read(reinterpret_cast<char*>(&(info.sygnatura)), 2);
    plik.read(reinterpret_cast<char*>(&(info.rozmiar)), 4);
    plik.read(reinterpret_cast<char*>(&(info.reserved1)), 2);
    plik.read(reinterpret_cast<char*>(&(info.reserved2)), 2);
    plik.read(reinterpret_cast<char*>(&(info.offset)), 4);
    plik.read(reinterpret_cast<char*>(&(info.infHeadSize)), 4);
    plik.read(reinterpret_cast<char*>(&(info.szer)), 4);
    plik.read(reinterpret_cast<char*>(&(info.wys)), 4);
    plik.read(reinterpret_cast<char*>(&(info.planes)), 2);
    plik.read(reinterpret_cast<char*>(&(info.bitcount)), 2);
    plik.read(reinterpret_cast<char*>(&(info.comp)), 4);
    plik.read(reinterpret_cast<char*>(&(info.rozmiarRysunku)), 4);
    plik.read(reinterpret_cast<char*>(&(info.XPelsPM)), 4);
    plik.read(reinterpret_cast<char*>(&(info.YPelsPM)), 4);

    info.l_bajt_0 = info.szer % 4;



}



void wypisz_info(BMPinfo& info, string nazwa) {
    cout << endl << endl << "---" << nazwa << "---" << endl << endl;
    cout << "Typ: " << info.sygnatura << endl;
    cout << "Rozmiar pliku: " << info.rozmiar << endl;
    cout << "Pozycja danych obrazowych w pliku: " << info.offset << endl;
    cout << "Rozmiar naglowka informacyjnego: " << info.infHeadSize << endl;
    cout << "Szerokosc w pikselach: " << info.szer << endl;
    cout << "Wysokosc w pikselach: " << info.wys << endl;
    cout << "Liczba platow: " << info.planes << endl;
    cout << "Liczba bitow na piksel: " << info.bitcount << endl;
    cout << "Algorytm kompresji: " << info.comp << endl;
    cout << "Rozmiar rysunku: " << info.rozmiarRysunku << endl;
    cout << "Rozdzielczosc pozioma: " << info.XPelsPM << endl;
    cout << "Rozdzielczosc pionowa: " << info.YPelsPM << endl;
    cout << endl << "-----------------------" << endl;

}


void delete2d(pixel** piksele, int i_max){
    for (int i = 0; i < i_max/TILE_SIZE; i++){
        delete [] piksele[i];
    }
    delete [] piksele;
}


void wczytaj_piksele(ifstream& plik, BMPinfo info, pixel** piksele) {
    plik.seekg(info.offset, ios::beg);

    unsigned char R, G, B;

    for (int i = 0; i < (info.wys); i++)
    {
        for (int j = 0; j < (info.szer); j++) {
            plik.read((char*)&B, sizeof(unsigned char));
            plik.read((char*)&G, sizeof(unsigned char));
            plik.read((char*)&R, sizeof(unsigned char));

            piksele[i][j].red = int(R);
            piksele[i][j].green = int(G);
            piksele[i][j].blue = int(B);

        }
        if (info.l_bajt_0) {
            for (int i = 0; i < info.l_bajt_0; i++) {
                int czarnek = 0;
                plik.read((char*)&czarnek, sizeof(unsigned char));
            }
        }
    }
}



pixel** wczytajObrazek(BMPinfo& info, string nazwa){
    ifstream bckg;
    bckg.open(nazwa, ios::in | ios::binary);

    if (!bckg) cout << "huujjjj\n";

    wczytaj_info(info, bckg);
    wypisz_info(info, nazwa);
    
    //pixel** piksele = NULL;
    pixel** bg_image = new pixel * [info.wys];
    for (int i = 0; i < info.wys; i++) {
        bg_image[i] = new pixel[info.szer];
    }
    
    wczytaj_info(info, bckg);
    wczytaj_piksele(bckg, info, bg_image);
    bckg.close();

    return bg_image;
}

int** mapParser(int screen_WIDTH, int screen_HEIGHT, string nazwa, int& numberOfTiles){
    ifstream mapIMG;
    mapIMG.open(nazwa, ios::in | ios::binary);
    if (!mapIMG) cout << "huujjjj\n";

    BMPinfo info;
    wczytaj_info(info, mapIMG);
    wypisz_info(info, nazwa);

    numberOfTiles = info.szer*TILE_SIZE/screen_WIDTH;

    pixel** map_image = new pixel * [info.wys];
    for (int i = 0; i < info.wys; i++) {
        map_image[i] = new pixel[info.szer];
    }

    wczytaj_piksele(mapIMG, info, map_image);

    int ** map = new int * [screen_HEIGHT/TILE_SIZE];
    for (int i = 0; i < screen_HEIGHT/TILE_SIZE; i++){
        map[i] = new int [(screen_WIDTH/TILE_SIZE)*numberOfTiles];
    }

    int i_max = screen_HEIGHT/TILE_SIZE;
    int j_max = (screen_WIDTH/TILE_SIZE)*numberOfTiles;

   for (int i = 0; i < i_max; i++){
            for(int j = 0; j < j_max; j++){
                if(map_image[i][j].red == 0){
                    map[(i_max-1)-i][j] = 1; 
                }
                else {
                    map[(i_max-1)-i][j] = 0;
                }
                
            }
            
        }

    mapIMG.close();

    delete2d(map_image, i_max);

    return map;
}