// Lazar Mihai 312CC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} Pixel;

typedef struct tr {
    Pixel p;
    int area;
    struct tr *next_1;
    struct tr *next_2;
    struct tr *next_3;
    struct tr *next_4;
} LTree, *Tree;

typedef struct queue {
    Tree tree;
    int type;
    struct queue *next;
} LQueue, *Queue;

Queue enqueue(Queue rear, Tree p, int type)
{
    // adaug in coada
    Queue new = (Queue) malloc(sizeof(LQueue));
    new->next = NULL;
    new->tree = p;
    new->type = type;

    rear->next = new;
    return rear->next;
}

Queue dequeue(Queue front)
{
    // elimin din coada
    Queue aux = front;
    front = front->next;
    free(aux);
    return front;
}

int is_empty(Queue list)
{
    // verific daca este goala coada
    if (list->next == NULL)
        return 1;
    return 0;
}

int transform(char *s)
{
    // transform din sir de caractere in int
    int nr = 0, i = 0;
    while (s[i] != ' ' && s[i] != '\0') {
        nr = nr * 10 + s[i] - '0';
        i++;
    }
    return nr;
}

void read(Pixel **image, FILE *f, int p6, unsigned int n)
{
    // citesc matricea de pixeli
    int i, j;
    char buf[4];
    // daca este de tipul p6 citesc binar
    if (p6) {
        for (i = 0; i < n; i++) 
            for (j = 0; j < n; j++)
                fread(&image[i][j], sizeof(Pixel), 1, f);
    } else {
        // citesc ascii
        for (i = 0; i < n; i++) 
            for (j = 0; j < n; j++) {
                fscanf(f, "%s", buf);
                image[i][j].red = transform(buf);
                fscanf(f, "%s", buf);
                image[i][j].green = transform(buf);
                fscanf(f, "%s", buf);
                image[i][j].blue = transform(buf);
            }
    }
}


Tree add(Tree tree, int pos) 
{
    //adaug pe pozitia poz un nod in arbore
    Tree new = (Tree) malloc(sizeof(LTree));
    new->next_1 = new->next_2 = new->next_3 = new->next_4 = NULL;
    switch (pos) {
    case 1:
        tree->next_1 = new;
        break;
    case 2:
        tree->next_2 = new;
        break;    
    case 3:
        tree->next_3 = new;
        break;
    case 4:
        tree->next_4 = new;
        break;        
    }
    return tree;
}

unsigned long long avv(Pixel **image, int x, int y, unsigned int size, Pixel *aux)
{
    // calculez mean
    unsigned long long mean = 0, red = 0, green = 0, blue = 0;
    int i, j;
    // aplic formula
    for (i = x; i < x + size; i++)
        for (j = y; j < y + size; j++) {
            red += image[i][j].red;
            green += image[i][j].green;
            blue += image[i][j].blue;
        }
    red = red / (size * size);
    green = green / (size * size);
    blue = blue / (size * size);
    aux->red = red;
    aux->green = green;
    aux->blue = blue;

    for (i = x; i < x + size; i++)
        for (j = y; j < y + size; j++) {
            mean += (red - image[i][j].red) * (red - image[i][j].red) + 
                (green - image[i][j].green) * (green - image[i][j].green) +
                (blue - image[i][j].blue) * (blue - image[i][j].blue);
        }
    mean = floor(mean / (3 * size * size));
    return mean;
}

Tree compress(Pixel **image, Tree tree, int x, int y, unsigned int n, 
                                                            int limit)
{
    Pixel aux;
    if (avv(image, x, y, n, &aux) <= limit) {
        // am gasit suprafata care indeplineste conditia
        tree->p = aux;
        // memorez latura suprafatei ca sa aflu suprafata maxima
        tree->area = n;
        return tree;
    }
    // parcurg recursiv matricea pana indeplinesc conditia
    tree = add(tree, 1);
    compress(image, tree->next_1, x, y, n / 2, limit);

    tree = add(tree, 2);
    compress(image, tree->next_2, x, n / 2 + y, n / 2, limit);

    tree = add(tree, 3);
    compress(image, tree->next_3, n / 2 + x, n / 2 + y, n / 2, limit);

    tree = add(tree, 4);
    compress(image, tree->next_4, n / 2 + x, y, n / 2, limit);

    return tree;
}

