#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_INTENSITY 256

#ifndef input_file
#define input_file "images/image.ppm"
#endif 

/*_______________________________________________STRUCT_______________________________________________*/
typedef struct color 
{
    unsigned char r, g, b;
} color;

typedef struct picture 
{
    int width, height;
    color* pixels;
} picture;

typedef struct HuffmanTable
{
    char huffman[256][256];
} HuffmanTable;

typedef struct Node 
{
    unsigned char value; //intensité de pixel
    int freq; //fréquence cumulée
    struct Node *left, *right;
} Node;

/*_______________________________________________NEW/SAVE/LOAD_______________________________________________*/
picture new_pic(int width, int height) 
{
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = calloc(width * height, sizeof(color));
    return pic;
}

void save_pic(picture pic, const char* filename) 
{
    FILE* file = fopen(filename, "wb");
    if (!file) 
    {
        perror("Erreur ouverture pour écriture");
        exit(1);
    }

    fprintf(file, "P6\n%d %d\n255\n", pic.width, pic.height);
    fwrite(pic.pixels, sizeof(color), pic.width * pic.height, file);
    fclose(file);
}

picture load_pic(const char* filename) 
{
    FILE* file = fopen(filename, "rb");
    if (!file) 
    {
        perror("Erreur ouverture pour lecture");
        exit(1);
    }

    char ignore[10];
    int width, height, maxval;

    fgets(ignore, sizeof(ignore), file);

    fscanf(file, "%d %d", &width, &height);
    fscanf(file, "%d", &maxval);
    fgetc(file);

    picture pic = new_pic(width, height);
    fread(pic.pixels, sizeof(color), width * height, file);
    fclose(file);
    
    return pic;
}

/*_______________________________________________CREATE_PIC_______________________________________________*/
void create_image() 
{
    int width = 256, height = 256;
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = malloc(sizeof(color)*width*height);

    color r =   {255, 0, 0};
    color g = {0, 255, 0};
    color b =  {0, 0, 255};

    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
            if (x < width / 3)
                pic.pixels[y * width + x] = r;
            else if (x < 2 * width / 3)
                pic.pixels[y * width + x] = g;
            else
                pic.pixels[y * width + x] = b;
        }
    }
    save_pic(pic, "images/image.ppm");
    free(pic.pixels);
}

void create_image_1x1() 
{
    int width = 1, height = 1;
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = malloc(sizeof(color) * width * height);

    color red = {255, 0, 0};
    pic.pixels[0] = red;

    save_pic(pic, "images/image_1x1.ppm");
    free(pic.pixels);
}

/*_______________________________________________NAIVE_CODES_______________________________________________*/
void naive_codes(HuffmanTable* table) 
{
    for (int i = 0; i < MAX_INTENSITY; ++i) 
    {
        for (signed char bit= 7; bit>= 0; --bit) 
        {
            table->huffman[i][7-bit]=((i>>bit)&1)?'1':'0';
        }
        table->huffman[i][8]= '\0';
    }
}

/*_______________________________________________COMPRESS_NAIF_______________________________________________*/
void compress_img_naif(picture pic, const char* filename, HuffmanTable* table) 
{
    FILE* file = fopen(filename, "wb");
    fprintf(file, "P7 %d %d 255 ", pic.width, pic.height);

    for (int i = 0; i < MAX_INTENSITY; ++i) 
    {
        fwrite(table->huffman[i], sizeof(char), MAX_INTENSITY, file);
    }   

    for (int i = 0; i < pic.width * pic.height; ++i) 
    {
        for (int j=0; j<3; ++j) 
        {
            unsigned char intensity;
            if (j==0) intensity = pic.pixels[i].r;
            else if (j== 1) intensity = pic.pixels[i].g;
            else intensity = pic.pixels[i].b;

            unsigned char value = 0;
            for (int b=0;b<8; ++b) 
            {
                value= (value<< 1) | (table->huffman[intensity][b]=='1' ? 1 : 0);
            }
            fputc(value, file);
        }
    }

    fclose(file);
}

/*_______________________________________________DECOMPRESS_NAIF_______________________________________________*/
picture decompress_img_naif(const char* filename, HuffmanTable* table) 
{
    FILE* file = fopen(filename, "rb");
    if (!file) 
    {
        perror("Erreur ouverture fichier HPPM");
        exit(1);
    }

    char header[3];
    int width, height, maxval;
    fscanf(file, "%2s %d %d %d", header, &width, &height, &maxval);
    fgetc(file);

    for (int i = 0; i < MAX_INTENSITY; ++i) 
    {
        fread(table->huffman[i], sizeof(char), MAX_INTENSITY, file);
    }

    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = malloc(sizeof(color) * width * height);

    for (int i = 0; i < width * height; ++i) 
    {
        for (int j = 0; j < 3; ++j) 
        {
            int val = fgetc(file);
            if (j == 0) pic.pixels[i].r = val;
            else if (j == 1) pic.pixels[i].g = val;
            else pic.pixels[i].b = val;
        }
    }
    fclose(file);
    return pic;
}

