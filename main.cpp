// Author: Dang Vo Hong Phuc - MSSV: 23120155
#include<iostream>
#include<cstring>
#include<fstream>
#include<stdint.h>
using namespace std;

// HEADER FORMAT -> (BYTES TAKEN)
const int HEADER_SIZE = 14;
const int BITMAP_SIGNATURE = 2;
const int FILE_SIZE = 4;
const int RESERVED = 2;
const int OFFSET = 4;

// DIB FORMAT -> (BYTES TAKEN)
const int DIB_BITMAP_SIZE = 40;
const int DIB_SIZE = 4;
const int IMAGE_WIDTH = 4;
const int IMAGE_HEIGHT = 4;
const int COLOR_PLANES = 2; 
const int COLOR_DEPTH = 2;      // bit per pixels
const int COMPRESSION_ALGOR = 4;   
const int PIXEL_ARRAY_SIZE = 4;
const int HRES = 4;             // horizontal resolution
const int VRES = 4;            // vertical resolution
const int NUM_COLORS = 4;
const int NUM_IMPORTANT_COLORS = 4;
int colSize;

// ---------------   FORMAT DATA  ----------------------
struct BMP_MAGIC_SIGN{
    char magic[2];
};

struct BMP_HEADER{
    int32_t file_size;
    int16_t reserved1;
    int16_t reserved2;
    int32_t bmp_offset;
};

struct BMP_DIB{
    uint32_t dib_size;
    int32_t img_width;
    int32_t img_height;
    uint16_t colorPlanes;
    uint16_t colorDepth;
    uint32_t compress;
    uint32_t pixelArraySize;
    int32_t hres;
    int32_t vres;
    uint32_t numColors;
    uint32_t numImportantColors;
    int32_t padding;
};