void write(Tree tree, int cur, int *max, int *leaves, int *area)
{
    if (tree->next_1 == NULL && tree->next_2 == NULL && 
            tree->next_3 == NULL && tree->next_4 == NULL) {
                if (tree->area > *area)
                    *area = tree->area;
                if (cur > *max)
                    *max = cur;
                (*leaves)++;
            }
    if (tree->next_1 != NULL)
        write(tree->next_1, cur + 1, max, leaves, area);
    if (tree->next_2 != NULL)
        write(tree->next_2, cur + 1, max, leaves, area);
    if (tree->next_3 != NULL)
        write(tree->next_3, cur + 1, max, leaves, area);
    if (tree->next_4 != NULL)
        write(tree->next_4, cur + 1, max, leaves, area);
}

int leaf(Tree tree) {
    // returneaza 1 daca este frunza
    if (tree->next_1 == NULL && tree->next_2 == NULL && 
        tree->next_3 == NULL && tree->next_4 == NULL)
        return 1;
    return 0;
}

void bfs(Tree tree, FILE *g)
{
    Queue front = (Queue) malloc(sizeof(LQueue));
    Queue rear = front;
    rear = enqueue(rear, tree, leaf(tree));
    // adaug in coada radacina arborelui
    Tree aux;
    
    while (!is_empty(front)) {
        // daca este frunza, afisez tipul si pixelul
        if (front->next->type) {
            unsigned char a = 1;
            fwrite(&a, sizeof(unsigned char), 1, g);
            fwrite(&front->next->tree->p, sizeof(Pixel), 1, g);
        } else {
            unsigned char a = 0;
            fwrite(&a, sizeof(unsigned char), 1, g);
        }
        aux = front->next->tree;

        // sterg din coada primul element
        front = dequeue(front);
        
        if (is_empty(front)) {
            rear = front;
        }
        // adaug in coada fii lui
        if (aux->next_1 != NULL) {
            rear = enqueue(rear, aux->next_1, leaf(aux->next_1));
            rear = enqueue(rear, aux->next_2, leaf(aux->next_2));
            rear = enqueue(rear, aux->next_3, leaf(aux->next_3));
            rear = enqueue(rear, aux->next_4, leaf(aux->next_4));
        }
    }
    free(front);
}

Tree add_pixel(Tree tree, Pixel p, int pos)
{
    // adaug pixelul pe pozitia fiului corespunzator
    switch (pos) {
    case 1:
        tree->next_1->p = p;
        break;
    case 2:
        tree->next_2->p = p;
        break;    
    case 3:
        tree->next_3->p = p;
        break;
    case 4:
        tree->next_4->p = p;
        break;        
    }
    return tree;
} 

Tree decompress(FILE *f)
{
    int i;
    Queue front = (Queue) malloc(sizeof(LQueue));
    Queue rear = front;
    Tree tree = (Tree) malloc(sizeof(LTree)); 
    Tree aux = tree;
    tree->next_1 = tree->next_2 = tree->next_3 = tree->next_4 = NULL;
    unsigned char type;
    Pixel p;
    fread(&type, sizeof(unsigned char), 1, f);
    // daca radacina nu are fii, ies din functie si 
    // eliberez zona de memorie alocata pt coada
    if (type) {
        fread(&p, sizeof(Pixel), 1, f);
        tree->p = p;
        free(front);
        return aux;
    }
    // adaug in coada radacina
    rear = enqueue(rear, tree, type);

    while (!is_empty(front)) {
        for (i = 1; i <= 4; i++) {
            // adaug nodul
            tree = add(tree, i);
            fread(&type, sizeof(unsigned char), 1, f);
            if (type) {
                fread(&p, sizeof(Pixel), 1, f);
                tree = add_pixel(tree, p, i);
            } else {
                // nu e frunza, adaug nodul creat in coada
                switch (i) {
                    case 1:
                        rear = enqueue(rear, tree->next_1, 0);
                    break;
                    case 2:
                        rear = enqueue(rear, tree->next_2, 0);
                    break;
                    case 3:
                        rear = enqueue(rear, tree->next_3, 0);
                    break;
                    case 4: 
                        rear = enqueue(rear, tree->next_4, 0);
                    break;
                }
            }
        }
        // elimin primul element din coada
        front = dequeue(front);
        if (is_empty(front)) {
            rear = front;
        } else
            tree = front->next->tree;
        // trec la urmatorul nod din arbore
    }
    free(front);
    return aux;
}