/*_______________________________________________HISTOGRAMME_______________________________________________*/
float* histogram_img(color* pixels, int total_pixels) 
{
    float* hist = calloc(MAX_INTENSITY, sizeof(float));
    for (int i = 0; i < total_pixels; ++i) 
    {
        hist[pixels[i].r]++;
        hist[pixels[i].g]++;
        hist[pixels[i].b]++;
    }
    float total_values = 3.0f * total_pixels;
    for (int i = 0; i < MAX_INTENSITY; ++i) 
    {
        hist[i] /= total_values;
    }
    return hist;
}

/*_______________________________________________ENTROPIE_______________________________________________*/
float entropy(float* hist) {
    float E = 0.0f;
    for (int i = 0; i < MAX_INTENSITY; ++i) {
        if (hist[i] > 0.0f) {
            E -= hist[i] * log2f(hist[i]);
        }
    }
    return E;
}

/*_______________________________________________BUILD_HUFFMAN_TREE_______________________________________________*/
/* void build_huffman_tree(float* histogram, Node** root) {
Node* nodes[256]; //
// Création des n_noeuds pour chaque intensité présente (value, freq, etc.)
// Construction de l’arbre de Huffman
while(n_noeuds > 1) {
// Recherche des 2 plus petites fréquences
// Création d’un nouveau nœud fusionnant les deux plus petits
// Remplacement des deux anciens nœuds par le nouveau
nodes[min1] = new_node;
nodes[min2] = nodes[n_noeuds-1]; // Déplacer le dernier élément
n_noeuds--;
}
//return le dernier noeud
}
*/


Node* create_node(unsigned char value, int freq, Node* left, Node* right) 
{
    Node* node = malloc(sizeof(Node));
    node->value = value;
    node->freq= freq;
    node->left = left;
    node->right = right;
    return node;
}

void build_huffman_tree(float* histogram, Node** root) 
{
    Node* node[MAX_INTENSITY];
    int n_node = 0;

    for (int i = 0; i < MAX_INTENSITY; i++) 
    {
        if (histogram[i]> 0.0f) 
        {
            int freq = (int)(histogram[i] * 1000000); // éviter les flottants
            node[n_node++] = create_node((unsigned char)i, freq, NULL, NULL);
        }
    }
    while (n_node > 1) 
    {
        int min1 = -1, min2 = -1;
        for (int i = 0; i < n_node; i++) 
        {
            if (min1== -1 || node[i]->freq < node[min1]->freq) 
            {
                min2 = min1;
                min1 = i;
            } 
            else if (min2== -1 || node[i]->freq < node[min2]->freq) 
            {
                min2 = i;
            }
        }

        Node* merged = create_node(0, node[min1]->freq + node[min2]->freq, node[min1], node[min2]);

        node[min1]= merged;
        node[min2]= node[n_node- 1];
        n_node--;
    }
    *root = node[0];
}

/*_______________________________________________GENERATE_HUFFMAN_CODES_______________________________________________*/
void generate_huffman_codes(Node* root, HuffmanTable* table, char* buffer, int depth) 
{
    if (!root) return;
    if (root->left ==NULL && root->right== NULL) 
    {
        buffer[depth] ='\0';
        strcpy(table->huffman[root->value], buffer);
        return;
    }
    buffer[depth]='0';
    generate_huffman_codes(root->left, table, buffer, depth+ 1);

    buffer[depth]='1';
    generate_huffman_codes(root->right, table, buffer, depth+ 1);
}

/*_______________________________________________LONGUEUR_MOYENNE_CODES_______________________________________________*/
float moy_length(float* histogram, HuffmanTable* table) 
{
    float total = 0.0f;
    for (int i = 0; i< MAX_INTENSITY; ++i) 
    {
        if (histogram[i]> 0.0f) 
        {
            int length=strlen(table->huffman[i]);
            total+= histogram[i]* length;
        }
    }
    return total;
}