struct PIXEL{
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

struct BMP_PIXEL_ARRAY{
    PIXEL **pixel;
};

struct BMP_FILE{
    BMP_MAGIC_SIGN MAGIC_SIGN;
    BMP_HEADER HEADER;
    BMP_DIB DIB;
    BMP_PIXEL_ARRAY PIXEL_ARRAY;
};
BMP_FILE* BMP;

struct Data{
    int x,y,width,height;
};
Data *arr;

// ---------------- READ BITMAP FILE --------------------
void readBitmapFile(char *FileName, BMP_FILE* BMP){
    fstream BMP_FILE(FileName, ios::in | ios::binary);
    if (!BMP_FILE.is_open()){
        cout << "Can not open file !!!";
        return;
    }   
    BMP_FILE.read((char *)&BMP->MAGIC_SIGN,2);
    BMP_FILE.read((char *)&BMP->HEADER,HEADER_SIZE - BITMAP_SIGNATURE);
    BMP_FILE.read((char *)&BMP->DIB,DIB_BITMAP_SIZE);
    colSize = (BMP->DIB.img_width * BMP->DIB.colorDepth) / 8;
    BMP->DIB.padding =(4 - (colSize % 4 )) % 4;
    int padding = BMP->DIB.padding;
    char *buff = (char *)malloc(padding * sizeof(char));
    BMP->PIXEL_ARRAY.pixel = new PIXEL *[BMP->DIB.img_height];
    
    for (int i = 0; i < BMP->DIB.img_height; ++i){
        BMP->PIXEL_ARRAY.pixel[i] = new PIXEL [BMP->DIB.img_width];
        BMP_FILE.read((char*)BMP->PIXEL_ARRAY.pixel[i], colSize);
        BMP_FILE.read((char*)buff, padding);
    }
    free(buff);
    BMP_FILE.close();
}

//   ---------------- MAKE BMP FILE --------------------------
void writeFile(char *fileName, BMP_FILE* BMP){
    fstream BMP_FILE(fileName, ios::out | ios::binary);
    if (!BMP_FILE.is_open()){
        cout << "Can not open file !!!";
        return;
    }
    // WRITE HEADER
    BMP_FILE.write(BMP->MAGIC_SIGN.magic,BITMAP_SIGNATURE);
    BMP_FILE.write((char*)&BMP->HEADER.file_size,FILE_SIZE);
    BMP_FILE.write((char*)&BMP->HEADER.reserved1,RESERVED);
    BMP_FILE.write((char*)&BMP->HEADER.reserved2,RESERVED);
    BMP_FILE.write((char*)&BMP->HEADER.bmp_offset,OFFSET);

    // WRITE DIB
    BMP_FILE.write((char*)&BMP->DIB.dib_size,DIB_SIZE);
    BMP_FILE.write((char*)&BMP->DIB.img_width,IMAGE_WIDTH);
    BMP_FILE.write((char*)&BMP->DIB.img_height,IMAGE_HEIGHT);
    BMP_FILE.write((char*)&BMP->DIB.colorPlanes,COLOR_PLANES);
    BMP_FILE.write((char*)&BMP->DIB.colorDepth,COLOR_DEPTH);
    BMP_FILE.write((char*)&BMP->DIB.compress,COMPRESSION_ALGOR);
    BMP_FILE.write((char*)&BMP->DIB.pixelArraySize,PIXEL_ARRAY_SIZE);
    BMP_FILE.write((char*)&BMP->DIB.hres,HRES);
    BMP_FILE.write((char*)&BMP->DIB.vres,VRES);
    BMP_FILE.write((char*)&BMP->DIB.numColors,NUM_COLORS);
    BMP_FILE.write((char*)&BMP->DIB.numImportantColors,NUM_IMPORTANT_COLORS);
    colSize = BMP->DIB.img_width * 3;
    char *trunk = (char*)malloc(BMP->DIB.padding);
    for (int i = 0; i < BMP->DIB.img_height; ++i){
        BMP_FILE.write((char *)BMP->PIXEL_ARRAY.pixel[i],colSize);
        BMP_FILE.write(trunk,BMP->DIB.padding);
    }
    free(trunk);
    BMP_FILE.close();
    cout << "Write sucessful !!!" << '\n';
}


char * createName(int id){
    char *res;
    char tmp[100] = "file";
    char ID[100];
    sprintf(ID, "%d", id);
    char extension[] = ".bmp";
    res = new char [strlen(tmp) + strlen(ID) + strlen(extension) + 1];
    strcpy(res,tmp);
    strcat(res,ID);
    strcat(res,extension);
    return res;
}

void calculate(int h, int w, int height, int width){
    int x_jump = width / w;
    int x_remain = width % w;
    int y_jump = height / h;
    int y_remain = height % h;
    arr = new Data[w*h];
    int tmp = 0;
    for (int i = 0; i < h; ++i){
        for (int j = 0; j < w; ++j){
            arr[tmp].y = i * y_jump;
            arr[tmp].x = j * x_jump;
            arr[tmp].width = x_jump + (j == w-1?x_remain:0);
            arr[tmp].height = y_jump+ (i == h-1?y_remain:0);
            tmp++;
        }
    }
}

void makeFile(char *fileName, BMP_FILE *BMP, Data data){
    BMP_FILE *BMP_TMP = new BMP_FILE;
    BMP_TMP->HEADER = BMP->HEADER;
    BMP_TMP->MAGIC_SIGN = BMP->MAGIC_SIGN;
    BMP_TMP->DIB = BMP->DIB;
    BMP_TMP->DIB.img_width = data.width;
    BMP_TMP->DIB.img_height = data.height;
    BMP_TMP->DIB.padding =(4 - ((data.width * 3) % 4 )) % 4;
    BMP_TMP->DIB.pixelArraySize = data.width * data.height * 3;
    BMP_TMP->HEADER.file_size = BMP_TMP->DIB.pixelArraySize + BMP_TMP->HEADER.bmp_offset;

    BMP_TMP->PIXEL_ARRAY.pixel = new PIXEL*[data.height];
    for (int i = 0; i < data.height; ++i){
        BMP_TMP->PIXEL_ARRAY.pixel[i] = new PIXEL[data.width];
    }
    
    for (int i = data.y, x = 0; x < data.height; ++i, x++){
        memcpy(BMP_TMP->PIXEL_ARRAY.pixel[x],BMP->PIXEL_ARRAY.pixel[i] + data.x,3 * data.width);
    }
    writeFile(fileName,BMP_TMP);
    for(int i = 0; i < data.height; ++i) delete[]BMP_TMP->PIXEL_ARRAY.pixel[i];
    delete[]BMP_TMP->PIXEL_ARRAY.pixel;
}

int main(int argc, char **argv){
    BMP = new BMP_FILE;
    readBitmapFile(argv[1],BMP);
    int h = 1;
    int w = 1;
    if (argv[2][1] == 'h'){
        h = atoi(argv[3]);
    }
    else{
        w = atoi(argv[3]);
    }
    if (argc == 6){
        if (argv[4][1] == 'h'){
            h = atoi(argv[5]);
        }
        else{
            w = atoi(argv[5]);
        }
    }
    calculate(h,w,BMP->DIB.img_height,BMP->DIB.img_width);
    for (int i = 1; i <= h*w; ++i){
        makeFile(createName(i),BMP,arr[i-1]);
    }
    for (int i = 0; i < BMP->DIB.img_height; ++i){
        delete [] BMP->PIXEL_ARRAY.pixel[i];
    }
    delete [] BMP->PIXEL_ARRAY.pixel;
    delete [] arr;
}