void fill(Pixel **image, int x, int y, int n, Pixel p)
{
    // umplu o zona din matrice cu un anumit pixel
    int i, j;
    for (i = x; i < x + n; i++)
        for (j = y; j < y + n; j++) 
            image[i][j] = p;
}

void dfs(Tree tree, Pixel **image, int x, int y, int n)
{
    // parcurg arborele creat in adancime pana ajung la o frunza
    if (tree->next_1 == NULL && tree->next_2 == NULL && 
            tree->next_3 == NULL && tree->next_4 == NULL) {
        fill(image, x, y, n, tree->p); 
    }
    
    if (tree->next_1 != NULL)
        dfs(tree->next_1, image, x, y, n / 2);
    if (tree->next_2 != NULL)
        dfs(tree->next_2, image, x, n / 2 + y, n / 2);
    if (tree->next_3 != NULL)
        dfs(tree->next_3, image, n / 2 + x, n / 2 + y, n / 2);
    if (tree->next_4 != NULL)
        dfs(tree->next_4, image, n / 2 + x, y, n / 2);
}

void free_tree(Tree tree)
{
    // eliberez zona de memorie alocata arborelui
    if (tree != NULL) {
        free_tree(tree->next_1);
        free_tree(tree->next_2);
        free_tree(tree->next_3);
        free_tree(tree->next_4);
        free(tree);
    }
}

int main(int argc, char const *argv[]) {
    char header[256];
    int i, j;
    unsigned int n;

    char s[10];
    strcpy(s, argv[1]);

    if (!strcmp("-c1", s) || !strcmp("-c2", s)) {
        FILE *f = fopen(argv[3], "rb");
        FILE *g = fopen(argv[4], "wb");

        int k = 0, p6 = 0;
        // citesc primele 3 randuri
        while (k < 3) {
            fgets(header, 255, f);
            if (header[0] != '#')
                k++;
            if (k == 1 && header[1] == '6')
                p6 = 1;
            if (k == 2) 
                n = transform(header);
        }
        // aloc dinamic o matrice de pixeli
        Pixel **image = (Pixel **) malloc(n * sizeof(Pixel *));
        for (i = 0; i < n; i++) {
            image[i] = (Pixel *) malloc(n * sizeof(Pixel));
        }
        // citesc matricea
        read(image, f, p6, n);
        Tree tree;
        tree = (Tree) malloc(sizeof(LTree));
        tree->next_1 = tree->next_2 = tree->next_3 = tree->next_4 = NULL;

        strcpy(s, argv[2]);
        int limit = transform(s);
        // creez arborele de compresie
        tree = compress(image, tree, 0, 0, n, limit);

        if (!strcmp("-c1", argv[1])) {
            // afisez latura suprafatei maxime, numarul de frunze si inaltimea
            int max = 0, leaves = 0, area = 0;
            write(tree, 1, &max, &leaves, &area);
            fprintf(g, "%d\n", max);
            fprintf(g, "%d\n", leaves);
            fprintf(g, "%d\n", area);
        }
        if (!strcmp("-c2", argv[1])) {
            // parcurg in latime arborele
            fwrite(&n, sizeof(unsigned int), 1, g);
            bfs(tree, g);
        }
        // eliberez memoria
        for (i = 0; i < n; i++) 
            free(image[i]);
        free(image);
        free_tree(tree);
        fclose(f);
        fclose(g);
    } else {
        FILE *f = fopen(argv[2], "rb");
        FILE *g = fopen(argv[3], "wb");
        int n;
        fread(&n, sizeof(unsigned int), 1, f);
        Tree tree = decompress(f);
        printf("%d ", n);
        printf("\n");
        Pixel **image = (Pixel **) malloc(n * sizeof(Pixel *));
        for (i = 0; i < n; i++) {
            image[i] = (Pixel *) malloc(n * sizeof(Pixel));
        }

        // parcurg in adancime arborele creat si alcatuiesc matricea
        // de pixeli
        dfs(tree, image, 0, 0, n);


        fprintf(g, "P6\n");
        fprintf(g, "%d %d\n", n, n);
        fprintf(g, "255\n");

        // scriu fisierul ppm
        for (i = 0; i < n; i++) 
            for (j = 0; j < n; j++) 
                fwrite(&image[i][j], sizeof(Pixel), 1, g);

        // eliberez memoria
        for (i = 0; i < n; i++) 
            free(image[i]);
        free(image);
        free_tree(tree);
        fclose(f);
        fclose(g);
    }
    return 0;
}