/*_______________________________________________COMPRESS_HUFFMAN_______________________________________________*/
void compress_img_huffman(picture pic, const char* filename, HuffmanTable* table) 
{
    FILE* file = fopen(filename, "wb");
    if (!file) 
    {
        perror("Erreur ouverture fichier .hppm");
        exit(1);
    }
    fprintf(file, "P7\n%d %d\n255\n", pic.width, pic.height);

    for (int i = 0; i < MAX_INTENSITY; ++i) 
    {
        fwrite(table->huffman[i], sizeof(char), MAX_INTENSITY, file);
    } 
    unsigned char buffer = 0; 
    int bit_count = 0;

    for (int i = 0; i< pic.width * pic.height; ++i) 
    {
        unsigned char values[3] = 
        {
            pic.pixels[i].r,
            pic.pixels[i].g,
            pic.pixels[i].b
        };

        for (int c = 0; c < 3; ++c) 
        {
            char* code = table->huffman[values[c]];

            for (int j = 0; code[j] != '\0'; ++j) 
            {
                buffer <<= 1;
                if (code[j] == '1') 
                {
                    buffer |= 1; 
                }
                bit_count++;

                if (bit_count== 8) 
                {
                    fputc(buffer, file);
                    bit_count= 0;
                    buffer= 0;
                }
            }
        }
    }
     if (bit_count > 0) 
    {
        buffer <<= (8 - bit_count);
        fputc(buffer, file);
    }

    fclose(file);
}


/*_______________________________________________DECOMPRESS_HUFFMAN_______________________________________________*/
Node* build_tree_from_table(HuffmanTable* table) {

    Node* root  = (Node*)malloc(sizeof(Node));
    root->left  = NULL;
    root->right = NULL;

    for (int i = 0; i < MAX_INTENSITY; i++) {
        if (strlen(table->huffman[i]) > 0) { // There is an associated code
            Node* current = root;
            for (size_t j = 0; j < strlen(table->huffman[i]); j++) {
                if (table->huffman[i][j] == '0') {
                    if (!current->left) {
                        current->left = (Node*)malloc(sizeof(Node));
                        current->left->left  = NULL;
                        current->left->right = NULL;
                    }
                    current = current->left;
                } else {
                    if (!current->right) {
                        current->right = (Node*)malloc(sizeof(Node));
                        current->right->left = NULL;
                        current->right->right = NULL;
                    }
                    current = current->right;
                }
            }
            current->value = i; // Set the value to the leaf node
        }
    }
    return root;
}

picture decompress_img_huffman(const char* filename) 
{

    FILE *in = fopen(filename, "rb");
    
    // Header
    char header[10];
    fscanf(in, "%s", header);

    if (strcmp(header, "P7") != 0) 
    {
        fprintf(stderr, "Format de fichier incorrect\n");
        fclose(in);
        exit(1);
    } 
    
    int width, height;
    fscanf(in, "%d %d 255", &width, &height);
    fgetc(in); // Use the newline character
    
    // Image allocation
    picture pic;
    pic.width = width;
    pic.height = height;
    pic.pixels = (color*) malloc(width * height * sizeof(color));
    
    // Coding table reading
    HuffmanTable table;
    //char huffman[256][256] ;
    fread(&table, sizeof(HuffmanTable), 1, in);

    // Reading and decompression of binary codes
    Node* root = build_tree_from_table(&table);

    Node* current = root;
    unsigned char byte;
    int index = 0;
    int c = 0;
    while (fread(&byte, 1, 1, in) == 1) 
    {
        for (signed char bit = 7; bit >= 0; bit--) 
        {
            current = (byte & (1 << bit)) ? current->right : current->left;        

            if (current->left == NULL && current->right == NULL)
            {
                if (c % 3 == 0)
                    pic.pixels[index].r = current->value;
                else if (c % 3 == 1)
                    pic.pixels[index].g = current->value;
                else 
                {
                    pic.pixels[index].b = current->value;
                    index++; // pixel complet
                }
                c++;
                current = root;
            }
        }
    }
    fclose(in);
    return pic;
}



/*_______________________________________________TAILLE_FICHIERS_______________________________________________*/
long file_size(const char* filename) 
{
    struct stat st;
    if (stat(filename,&st) != 0) 
    {
        perror("Impossible de lire la taille du fichier");
        return -1;
    }
    return st.st_size;
}


/*_______________________________________________HISTOGRAMME_PPM_______________________________________________*/
void histogram_ppm(float* hist, const char* filename) 
{
    int width = 256, height = 256;
    picture img;
    img.width= width;
    img.height= height;
    img.pixels= malloc(sizeof(color) *width* height);

    color white = {255,255,255};
    color black = {0,0,0};

    for (int i = 0; i < width * height; i++) 
    {
        img.pixels[i] = white;
    }
    for (int x =0; x< 256; x++) 
    {
        int h = (int)(hist[x] * height);
        for (int y = height- 1; y >= height- h; y--) 
        {
            img.pixels[y *width + x] = black;
        }
    }
    save_pic(img, filename);
    free(img.pixels);
}

/*_______________________________________________MAIN_______________________________________________*/
int main() {

    create_image();     //crée image.ppm de test et l'enregistre dans images/
    create_image_1x1(); //crée image_1x1.ppm de test et l'enregistre dans images/
    picture pic = load_pic(input_file);

#ifdef NAIF

    HuffmanTable huffman ;

// Compression
    naive_codes(&huffman);
    compress_img_naif(pic, "image_naif.hppm", &huffman);

// Décompression
    picture compression_naif = decompress_img_naif("image_naif.hppm", &huffman);
    save_pic(compression_naif, "image_naif_reconstruit.ppm");

// Taille
    long size_ppm = file_size(input_file);
    printf("Taille fichier PPM original : %ld octets\n", size_ppm);
    long size_hppm = file_size("image_naif.hppm");
    printf("Taille fichier compresse : %ld octets\n", size_hppm);

    if (size_ppm > 0 && size_hppm > 0) 
    {
    double ratio = (double)size_hppm / size_ppm * 100.0;
    double gain = 100.0 - ratio;
    printf("Taux de compression : %.2f%%\n", ratio);
    printf("Gain de taille : %.2f%%\n", gain);
    }

    free(compression_naif.pixels);
    printf("Compression/Decompression terminee.\n");


#elif defined(STATS)

// Histogramme 
    float* histogram = histogram_img(pic.pixels, pic.width * pic.height);
    for (int i = 0; i < 256; ++i) 
    {
        printf("Intensite %d : %.4f\n", i, histogram[i]);
    }

// Entropie 
    float E = entropy(histogram);
    printf("Entropie : %.4f bits\n", E);
    Node* root = NULL;
    build_huffman_tree(histogram, &root);

// Codes associés à l'intensité     
    HuffmanTable huffman = {.huffman = 0};
    char buffer[256];
    generate_huffman_codes(root, &huffman, buffer, 0);
    for (int i = 0; i < 256; ++i) 
    {
        if (huffman.huffman[i][0] != '\0') 
        {
            printf("Intensite %3d : %s\n", i, huffman.huffman[i]);
        }
    }

// Longueur Moyenne
    float longueur_moy = moy_length(histogram, &huffman);
    printf("Longueur moyenne des codes : %.4f bits\n", longueur_moy);
    
// Histogramme PPM
    histogram_ppm(histogram, "histogram.ppm");
    printf("Histogramme enregistre\n");
    
    free(histogram);

#elif defined(COMPRESS_HUFFMAN)

// Compression
    float* histogram = histogram_img(pic.pixels, pic.width * pic.height);
    Node* root = NULL;
    build_huffman_tree(histogram, &root);

    HuffmanTable huffman = {.huffman = 0};
    char buffer[256];
    generate_huffman_codes(root, &huffman, buffer, 0);
    compress_img_huffman(pic, "image_huffman.hppm", &huffman);
    
// Taille
    long size_ppm = file_size(input_file);
    printf("Taille fichier PPM original : %ld octets\n", size_ppm);
    long size_hppm = file_size("image_huffman.hppm");
    printf("Taille fichier compresse : %ld octets\n", size_hppm);

    if (size_ppm > 0 && size_hppm > 0) 
    {
    double ratio = (double)size_hppm / size_ppm * 100.0;
    double gain = 100.0 - ratio;
    printf("Taux de compression : %.2f%%\n", ratio);
    printf("Gain de taille : %.2f%%\n", gain);
    }

    free(histogram);
    printf("Compression terminee.\n");


#elif defined(DECOMPRESS_HUFFMAN)

// Déompression
    picture compression_huffman = decompress_img_huffman("image_huffman.hppm");
    save_pic(compression_huffman, "image_huffman_reconstruit.ppm");

// Taille     
    long size_ppm = file_size("images/image.ppm");
    printf("Taille fichier PPM original : %ld octets\n", size_ppm);
    long size_reconstruit_ppm = file_size("image_huffman_reconstruit.ppm");
    printf("Taille fichier PPM decompresse : %ld octets\n", size_reconstruit_ppm);

    free(compression_huffman.pixels);
    printf("Decompression terminee.\n");


#endif
    free(pic.pixels);
    return 0;
}